# Testing Specification: BitCal vNext

## Contract Target

Testing requirements for the planned **v4.0.0** redesign.

This specification defines the target validation scope for BitCal vNext. During transition, older retained tests may continue to exist, but only the behaviors and seams described here define completion for the redesign.

## Test Strategy

BitCal SHALL validate the vNext public contract with three layers:
1. public contract tests through `<bitcal/bitcal.hpp>`
2. implementation regression tests for the new block/view/algorithm model
3. platform/build verification aligned with the x86-64-first support matrix

## Public Contract Coverage

### 1. Stable include seam

- [ ] Public contract tests compile through `<bitcal/bitcal.hpp>`
- [ ] No public contract test requires a direct include of internal headers

### 2. Owning block construction and metadata

- [ ] Default `bit_block<Bits>` construction initializes all words to zero
- [ ] `bits` and `word_count` remain correct for representative widths
- [ ] `view()` exposes the corresponding public view types

### 3. View semantics

- [ ] `bit_view` and `const_bit_view` expose contiguous word storage
- [ ] `word_count()` matches the owning block width
- [ ] `word(index)` matches observable stored word values

### 4. Public algorithm smoke coverage

- [ ] `bit_and`, `bit_or`, `bit_xor`, and `bit_andnot` validate representative word patterns
- [ ] `is_zero`, `equals`, and `popcount` validate representative state and count behavior
- [ ] `equals` returns `false` for mismatched view lengths instead of matching a shared prefix
- [ ] shifts validate zero, word-crossing, and width-clearing cases

### 5. Representative widths

The vNext contract suite SHALL cover at least:
- [ ] one 128-bit block
- [ ] one 256-bit block
- [ ] one 512-bit block
- [ ] one custom width satisfying the width constraint

## Removed Surface Handling

The following are not part of vNext public contract completion:
- the older `bitarray` retained suite as the definition of redesign correctness
- helper namespaces and traits removed from public contract
- any assumption that internal headers are stable include seams

These may temporarily exist during migration, but they MUST NOT define vNext completion.

## Backend and Platform Validation

### Retained backend names

- [ ] `backend_kind::scalar`
- [ ] `backend_kind::sse2`
- [ ] `backend_kind::avx2`
- [ ] `backend_kind::avx512`

### Validation policy

- public contract tests verify observable behavior, not internal header topology
- backend-specific validation may use implementation-aware tests, but support claims must align with the documented primary matrix
- x86-64 is the primary retained evidence path for performance and correctness claims

### Platform support matrix

| Platform | Compiler | Architecture | Validation path |
|----------|----------|--------------|-----------------|
| Linux | GCC / Clang | x86-64 | Primary native tests |
| Windows | MSVC | x86-64 | Primary native tests |
| Linux | GCC / Clang | ARM64 | Secondary build/test path as retained |
| macOS | Apple Clang | ARM64 | Secondary build/test path as retained |

Rows not listed above are outside the intended redesign matrix unless a later change adds and keeps a matching validation path.

## Verification Commands

BitCal's vNext local verification path remains a clean CMake build directory:

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
```

## Validation Requirements

### Requirement: Testing scope SHALL match the vNext public contract
BitCal SHALL focus automated testing on the public behaviors, include seam, platforms, and invariants that remain part of the vNext public contract.

#### Scenario: A behavior is part of the redesign contract
- **WHEN** the project keeps a public operation, include seam, platform claim, or backend guarantee in vNext
- **THEN** the test strategy MUST include validation for that contract

### Requirement: Old retained helpers SHALL not define redesign completion
BitCal SHALL stop treating the older retained helper surface or `bitarray`-centered tests as the definition of vNext completion.

#### Scenario: Maintainers review redesign test coverage
- **WHEN** a test only validates the older public model or a removed helper API
- **THEN** it MUST be deleted, demoted to migration-only coverage, or rewritten against the vNext public seam

### Requirement: Validation workflows SHALL match the x86-64-first support policy
BitCal SHALL ensure that CI validation, local verification instructions, and testing specs describe the same x86-64-first support matrix.

#### Scenario: Maintainers review support claims
- **WHEN** support claims are listed in testing specs or documentation
- **THEN** workflow configuration and verification instructions MUST align with those claims
- **AND** non-primary platforms MUST NOT be described with stronger guarantees than the retained workflows support
