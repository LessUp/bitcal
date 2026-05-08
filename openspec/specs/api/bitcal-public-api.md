# API Specification: BitCal Public Interface

## Contract Target

Planned breaking contract for **v3.0.0**.

This specification freezes the retained public API that BitCal intends to keep after the public-surface contraction. During the transition, implementation and tests may still carry legacy helpers, but only the API described here remains part of the supported compatibility contract.

## Namespace

All retained public APIs live in the `bitcal` namespace.

## Stable Include Contract

```cpp
#include <bitcal/bitcal.hpp>
```

`<bitcal/bitcal.hpp>` is the only stable public include seam.

Implementation notes:
- The retained `bitarray` definition is planned to live in `include/bitcal/bitarray.hpp`.
- `bitcal/bitarray.hpp`, `config.hpp`, `backend_ops.hpp`, and `scalar_ops.hpp` are not separate stable include contracts.
- Documentation and examples must present the umbrella header as the supported consumption path.

## Retained Core Types

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
| `Bits` | `size_t` | `Bits >= 64` and multiple of 64 | Total number of bits |
| `Backend` | `simd_backend` | A retained backend enum value | Compile-time backend selection |

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
        scalar,
        sse2,
        avx2,
        avx512,
        neon
    };
}
```

`simd_backend::avx` is not part of the retained public contract.

### Default Backend Selection

```cpp
namespace bitcal {
    constexpr simd_backend get_default_backend() noexcept;
}
```

Selection priority follows the retained implementation path:

`avx512 -> avx2 -> sse2 -> neon -> scalar`

## Retained bitarray Constructors and Constants

```cpp
bitarray() noexcept;
explicit bitarray(uint64_t value) noexcept;
bitarray(const bitarray&) noexcept;
bitarray(bitarray&&) noexcept;
bitarray& operator=(const bitarray&) noexcept;
bitarray& operator=(bitarray&&) noexcept;

static constexpr size_t bits = Bits;
static constexpr size_t u64_count = Bits / 64;
static constexpr simd_backend backend = Backend;
```

## Retained Data Access

```cpp
const uint64_t* data() const noexcept;
uint64_t word(size_t index) const noexcept;
void set_word(size_t index, uint64_t value) noexcept;
uint64_t operator[](size_t index) const noexcept;
```

Contract notes:
- storage is contiguous `uint64_t` word storage
- word order is little-endian (`word 0` holds the least significant bits)
- alignment is implementation-defined but MUST satisfy retained backend requirements for the instantiated width; the API does not promise universal 64-byte alignment for every width
- index precondition: `index < u64_count`

## Retained Bitwise Operations

```cpp
bitarray operator&(const bitarray& other) const noexcept;
bitarray& operator&=(const bitarray& other) noexcept;
bitarray operator|(const bitarray& other) const noexcept;
bitarray& operator|=(const bitarray& other) noexcept;
bitarray operator^(const bitarray& other) const noexcept;
bitarray& operator^=(const bitarray& other) noexcept;
bitarray operator~() const noexcept;
bitarray andnot(const bitarray& mask) const noexcept;
```

## Retained Shift Operations

```cpp
bitarray operator<<(int count) const noexcept;
bitarray& operator<<=(int count) noexcept;
void shift_left(int count) noexcept;

bitarray operator>>(int count) const noexcept;
bitarray& operator>>=(int count) noexcept;
void shift_right(int count) noexcept;
```

Boundary behavior:

| Condition | Behavior |
|-----------|----------|
| `count == 0` | no-op |
| `count >= Bits` | all bits cleared |
| `count < 0` | undefined behavior |

## Retained Comparison and State Operations

```cpp
bool operator==(const bitarray& other) const noexcept;
bool operator!=(const bitarray& other) const noexcept;
bool is_zero() const noexcept;
void clear() noexcept;
```

## Retained Counting and Single-Bit Operations

```cpp
uint64_t popcount() const noexcept;
int count_leading_zeros() const noexcept;
int count_trailing_zeros() const noexcept;

bool get_bit(size_t bit_index) const noexcept;
void set_bit(size_t bit_index, bool value = true) noexcept;
void flip_bit(size_t bit_index) noexcept;
void reverse() noexcept;
```

Precondition: `bit_index < Bits`

## Removed from the Retained Public Contract

The following surfaces are intentionally out of contract for v3.0.0 and must not be documented as retained public API unless a later OpenSpec change re-adds them:

- `namespace bitcal::ops`
- `is_bitarray`, `is_bitarray_v`, `bitarray_traits`
- `find_first_set()` / `find_last_set()`
- `set_range()` / `clear_range()` / `flip_range()`
- `all()` / `any()` / `none()` / `count()` / `size()` / `test()`
- the `bitarray<64>` specialization's explicit conversion to `uint64_t`
- any direct-include promise for headers other than `<bitcal/bitcal.hpp>`

These APIs may continue to exist temporarily in implementation during migration, but they are not covered by compatibility, documentation, or retained test requirements.

## Error Handling and Thread Safety

- Out-of-range `index` / `bit_index` usage is undefined behavior outside debug assertions.
- Negative shift counts are undefined behavior.
- Unsupported backend and toolchain combinations are outside the retained contract.
- Different instances may be used concurrently.
- Concurrent read/write on the same instance requires external synchronization.

## Contract Governance Requirements

### Requirement: Stable public include SHALL remain the umbrella header
BitCal SHALL document and verify `<bitcal/bitcal.hpp>` as the only stable public include seam for the retained API.

#### Scenario: Public-facing materials show how to include BitCal
- **WHEN** examples, documentation, or tests demonstrate public consumption
- **THEN** they MUST include `<bitcal/bitcal.hpp>`
- **AND** they MUST NOT describe any other include path as a stable public contract

### Requirement: Retained public API SHALL exclude removed helper surface
BitCal SHALL keep helper namespaces, traits, and undocumented convenience methods out of the retained 3.0.0 public contract unless they are explicitly re-added through OpenSpec.

#### Scenario: Maintainers review retained public API coverage
- **WHEN** a symbol or method is not listed in this specification as retained
- **THEN** it MUST be treated as out of contract for compatibility, testing, and public documentation purposes

### Requirement: Public API changes SHALL be explicitly versioned and documented
BitCal SHALL record any breaking public API or behavioral change in its API specification, migration-facing documentation, and canonical versioning source.

#### Scenario: A public API contract changes
- **WHEN** a method signature, include seam, type alias, semantic behavior, or supported contract changes in a breaking way
- **THEN** the API specification MUST describe the new contract
- **AND** the project documentation MUST identify the migration implication
- **AND** the version target MUST reflect the breaking change
