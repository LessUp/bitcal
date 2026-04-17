# Types Reference

Complete reference for BitCal types, templates, and type aliases.

## Table of Contents

- [bitarray Template](#bitarray-template)
- [Predefined Types](#predefined-types)
- [Constructors](#constructors)
- [Data Access](#data-access)
- [Static Members](#static-members)
- [SIMD Backend Enum](#simd-backend-enum)
- [Type Traits](#type-traits)

---

## bitarray Template

The core class template for all bit array operations.

```cpp
namespace bitcal {
    template<size_t Bits, simd_backend Backend = get_default_backend()>
    class bitarray;
}
```

### Template Parameters

| Parameter | Type | Constraints | Description |
|-----------|------|-------------|-------------|
| `Bits` | `size_t` | Multiple of 64 | Total number of bits |
| `Backend` | `simd_backend` | Any valid backend | SIMD implementation to use |

**Static Assertions:**
```cpp
static_assert(Bits % 64 == 0, "Bits must be multiple of 64");
```

### Memory Layout

```
bitarray<256, avx2> memory representation:
┌──────────────────────────────────────────────────────────────────────┐
│ Alignment  │  Word 0   │  Word 1   │  Word 2   │  Word 3   │ Padding │
│  64 bytes  │ bits 0-63 │bits 64-127│bits 128-191│bits 192-255│to 64B  │
└──────────────────────────────────────────────────────────────────────┘
                    Little-endian: word 0 holds LSB
```

**Key Properties:**
- **Alignment:** 64 bytes (cache-line aligned)
- **Endianness:** Little-endian (LSB in `data_[0]`)
- **Storage:** Contiguous `uint64_t` array
- **Padding:** Extra space to maintain alignment

---

## Predefined Types

Convenience type aliases for common bit widths:

| Type Alias | Definition | Use Case | Optimal Platform |
|------------|------------|----------|------------------|
| `bit64` | `bitarray<64>` | Machine word operations | All platforms |
| `bit128` | `bitarray<128>` | SSE2/NEON native width | x86-SSE2, ARM-NEON |
| `bit256` | `bitarray<256>` | AVX2 native width | x86-AVX2 |
| `bit512` | `bitarray<512>` | Large parallel operations | AVX2 (two registers) |
| `bit1024` | `bitarray<1024>` | Very large operations | AVX2 (four registers) |

### Declarations

```cpp
namespace bitcal {
    using bit64   = bitarray<64>;
    using bit128  = bitarray<128>;
    using bit256  = bitarray<256>;
    using bit512  = bitarray<512>;
    using bit1024 = bitarray<1024>;
}
```

### Performance Characteristics

```cpp
// bit64 - Always scalar (single u64)
bitcal::bit64 a(0xFF);  // Fastest for small data

// bit128 - 128-bit SIMD optimal
bitcal::bit128 b;  // Single SSE/NEON register

// bit256 - 256-bit SIMD optimal
bitcal::bit256 c;  // Single AVX register, 2× SSE, or 2× NEON

// bit512 - 512-bit width
bitcal::bit512 d;  // 2× AVX or 4× SSE/NEON

// bit1024 - Large width
bitcal::bit1024 e;  // Loop unrolled operations
```

**Performance Table (Intel i7-12700K, AVX2):**

| Type | AND Latency | Memory | Backend Used |
|------|-------------|--------|--------------|
| bit64 | ~1ns | 8 bytes | Scalar |
| bit128 | ~1.5ns | 16+48 bytes | SSE2 |
| bit256 | ~2.1ns | 32+32 bytes | AVX2 |
| bit512 | ~4.2ns | 64 bytes | AVX2 (2 ops) |
| bit1024 | ~8.5ns | 128 bytes | AVX2 (4 ops) |

---

## Constructors

### Default Constructor

Initializes all bits to zero.

```cpp
bitarray();
```

**Example:**
```cpp
bitcal::bit256 arr;  // All 256 bits are 0
```

**Performance:** Uses SIMD-optimized `clear()` internally.

### Value Constructor

Initializes the lowest 64 bits, clears the rest.

```cpp
explicit bitarray(uint64_t value);
```

| Parameter | Description |
|-----------|-------------|
| `value` | Lower 64 bits; upper bits zero-extended |

**Example:**
```cpp
bitcal::bit256 arr(0xDEADBEEF);
// arr[0] = 0xDEADBEEF
// arr[1..] = 0
```

### Copy & Move Constructors

```cpp
bitarray(const bitarray& other);           // Copy
bitarray(bitarray&& other) noexcept;       // Move
```

Both are trivial and compiler-generated. Move is `noexcept` for exception safety.

### Assignment Operators

```cpp
bitarray& operator=(const bitarray& other);      // Copy assignment
bitarray& operator=(bitarray&& other) noexcept;  // Move assignment
```

---

## Data Access

### Array Subscript Operator

Access individual 64-bit words.

```cpp
uint64_t  operator[](size_t index) const;
uint64_t& operator[](size_t index);
```

| Parameter | Range | Description |
|-----------|-------|-------------|
| `index` | `[0, Bits/64 - 1]` | Word index (0 = LSB) |

**Preconditions:**
```cpp
assert(index < u64_count);  // UB if violated
```

**Example:**
```cpp
bitcal::bit256 arr(0xDEADBEEF);

arr[0] = 0x12345678;      // Set lowest 64 bits
arr[1] = 0x9ABCDEF0;      // Set bits 64-127
arr[2] = arr[0] | arr[1]; // Read-modify-write

uint64_t low = arr[0];    // Read word
```

### Data Pointer

Direct access to underlying storage.

```cpp
uint64_t* data() noexcept;
const uint64_t* data() const noexcept;
```

**Properties:**
- Pointer is aligned to 64 bytes
- Points to contiguous `uint64_t` array
- Valid for `u64_count` elements

**Example:**
```cpp
bitcal::bit256 arr;
uint64_t* ptr = arr.data();

// Direct manipulation
ptr[0] = 0xFFFFFFFFFFFFFFFF;
std::memcpy(ptr, source, bitcal::bit256::u64_count * sizeof(uint64_t));

// Interoperability with C APIs
external_function(ptr, arr.u64_count);
```

### Size Information

```cpp
static constexpr size_t u64_count = Bits / 64;  // Number of 64-bit words
```

---

## Static Members

Compile-time constants for type introspection:

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
public:
    static constexpr size_t bits = Bits;                    // Total bit width
    static constexpr size_t u64_count = Bits / 64;          // 64-bit word count
    static constexpr simd_backend backend = Backend;        // Selected backend
};
```

**Usage:**
```cpp
using MyType = bitcal::bit256;

static_assert(MyType::bits == 256);
static_assert(MyType::u64_count == 4);
static_assert(MyType::backend == bitcal::simd_backend::avx2 ||
              MyType::backend == bitcal::simd_backend::sse2);

// Template metaprogramming
template<typename T>
using is_bitcal_type = std::bool_constant<
    requires { typename T::backend; }
>;
```

---

## SIMD Backend Enum

Enumeration of available SIMD backends.

```cpp
namespace bitcal {
    enum class simd_backend {
        scalar,  // Portable scalar implementation
        sse2,    // x86 SSE2 (128-bit)
        avx2,    // x86 AVX2 (256-bit)
        neon     // ARM NEON (128-bit)
    };
}
```

### Backend Selection

```cpp
// Automatic selection based on compiler flags
template<size_t Bits>
constexpr simd_backend select_best_backend() {
    #if defined(BITCAL_HAS_AVX2) && Bits >= 256
        return simd_backend::avx2;
    #elif defined(BITCAL_HAS_SSE2) && Bits >= 128
        return simd_backend::sse2;
    #elif defined(BITCAL_HAS_NEON) && Bits >= 128
        return simd_backend::neon;
    #else
        return simd_backend::scalar;
    #endif
}
```

### Default Backend

```cpp
simd_backend get_default_backend() noexcept;
```

Returns the backend selected at compile time for the current platform.

**Example:**
```cpp
auto backend = bitcal::get_default_backend();
switch (backend) {
    case bitcal::simd_backend::avx2:  std::cout << "AVX2\n"; break;
    case bitcal::simd_backend::sse2:  std::cout << "SSE2\n"; break;
    case bitcal::simd_backend::neon:  std::cout << "NEON\n"; break;
    default:                          std::cout << "Scalar\n";
}
```

---

## Type Traits

BitCal provides type traits for template metaprogramming.

### is_bitarray

```cpp
template<typename T>
struct is_bitarray : std::false_type {};

template<size_t Bits, simd_backend Backend>
struct is_bitarray<bitarray<Bits, Backend>> : std::true_type {};

template<typename T>
inline constexpr bool is_bitarray_v = is_bitarray<T>::value;
```

**Example:**
```cpp
static_assert(bitcal::is_bitarray_v<bitcal::bit256>);
static_assert(!bitcal::is_bitarray_v<int>);

// SFINAE
std::enable_if_t<bitcal::is_bitarray_v<T>, void>
process_bitarray(const T& arr);
```

### bitarray_traits

```cpp
template<typename T>
struct bitarray_traits;

template<size_t Bits, simd_backend Backend>
struct bitarray_traits<bitarray<Bits, Backend>> {
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;
    
    using word_type = uint64_t;
    using backend_type = /* implementation-defined */;
};
```

---

## Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>
#include <iomanip>
#include <type_traits>

template<typename T>
void print_array_info(const char* name) {
    std::cout << name << ":\n";
    std::cout << "  bits: " << T::bits << "\n";
    std::cout << "  words: " << T::u64_count << "\n";
    std::cout << "  backend: ";
    switch (T::backend) {
        case bitcal::simd_backend::avx2:  std::cout << "AVX2"; break;
        case bitcal::simd_backend::sse2:  std::cout << "SSE2"; break;
        case bitcal::simd_backend::neon:  std::cout << "NEON"; break;
        default:                          std::cout << "Scalar";
    }
    std::cout << "\n\n";
}

void print_hex(const bitcal::bit256& arr) {
    std::cout << "0x";
    for (int i = 3; i >= 0; --i) {
        std::cout << std::hex << std::setfill('0') << std::setw(16) 
                  << arr[i];
    }
    std::cout << std::dec << "\n";
}

int main() {
    // Type information
    print_array_info<bitcal::bit64>("bit64");
    print_array_info<bitcal::bit128>("bit128");
    print_array_info<bitcal::bit256>("bit256");
    print_array_info<bitcal::bit1024>("bit1024");
    
    // Custom type
    using custom_512 = bitcal::bitarray<512, bitcal::simd_backend::scalar>;
    print_array_info<custom_512>("bitarray<512, scalar>");
    
    // Construction and access
    bitcal::bit256 a;  // Default: all zeros
    print_hex(a);      // 0x00000000000000000000000000000000
    
    bitcal::bit256 b(0xDEADBEEF);
    print_hex(b);      // 0x000000000000000000000000DEADBEEF
    
    // Word access
    b[1] = 0xCAFEBABE;
    print_hex(b);      // 0x00000000CAFEBABE00000000DEADBEEF
    
    // Type traits
    static_assert(bitcal::is_bitarray_v<bitcal::bit256>);
    static_assert(bitcal::bitarray_traits<bitcal::bit256>::bits == 256);
    
    std::cout << "\n✓ All type operations successful\n";
    return 0;
}
```

---

## See Also

- [Core Operations](core-operations.html) — Bitwise operations
- [SIMD Backend](simd-backend.html) — Backend selection details
- [Architecture Overview](../architecture/overview.html) — Design principles
