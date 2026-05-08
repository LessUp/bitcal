# Types Reference

Complete reference for the retained BitCal 3.0 public type surface.

> **BitCal 3.0 migration:** `bitcal::ops`, `is_bitarray`, `is_bitarray_v`, `bitarray_traits`, and the explicit `bit64` conversion helper are no longer public API. Use `bitarray` member functions and `word(0)` when you need a 64-bit value.

## Table of Contents

- [bitarray Template](#bitarray-template)
- [Predefined Types](#predefined-types)
- [Constructors](#constructors)
- [Data Access](#data-access)
- [Static Members](#static-members)
- [SIMD Backend Enum](#simd-backend-enum)

---

## bitarray Template

The core class template for all public BitCal operations.

```cpp
namespace bitcal {
    template<size_t Bits,
             simd_backend Backend = (Bits == 64 ? simd_backend::scalar : get_default_backend())>
    class bitarray;
}
```

### Template Parameters

| Parameter | Type | Constraints | Description |
|-----------|------|-------------|-------------|
| `Bits` | `size_t` | `Bits >= 64` and a multiple of 64 | Total number of bits |
| `Backend` | `simd_backend` | Available backend for the target build | Backend selected for operations |

**Static Assertions:**
```cpp
static_assert(Bits >= 64, "Bits must be at least 64");
static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");
```

### Memory Layout

```
bitarray<256> memory representation:
┌──────────────────────────────────────────────────────────────────────┐
│ Alignment  │  Word 0   │  Word 1   │  Word 2   │  Word 3   │ Padding │
│  32 bytes  │ bits 0-63 │bits 64-127│bits 128-191│bits 192-255│to align│
└──────────────────────────────────────────────────────────────────────┘
                    Little-endian: word 0 holds the LSB range
```

**Key Properties:**
- Alignment is selected by `get_optimal_alignment<Bits>()`
- Storage is contiguous `uint64_t` words
- The public API exposes read-only raw access through `data()`

---

## Predefined Types

Convenience aliases for common bit widths:

| Type Alias | Definition | Use Case | Typical Backend |
|------------|------------|----------|-----------------|
| `bit64` | `bitarray<64>` | Single-word operations | Scalar |
| `bit128` | `bitarray<128>` | 128-bit SIMD width | SSE2 / NEON / AVX-512 VL |
| `bit256` | `bitarray<256>` | 256-bit SIMD width | AVX2 / AVX-512 VL |
| `bit512` | `bitarray<512>` | Large parallel operations | AVX-512 or looped AVX2 |
| `bit1024` | `bitarray<1024>` | Very large fixed-width operations | Loop over chosen backend |

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

---

## Constructors

### Default Constructor

Initializes all bits to zero.

```cpp
bitarray() noexcept;
```

### Value Constructor

Initializes the lowest 64 bits and clears the remaining words.

```cpp
explicit bitarray(uint64_t value) noexcept;
```

| Parameter | Description |
|-----------|-------------|
| `value` | Stored in word 0; higher words are zero-filled |

### Copy and Move Operations

```cpp
bitarray(const bitarray& other) noexcept = default;
bitarray(bitarray&& other) noexcept = default;
bitarray& operator=(const bitarray& other) noexcept = default;
bitarray& operator=(bitarray&& other) noexcept = default;
```

---

## Data Access

### Raw Data Pointer

Read-only access to the underlying storage.

```cpp
const uint64_t* data() const noexcept;
```

**Properties:**
- Pointer remains valid for the lifetime of the object
- Contains `u64_count` contiguous words
- Use `set_word()` for mutation rather than writing through the pointer

### Word Access Helpers

```cpp
uint64_t word(size_t index) const noexcept;
void set_word(size_t index, uint64_t value) noexcept;
uint64_t operator[](size_t index) const noexcept;
```

| API | Description |
|-----|-------------|
| `word(index)` | Read a single 64-bit word with bounds assertions in debug builds |
| `set_word(index, value)` | Replace one 64-bit word |
| `operator[](index)` | Read-only indexed access |

**Example:**
```cpp
bitcal::bit256 arr(0xDEADBEEF);
arr.set_word(1, 0xCAFEBABE);

uint64_t low = arr.word(0);
uint64_t high = arr[1];
const uint64_t* ptr = arr.data();
```

---

## Static Members

Compile-time constants available on every `bitarray` specialization:

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
public:
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;
};
```

**Example:**
```cpp
using MyType = bitcal::bit256;

static_assert(MyType::bits == 256);
static_assert(MyType::u64_count == 4);
```

---

## SIMD Backend Enum

Available backend tags:

```cpp
namespace bitcal {
    enum class simd_backend {
        scalar,
        sse2,
        avx2,
        avx512,
        neon
    };
}
```

### Default Backend

```cpp
constexpr simd_backend get_default_backend() noexcept;
```

Returns the backend selected at compile time for the current build.

**Example:**
```cpp
auto backend = bitcal::get_default_backend();
switch (backend) {
    case bitcal::simd_backend::avx512: std::cout << "AVX-512\n"; break;
    case bitcal::simd_backend::avx2:   std::cout << "AVX2\n"; break;
    case bitcal::simd_backend::sse2:   std::cout << "SSE2\n"; break;
    case bitcal::simd_backend::neon:   std::cout << "NEON\n"; break;
    default:                           std::cout << "Scalar\n";
}
```

---

## Complete Example

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 arr(0xDEADBEEF);
    arr.set_word(1, 0xCAFEBABE);

    std::cout << "bits: " << bitcal::bit256::bits << "\n";
    std::cout << "words: " << bitcal::bit256::u64_count << "\n";
    std::cout << "word0: 0x" << std::hex << arr.word(0) << "\n";
    std::cout << "word1: 0x" << std::hex << arr[1] << "\n";

    return 0;
}
```

---

## See Also

- [Core Operations](core-operations.md) — bitwise operators and ANDNOT
- [Bit Counting](bit-counting.md) — popcount, CLZ, and CTZ
- [Architecture Overview](../architecture/overview.md) — high-level implementation layout
