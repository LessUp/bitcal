# Product Spec: BitCal vNext - C++23 Bit Manipulation Library

## Product Position

BitCal vNext is a C++23, x86-64-first, header-only bit-manipulation library.

The planned **v4.0.0** release is a deliberate breaking redesign: it replaces the older `bitarray`-centered public model with a new split between owning fixed-width blocks, non-owning views, and free algorithms, while keeping a single stable public include seam.

## Stable Consumption Model

```cpp
#include <bitcal/bitcal.hpp>
```

BitCal SHALL present `<bitcal/bitcal.hpp>` as the only stable public include path for vNext.

## Core Public Model

The vNext product contract is centered on:
- `bit_block<Bits>` as the owning fixed-width storage type
- `bit_view` / `const_bit_view` as non-owning word views
- free algorithms operating on public blocks and views
- explicit backend identification through `backend_kind`

The older `bitarray` retained contract is not the target product model for vNext.

## Product Boundaries

### Retained vNext public capabilities
- Fixed-width owning blocks with widths that are multiples of 64
- Non-owning views over contiguous `uint64_t` word storage
- Free algorithms for bitwise operations, state queries, counting, and shifts
- Header-only consumption through the umbrella include
- x86-64-first optimization and validation posture

### Outside the vNext public product contract
- code-level compatibility shims for the older `bitarray` public API
- a promise that internal headers are stable include entry points
- platform or performance claims without a retained x86-64 validation path
- treating the 3.0.0 retained public-surface contraction as the final architecture target

## Core Features

### 1. Owning fixed-width storage
- `bit_block<Bits>` owns contiguous `uint64_t` words
- Width remains compile-time fixed
- Storage and view semantics are designed for algorithm composition and backend specialization

### 2. Non-owning views
- `bit_view` provides mutable access to contiguous word storage
- `const_bit_view` provides read-only access to contiguous word storage
- Public algorithms operate on views so callers can compose storage and algorithm layers explicitly

### 3. Public algorithm surface
- Bitwise algorithms: AND, OR, XOR, NOT, ANDNOT
- Query/state algorithms: equality, zero checks
- Counting algorithms: popcount and scan/count-style queries
- Shift and transformation algorithms operating on blocks/views

### 4. Backend posture
- Primary optimized backend targets are x86-64 scalar, SSE2, AVX2, and AVX-512 paths
- ARM and macOS may continue to build or run as secondary targets, but they do not define the primary optimization contract

## Support and Validation Posture

### Platform support

| Platform | Compiler | Architecture | Posture |
|----------|----------|--------------|---------|
| Linux | GCC / Clang | x86-64 | Primary validation and optimization target |
| Windows | MSVC | x86-64 | Primary validation target |
| Linux | GCC / Clang | ARM64 | Secondary target; weaker guarantees than x86-64 |
| macOS | Apple Clang | ARM64 | Secondary target; weaker guarantees than x86-64 |

Rows not listed above are outside the intended vNext support matrix unless a later change explicitly adds and retains a validation path.

### Performance posture
- BitCal vNext optimizes first for x86-64 observable performance and correctness.
- Published performance claims must cite a retained benchmark or validation path.
- Secondary platforms must not inherit stronger performance claims than the retained evidence supports.

## Release and Migration Expectations

- The public redesign is a **breaking release planned as v4.0.0**.
- Migration guidance must explain the replacement of the older `bitarray` public model.
- vNext does **not** promise a compatibility shim layer for old public APIs.
- README, Pages, OpenSpec, and version anchors must describe the same vNext product boundary.

## Product Requirements

### Requirement: Product positioning SHALL describe a C++23 x86-64-first redesign
BitCal SHALL define vNext as a C++23, x86-64-first redesign rather than as an extension of the older stable-retained C++17 posture.

#### Scenario: Project materials describe BitCal vNext
- **WHEN** maintainers update README, Pages, or product-facing specs for the next major release
- **THEN** those materials MUST describe BitCal as a C++23 redesign with x86-64 as the primary target
- **AND** they MUST NOT present the older retained `bitarray` contract as the architectural center of vNext

### Requirement: Public model SHALL center on blocks, views, and free algorithms
BitCal SHALL present owning `bit_block`, non-owning view types, and public algorithms as the core vNext product model.

#### Scenario: Product materials describe how BitCal is used
- **WHEN** documentation or product specs summarize the vNext library surface
- **THEN** they MUST describe blocks, views, and public algorithms as the primary API model
- **AND** they MUST NOT describe code-level compatibility shims as part of the product contract

### Requirement: Platform and performance claims SHALL prioritize retained x86-64 evidence
BitCal SHALL only publish support or performance claims that are backed by retained validation procedures, with x86-64 as the primary evidence source.

#### Scenario: A support or performance claim appears in documentation
- **WHEN** documentation states that a platform, compiler, or performance target is supported
- **THEN** the repository MUST retain a corresponding validation path
- **AND** non-primary platforms MUST NOT receive stronger guarantees than the retained evidence supports

### Requirement: The redesign SHALL ship as a major breaking release
BitCal SHALL publish the new public model as version 4.0.0 or later major-version equivalent and SHALL describe it as a breaking release.

#### Scenario: Release planning is updated for vNext
- **WHEN** specs, docs, tests, and version semantics are aligned to the redesign
- **THEN** the release target MUST be a major version increment
- **AND** migration-facing materials MUST describe the replacement of the older public model
