# API Specification: BitCal vNext Public Interface

## Contract Target

Planned breaking contract for **v4.0.0**.

This specification defines the target public API for BitCal vNext. During the transition, implementation and tests may still carry older 3.0-era surfaces, but only the API described here is the target compatibility contract for the redesign.

## Namespace

All retained vNext public APIs live in the `bitcal` namespace.

## Stable Include Contract

```cpp
#include <bitcal/bitcal.hpp>
```

`<bitcal/bitcal.hpp>` is the only stable public include seam.

Implementation notes:
- Public definitions may physically live in `bit_block.hpp`, `bit_view.hpp`, and `algorithms.hpp`.
- Those headers do not become separate stable include promises.
- Files under `include/bitcal/detail/` are implementation details, not public seams.

## Core Public Types

### Owning block type

```cpp
namespace bitcal {
    template <std::size_t Bits>
    class bit_block;
}
```

**Template Parameters:**

| Parameter | Type | Constraints | Description |
|-----------|------|-------------|-------------|
| `Bits` | `std::size_t` | `Bits >= 64` and multiple of 64 | Total number of bits owned by the block |

### View types

```cpp
namespace bitcal {
    class bit_view;
    class const_bit_view;
}
```

`bit_view` and `const_bit_view` describe non-owning views over contiguous `uint64_t` word storage.

### Backend enumeration

```cpp
namespace bitcal {
    enum class backend_kind {
        scalar,
        sse2,
        avx2,
        avx512
    };
}
```

The vNext primary backend contract is x86-64-first; ARM-specific backend names are not part of the primary public contract.

## bit_block Construction and Metadata

```cpp
bit_block() noexcept;

static constexpr std::size_t bits = Bits;
static constexpr std::size_t word_count = Bits / 64;
static constexpr std::size_t storage_alignment = /* width-appropriate alignment */;
```

### Public view access

```cpp
bit_view view() noexcept;
const_bit_view view() const noexcept;
```

## View Contract

```cpp
std::uint64_t* data() noexcept;               // bit_view only
const std::uint64_t* data() const noexcept;   // const_bit_view
std::size_t word_count() const noexcept;
std::uint64_t word(std::size_t index) const noexcept;
```

Contract notes:
- storage is contiguous `uint64_t` word storage
- `bit_block<Bits>::storage_alignment` tracks width-appropriate alignment for the fixed-width block
- word order is little-endian (`word 0` holds the least significant bits)
- index precondition: `index < word_count()`
- a mutable view may expose write access through `data()`

## Public Algorithm Surface

The vNext public contract uses free algorithms instead of making the older `bitarray` member API the architectural center.

### Bitwise algorithms

```cpp
template <std::size_t Bits>
bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_xor(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_andnot(const const_bit_view lhs, const const_bit_view rhs) noexcept;
```

### Query and counting algorithms

```cpp
bool is_zero(const const_bit_view value) noexcept;
std::uint64_t popcount(const const_bit_view value) noexcept;
bool equals(const const_bit_view lhs, const const_bit_view rhs) noexcept;
```

`equals()` returns `false` when the two views do not expose the same `word_count()`.

### Shift and transform algorithms

```cpp
template <std::size_t Bits>
bit_block<Bits> shift_left(const const_bit_view value, int count) noexcept;

template <std::size_t Bits>
bit_block<Bits> shift_right(const const_bit_view value, int count) noexcept;
```

Boundary behavior:

| Condition | Behavior |
|-----------|----------|
| `count == 0` | no-op result |
| `count >= Bits` | all bits cleared |
| `count < 0` | undefined behavior |

## Removed from the vNext Contract

The following are intentionally out of contract for vNext and must not be documented as retained public API unless a later OpenSpec change re-adds them:

- the older `bitarray`-centered public API as the primary architecture model
- code-level compatibility shims for `bitarray`
- `namespace bitcal::ops`
- public type traits such as `is_bitarray` and `bitarray_traits`
- any direct-include promise for headers other than `<bitcal/bitcal.hpp>`

## Error Handling and Thread Safety

- Out-of-range `index` usage is undefined behavior outside debug assertions.
- Negative shift counts are undefined behavior.
- Unsupported backend/toolchain combinations are outside the public contract.
- Different instances may be used concurrently.
- Concurrent read/write on the same storage requires external synchronization.

## Contract Governance Requirements

### Requirement: Stable public include SHALL remain the umbrella header
BitCal SHALL document and verify `<bitcal/bitcal.hpp>` as the only stable public include seam for the vNext API.

#### Scenario: Public-facing materials show how to include BitCal
- **WHEN** examples, documentation, or tests demonstrate public consumption
- **THEN** they MUST include `<bitcal/bitcal.hpp>`
- **AND** they MUST NOT describe any other include path as a stable public contract

### Requirement: vNext public API SHALL center on blocks, views, and algorithms
BitCal SHALL define its next-generation public API around `bit_block`, `bit_view` / `const_bit_view`, and free algorithms.

#### Scenario: Maintainers review vNext public API coverage
- **WHEN** a public symbol is evaluated for the redesign
- **THEN** `bit_block`, view types, and free algorithms MUST be treated as the core public model
- **AND** the older `bitarray` model MUST NOT be treated as the architectural center of vNext

### Requirement: Breaking API changes SHALL be explicitly versioned and documented
BitCal SHALL record any breaking public API or behavior change in its API specification, migration-facing documentation, and canonical versioning source.

#### Scenario: A public API contract changes
- **WHEN** a type, function family, include seam, semantic behavior, or supported contract changes in a breaking way
- **THEN** the API specification MUST describe the new contract
- **AND** the project documentation MUST identify the migration implication
- **AND** the version target MUST reflect the breaking change
