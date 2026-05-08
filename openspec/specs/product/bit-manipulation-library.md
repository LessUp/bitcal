# Product Spec: BitCal - SIMD-Accelerated Bit Manipulation Library

## Product Position

BitCal is a stable, archive-ready, C++17 header-only bit-manipulation library.

The planned **v3.0.0** release contracts the public surface so the project retains a smaller, better-defined compatibility boundary centered on `bitarray` and a single stable public include seam.

## Stable Consumption Model

```cpp
#include <bitcal/bitcal.hpp>
```

BitCal SHALL present `<bitcal/bitcal.hpp>` as the only stable public include path.

The retained product contract is centered on:
- `bitarray<Bits, Backend>`
- stable aliases (`bit64`, `bit128`, `bit256`, `bit512`, `bit1024`)
- retained bitwise, shift, comparison, zero-state, counting, and single-bit operations
- retained backend selection through `simd_backend` and `get_default_backend()`

## Product Boundaries

### Retained public capabilities
- Fixed-width bit arrays with widths that are multiples of 64
- Compile-time backend selection across `scalar`, `sse2`, `avx2`, `avx512`, and `neon`
- Header-only consumption through the umbrella include
- Cross-platform support claims that have a retained validation path

### Removed from the retained public product contract
- `bitcal::ops` as a public helper namespace
- public type traits such as `is_bitarray` and `bitarray_traits`
- undocumented convenience methods not explicitly preserved in the API spec
- any promise that internal headers are stable include entry points

## Core Features

### 1. SIMD Acceleration
- Compile-time backend selection with scalar fallback
- Supported retained backend names: SSE2, AVX2, AVX-512, and NEON as available
- No retained product claim for a separate transitional `AVX` backend

### 2. Retained widths

| Type | Width | Words | Typical use |
|------|-------|-------|-------------|
| `bitcal::bit64` | 64-bit | 1 | Machine-word operations |
| `bitcal::bit128` | 128-bit | 2 | SSE2 / NEON-sized workloads |
| `bitcal::bit256` | 256-bit | 4 | AVX2-sized workloads |
| `bitcal::bit512` | 512-bit | 8 | Large parallel bit operations |
| `bitcal::bit1024` | 1024-bit | 16 | Larger fixed-size bitsets |

Custom widths remain supported when they satisfy the retained width constraint.

### 3. Retained operation families
- Bitwise operations: AND, OR, XOR, NOT, ANDNOT
- Shift operations: left/right shifts with retained width-clearing behavior
- Counting operations: popcount, CLZ, CTZ
- Bit manipulation: get/set/flip single bits, reverse
- State operations: equality, inequality, `is_zero()`, `clear()`
- Data access: `data()`, `word()`, `set_word()`, `operator[]`

## Support and Validation Posture

### Platform support

| Platform | Architecture | SIMD path | Status |
|----------|--------------|-----------|--------|
| Linux | x86-64 | SSE2 / AVX2 / AVX-512 when enabled | Retained validation path |
| Linux | ARM64 | NEON | Retained validation path |
| Linux | ARM32 | NEON | Retained validation path |
| Windows | x86-64 | SSE2 / AVX2 / AVX-512 when enabled | Retained validation path |
| macOS | x86-64 | SSE2 / AVX2 when enabled | Retained validation path |
| macOS | ARM64 | NEON | Retained validation path |

### Performance posture
- BitCal continues to optimize for zero runtime dispatch overhead.
- Published performance claims must only cover scenarios that still have a retained benchmark or validation path.
- Contract fidelity is preferred over expanding the feature surface.

## Release and Migration Expectations

- The public-surface contraction is a **breaking release planned as 3.0.0**.
- Migration-facing documentation must explain removed helper APIs and include-seam expectations.
- README, Pages, OpenSpec, and version anchors must describe the same retained product boundary.

## Product Requirements

### Requirement: Product positioning SHALL prioritize archive-ready stability
BitCal SHALL define itself as a stable, archive-ready SIMD bit-manipulation library whose documentation, support matrix, and engineering process reflect maintainable long-term stewardship.

#### Scenario: Project materials describe BitCal
- **WHEN** maintainers update README, Pages, or product-facing specs
- **THEN** those materials MUST describe the project as a stable library with explicit scope, support expectations, and maintenance boundaries

### Requirement: Public surface contraction SHALL ship as a major release
BitCal SHALL publish the retained public-surface contraction as version 3.0.0 and SHALL describe it as a breaking release.

#### Scenario: Release planning is updated
- **WHEN** specs, docs, tests, and version semantics are aligned to the contracted API
- **THEN** the release target MUST be a major version increment
- **AND** migration-facing materials MUST describe removed or re-scoped public APIs

### Requirement: Product-facing materials SHALL describe the contracted public boundary
BitCal SHALL present itself as a header-only library with a single stable public include seam and a `bitarray`-centered retained API.

#### Scenario: Product materials describe how BitCal is consumed
- **WHEN** README, Pages, or product specs summarize the library surface
- **THEN** they MUST describe `<bitcal/bitcal.hpp>` as the stable include seam
- **AND** they MUST NOT present removed helper namespaces, traits, or undocumented convenience methods as retained public features

### Requirement: Platform and performance claims SHALL reflect maintained reality
BitCal SHALL only publish platform, compiler, and performance claims that are backed by retained workflows, tests, or documented validation procedures.

#### Scenario: A support claim appears in documentation
- **WHEN** documentation states that a platform, compiler, or performance target is supported
- **THEN** the repository MUST retain a corresponding validation path or clearly mark the claim as historical/non-guaranteed information
