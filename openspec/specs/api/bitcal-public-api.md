# API Specification: BitCal Public Interface

## Version

v2.1.0

## Namespace

All public APIs are in the `bitcal` namespace.

## Core Types

### bitarray Template

```cpp
namespace bitcal {
    template<size_t Bits, simd_backend Backend = get_default_backend()>
    class bitarray;
}
```

**Template Parameters:**
| Parameter | Type | Constraints | Description |
|-----------|------|-------------|-------------|
| `Bits` | `size_t` | Multiple of 64 | Total number of bits |
| `Backend` | `simd_backend` | Any valid backend | SIMD implementation to use |

**Static Assertions:**
```cpp
static_assert(Bits % 64 == 0, "Bits must be multiple of 64");
```

### Predefined Type Aliases

```cpp
namespace bitcal {
    using bit64   = bitarray<64>;
    using bit128  = bitarray<128>;
    using bit256  = bitarray<256>;
    using bit512  = bitarray<512>;
    using bit1024 = bitarray<1024>;
}
```

### SIMD Backend Enumeration

```cpp
namespace bitcal {
    enum class simd_backend {
        scalar,  // Portable scalar implementation
        sse2,    // x86 SSE2 (128-bit)
        avx,     // x86 AVX (256-bit transitional backend)
        avx2,    // x86 AVX2 (256-bit)
        avx512,  // x86 AVX-512 (partial support)
        neon     // ARM NEON (128-bit)
    };
}
```

## Constructors

### Default Constructor

```cpp
bitarray();
```

Initializes all bits to zero.

### Value Constructor

```cpp
explicit bitarray(uint64_t value);
```

Initializes the lowest 64 bits with `value`, clears the rest.

**Parameters:**
- `value`: Lower 64 bits; upper bits zero-extended

### Copy and Move

```cpp
bitarray(const bitarray& other);           // Copy constructor
bitarray(bitarray&& other) noexcept;       // Move constructor
bitarray& operator=(const bitarray& other);      // Copy assignment
bitarray& operator=(bitarray&& other) noexcept;  // Move assignment
```

## Data Access

### Array Subscript

```cpp
uint64_t  operator[](size_t index) const;
uint64_t& operator[](size_t index);
```

Access individual 64-bit words (0-indexed, LSB is word 0).

**Preconditions:**
```cpp
assert(index < u64_count);  // UB if violated
```

### Data Pointer

```cpp
uint64_t* data() noexcept;
const uint64_t* data() const noexcept;
```

Direct access to underlying storage (64-byte aligned, contiguous).

### Size Constants

```cpp
static constexpr size_t bits = Bits;           // Total bit width
static constexpr size_t u64_count = Bits / 64; // Number of 64-bit words
```

## Bitwise Operators

### AND

```cpp
bitarray operator&(const bitarray& other) const;
bitarray& operator&=(const bitarray& other);
```

### OR

```cpp
bitarray operator|(const bitarray& other) const;
bitarray& operator|=(const bitarray& other);
```

### XOR

```cpp
bitarray operator^(const bitarray& other) const;
bitarray& operator^=(const bitarray& other);
```

### NOT

```cpp
bitarray operator~() const;
```

Bitwise inversion (accelerated by SIMD XOR with all-ones since v2.1).

## Shift Operators

### Left Shift

```cpp
bitarray operator<<(int count) const;
bitarray& operator<<=(int count);
void shift_left(int count);
```

Shift bits toward more significant positions, filling with zeros.

### Right Shift

```cpp
bitarray operator>>(int count) const;
bitarray& operator>>=(int count);
void shift_right(int count);
```

Logical right shift (fill with zeros).

### Boundary Behavior

| Condition | Behavior |
|-----------|----------|
| `count == 0` | No operation |
| `count >= Bits` | All bits cleared |
| `count < 0` | Undefined behavior |

## Comparison Operators

### Equality

```cpp
bool operator==(const bitarray& other) const;
```

### Inequality

```cpp
bool operator!=(const bitarray& other) const;
```

## Special Operations

### ANDNOT

```cpp
bitarray andnot(const bitarray& mask) const;
```

Computes `*this & ~mask` using native SIMD instructions.

**Performance:** ~2× faster than separate NOT + AND.

### is_zero

```cpp
bool is_zero() const;
```

Check if all bits are zero (SIMD-accelerated since v2.1).

### clear

```cpp
void clear();
```

Set all bits to zero (uses `std::memset`).

## Bit Counting

### popcount

```cpp
uint64_t popcount() const;
```

Count number of bits set to 1.

**Implementation:** Uses `__builtin_popcountll` (GCC/Clang) or `__popcnt64` (MSVC).

### count_leading_zeros

```cpp
int count_leading_zeros() const;
```

Count consecutive zeros from most significant bit.

**Returns:**
- `Bits` if all zeros
- `0` if MSB is 1

### count_trailing_zeros

```cpp
int count_trailing_zeros() const;
```

Count consecutive zeros from least significant bit.

**Returns:**
- `Bits` if all zeros
- `0` if LSB is 1

## Single Bit Operations

### get_bit

```cpp
bool get_bit(size_t bit_index) const;
```

Read bit at position `bit_index` (0-indexed from LSB).

**Complexity:** O(1)

### set_bit

```cpp
void set_bit(size_t bit_index, bool value = true);
```

Set bit at position `bit_index`.

**Parameters:**
- `bit_index`: Position (0 to Bits-1)
- `value`: `true` to set, `false` to clear

**Complexity:** O(1)

### flip_bit

```cpp
void flip_bit(size_t bit_index);
```

Toggle bit at position `bit_index`.

**Complexity:** O(1)

## Bit Manipulation

### reverse

```cpp
void reverse();
```

Reverse all bits: bit 0 swaps with bit (Bits-1), etc.

**Complexity:** O(Bits/64)

**Implementation:** In-place operation (v2.1+).

## Functional API (ops Namespace)

Low-level operations on raw `uint64_t*` pointers:

```cpp
namespace bitcal::ops {
    template<size_t Bits>
    uint64_t popcount(const uint64_t* data);

    template<size_t Bits>
    int count_leading_zeros(const uint64_t* data);

    template<size_t Bits>
    int count_trailing_zeros(const uint64_t* data);

    uint64_t reverse_bits_64(uint64_t x);
    uint64_t byte_swap_64(uint64_t x);
}
```

## Backend Selection

### Automatic Selection

```cpp
simd_backend get_default_backend() noexcept;
```

Returns optimal backend based on compiler flags.

**Priority:** AVX-512 → AVX2 → AVX → SSE2 → NEON → Scalar

### Manual Specification

```cpp
bitcal::bitarray<256, bitcal::simd_backend::avx2> arr;  // Force AVX2
bitcal::bitarray<128, bitcal::simd_backend::neon> arr;  // Force NEON
bitcal::bitarray<256, bitcal::simd_backend::scalar> arr; // Force scalar
```

## Type Traits

### is_bitarray

```cpp
template<typename T>
struct is_bitarray : std::false_type {};

template<size_t Bits, simd_backend Backend>
struct is_bitarray<bitarray<Bits, Backend>> : std::true_type {};

template<typename T>
inline constexpr bool is_bitarray_v = is_bitarray<T>::value;
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
};
```

## Memory Layout

```
bitarray<256> memory:
┌──────────────────────────────────────────────────────────┐
│ Alignment │ Word 0 │ Word 1 │ Word 2 │ Word 3 │ Padding │
│ 64 bytes  │ 0-63   │ 64-127 │128-191 │192-255 │ to 64B  │
└──────────────────────────────────────────────────────────┘
```

- **Alignment:** 64 bytes (cache-line aligned)
- **Endianness:** Little-endian (LSB in `data_[0]`)
- **Storage:** Contiguous `uint64_t` array

## Error Handling

- No bounds checking on `bit_index` in single bit operations (UB if out of range)
- Negative shift count is undefined behavior
- Unsupported backend/width combinations fall back to scalar

## Thread Safety

- Different threads on **different** instances: ✅ Thread-safe
- Read-only access to **shared** instance: ✅ Thread-safe
- Concurrent read/write on **same** instance: ❌ Requires synchronization

## Contract Governance Requirements

### Requirement: Public API changes SHALL be explicitly versioned and documented
BitCal SHALL record any breaking public API or behavioral change in its API specification, migration-facing documentation, and canonical versioning source.

#### Scenario: A public API contract changes
- **WHEN** a method signature, type alias, semantic behavior, or supported contract changes in a breaking way
- **THEN** the API specification MUST describe the new contract and the project documentation MUST identify the migration implication

### Requirement: Public API documentation SHALL stay synchronized with implementation
BitCal SHALL keep user-visible API documentation aligned with the implementation and tests that define retained behavior.

#### Scenario: A documented API remains in the project
- **WHEN** an API surface is documented as supported
- **THEN** the implementation and test suite MUST still cover that documented behavior or the documentation MUST be removed or corrected

## Backward Compatibility

- v2.1.0 is 100% backward compatible with v2.0.x
- No breaking changes planned for v2.x series
- Migration guide available for v1.x → v2.x
