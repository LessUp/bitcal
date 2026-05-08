# Testing Specification: BitCal

## Contract Target

Testing requirements for the planned **v3.0.0** public-surface contraction.

This specification defines the retained validation scope. Legacy tests may continue to exist during transition, but only the behaviors and seams described here remain part of the public contract test matrix.

## Test Strategy

BitCal SHALL validate the retained public contract with three layers:
1. public API contract tests through `<bitcal/bitcal.hpp>`
2. implementation regression tests for retained behavior
3. platform/build verification aligned with the documented support matrix

## Public Contract Coverage

### 1. Stable include seam

- [ ] Public contract tests compile through `<bitcal/bitcal.hpp>`
- [ ] No retained contract test requires a direct include of internal or non-stable headers

### 2. Retained type construction and constants

- [ ] Default constructor initializes all bits to zero
- [ ] Value constructor initializes the low 64 bits and clears the rest
- [ ] Copy and move construction/assignment preserve retained behavior
- [ ] `bits`, `u64_count`, and `backend` remain correct for retained aliases and custom widths

### 3. Retained data access

- [ ] `data()` exposes contiguous word storage for read-only access
- [ ] `word()` / `set_word()` operate on valid word indices
- [ ] `operator[]` provides read-only word access

### 4. Retained bitwise and shift operations

- [ ] `&`, `|`, `^`, `~`, and `andnot()` match retained semantics
- [ ] compound assignments (`&=`, `|=`, `^=`) preserve retained semantics
- [ ] left and right shifts handle zero, word-crossing, and width-clearing cases
- [ ] negative shift counts remain outside contract and are not asserted as supported behavior

### 5. Retained comparison, state, and counting operations

- [ ] `==` / `!=` validate equality semantics
- [ ] `is_zero()` and `clear()` validate retained zero-state behavior
- [ ] `popcount()`, `count_leading_zeros()`, and `count_trailing_zeros()` validate known values and boundary cases

### 6. Retained single-bit operations

- [ ] `get_bit()` reads valid positions
- [ ] `set_bit()` sets and clears valid positions
- [ ] `flip_bit()` toggles valid positions
- [ ] `reverse()` validates representative patterns and involution behavior

### 7. Retained predefined widths

The retained contract suite SHALL cover at least:
- [ ] `bitcal::bit64`
- [ ] `bitcal::bit128`
- [ ] `bitcal::bit256`
- [ ] `bitcal::bit512`
- [ ] `bitcal::bit1024`
- [ ] one custom width that satisfies the retained width constraint

## Removed Surface Handling

The following are not part of the retained public contract suite for v3.0.0:
- `bitcal::ops`
- `is_bitarray`, `is_bitarray_v`, `bitarray_traits`
- undocumented convenience methods not retained in the API spec
- any test expectation that another include path is a stable public seam

These may exist temporarily as migration scaffolding, but they MUST NOT block or define public contract completion.

## Backend and Platform Validation

### Retained backend names

- [ ] `simd_backend::scalar`
- [ ] `simd_backend::sse2`
- [ ] `simd_backend::avx2`
- [ ] `simd_backend::avx512`
- [ ] `simd_backend::neon`

`simd_backend::avx` is not part of the retained support matrix.

### Validation policy

- retained public contract tests verify observable behavior, not internal header topology
- backend-specific validation may use implementation-aware builds, but support claims must still align with retained documented backends
- public contract tests should prefer behavior equivalence across supported backend selections

### Platform support matrix

| Platform | Compiler | Architecture | SIMD path | Validation path |
|----------|----------|--------------|-----------|-----------------|
| Linux | GCC / Clang | x86-64 | SSE2 / AVX2 / AVX-512 when enabled | Native retained tests |
| Linux | GCC / Clang | ARM64 | NEON | Native or documented cross-validation |
| Linux | GCC (cross) | ARM32 | NEON | Cross-compile with documented runtime validation path |
| Windows | MSVC | x86-64 | SSE2 / AVX2 / AVX-512 when enabled | Native retained tests |
| macOS | Apple Clang | x86-64 | SSE2 / AVX2 when enabled | Native retained tests |
| macOS | Apple Clang | ARM64 | NEON | Native retained tests |

## Verification Commands

BitCal's retained local verification path remains:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
```

## Validation Requirements

### Requirement: Testing scope SHALL match the retained public contract
BitCal SHALL focus automated testing on the public behaviors, include seam, platforms, and invariants that remain part of the maintained contract after the 3.0.0 contraction.

#### Scenario: A behavior is retained as supported
- **WHEN** the project keeps a public operation, include seam, platform claim, or backend guarantee
- **THEN** the retained test strategy MUST include validation for that contract

### Requirement: Removed public helpers SHALL leave the retained contract suite
BitCal SHALL stop treating removed helper namespaces, traits, and undocumented convenience methods as part of retained public contract completion.

#### Scenario: Maintainers prune tests for 3.0.0
- **WHEN** a test only exists to validate a removed helper API or non-stable include path
- **THEN** it MUST be deleted, demoted to non-contract regression coverage, or rewritten against the retained public seam

### Requirement: Validation workflows SHALL match the documented support policy
BitCal SHALL ensure that CI validation, local verification instructions, and testing specs describe the same maintained support matrix.

#### Scenario: Maintainers review support claims
- **WHEN** support claims are listed in testing specs or documentation
- **THEN** workflow configuration and verification instructions MUST align with those claims and omit unsupported combinations
