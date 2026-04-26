# Testing Specification: BitCal

## Test Strategy

BitCal uses a comprehensive testing strategy covering:
1. Unit tests for all operations
2. SIMD backend verification
3. Edge case coverage
4. Cross-platform validation

## Unit Test Coverage

### 1. Type Construction

- [ ] Default constructor (all zeros)
- [ ] Value constructor (lower 64 bits set)
- [ ] Copy constructor and assignment
- [ ] Move constructor and assignment

### 2. Bitwise Operations

#### AND
- [ ] Basic AND operation
- [ ] Compound assignment (`&=`)
- [ ] Identity: `a & a == a`
- [ ] Commutativity: `a & b == b & a`
- [ ] Zero property: `a & 0 == 0`

#### OR
- [ ] Basic OR operation
- [ ] Compound assignment (`|=`)
- [ ] Identity: `a | a == a`
- [ ] Commutativity: `a | b == b | a`
- [ ] Zero property: `a | 0 == a`

#### XOR
- [ ] Basic XOR operation
- [ ] Compound assignment (`^=`)
- [ ] Self-XOR: `a ^ a == 0`
- [ ] Commutativity: `a ^ b == b ^ a`
- [ ] Zero property: `a ^ 0 == a`

#### NOT
- [ ] Basic NOT operation
- [ ] Double negation: `~~a == a`
- [ ] De Morgan's laws: `~(a & b) == ~a | ~b`

#### ANDNOT
- [ ] Basic ANDNOT operation
- [ ] Equivalence: `a.andnot(b) == (a & ~b)`
- [ ] Self-ANDNOT: `a.andnot(a) == 0`

### 3. Shift Operations

#### Left Shift
- [ ] Basic left shift
- [ ] Compound assignment (`<<=`)
- [ ] Shift by 0 (no change)
- [ ] Shift by width (all zeros)
- [ ] Cross-word carry propagation
- [ ] Multiple of 64 shift (word movement)

#### Right Shift
- [ ] Basic right shift
- [ ] Compound assignment (`>>=`)
- [ ] Shift by 0 (no change)
- [ ] Shift by width (all zeros)
- [ ] Cross-word carry propagation

### 4. Comparison

- [ ] Equality (`==`)
- [ ] Inequality (`!=`)
- [ ] Reflexivity: `a == a`
- [ ] Symmetry: `a == b` iff `b == a`

### 5. Bit Counting

#### popcount
- [ ] All zeros → 0
- [ ] All ones → Bits
- [ ] Single bit set → 1
- [ ] Alternating pattern → Bits/2
- [ ] Known value verification

#### count_leading_zeros
- [ ] All zeros → Bits
- [ ] MSB set → 0
- [ ] Single LSB → Bits-1

#### count_trailing_zeros
- [ ] All zeros → Bits
- [ ] LSB set → 0
- [ ] Single MSB → Bits-1

### 6. Single Bit Operations

#### get_bit
- [ ] Read bit at valid positions (0 to Bits-1)
- [ ] Read after set_bit
- [ ] Read after flip_bit

#### set_bit
- [ ] Set to 1
- [ ] Clear to 0
- [ ] Set then get verification

#### flip_bit
- [ ] Flip 0 → 1
- [ ] Flip 1 → 0
- [ ] Double flip restores original

### 7. Bit Reversal

- [ ] Single bit reversal
- [ ] All zeros reversal
- [ ] All ones reversal
- [ ] Symmetric pattern
- [ ] Double reversal restores original

### 8. State Detection

#### is_zero
- [ ] Default constructed → true
- [ ] Any bit set → false
- [ ] After clear() → true

#### clear
- [ ] Clear non-zero value
- [ ] Clear already-zero value
- [ ] Verify is_zero() after clear

### 9. Data Access

#### operator[]
- [ ] Read/write at all word indices
- [ ] Bounds verification (debug mode)

#### data()
- [ ] Pointer validity
- [ ] Contiguous storage
- [ ] 64-byte alignment

## Test Coverage by Type

All above tests must pass for:
- [ ] `bitcal::bit64`
- [ ] `bitcal::bit128`
- [ ] `bitcal::bit256`
- [ ] `bitcal::bit512`
- [ ] `bitcal::bit1024`
- [ ] Custom width (e.g., `bitarray<2048>`)

## SIMD Backend Verification

### Scalar Backend
- [ ] All operations work with `simd_backend::scalar`
- [ ] Platform-independent results

### SSE2 Backend (x86)
- [ ] All operations work with `simd_backend::sse2`
- [ ] Results match scalar implementation

### AVX2 Backend (x86)
- [ ] All operations work with `simd_backend::avx2`
- [ ] Results match scalar implementation
- [ ] Cross-lane shift correctness

### AVX-512 Backend (x86, partial)
- [ ] Supported AVX-512 operations work with `simd_backend::avx512`
- [ ] Results match retained API semantics
- [ ] Documented AVX-512 fallbacks remain explicit where full native coverage is not provided

### NEON Backend (ARM)
- [ ] All operations work with `simd_backend::neon`
- [ ] Results match scalar implementation
- [ ] Variable shift correctness

## Edge Cases

### Boundary Conditions
- [ ] Minimum width (64 bits)
- [ ] Maximum practical width (tested up to 65536)
- [ ] Shift by exact width
- [ ] Shift by multiple of width
- [ ] Operations on all-zeros data
- [ ] Operations on all-ones data

### Cross-Word Operations
- [ ] Carry propagation across word boundaries
- [ ] Multi-word shifts
- [ ] Multi-word reversals

### Type Width Constraints
- [ ] Non-multiple-of-64 widths rejected at compile time
- [ ] Custom widths work correctly

## Performance Tests

### Benchmark Suite

Using Google Benchmark framework:

- [ ] AND/OR/XOR latency by width
- [ ] NOT latency by width
- [ ] Shift latency by width and amount
- [ ] popcount throughput
- [ ] CLZ/CTZ latency
- [ ] ANDNOT latency
- [ ] is_zero latency

### Performance Verification

Verify speedup targets on supported platforms:

- [ ] 256-bit AND ≥5× scalar (AVX2)
- [ ] 256-bit shift ≥3× scalar (AVX2)
- [ ] 512-bit popcount ≥2× scalar

## Platform Test Matrix

| Platform | Compiler | Architecture | SIMD | Validation Path |
|----------|----------|--------------|------|-----------------|
| Linux | GCC 11 | x86-64 | AVX2 | ✅ Native tests required |
| Linux | Clang 14 | x86-64 | AVX2 | ✅ Native tests required |
| Linux | GCC (cross) | ARM64 | NEON | ✅ Cross-compile required |
| Linux | GCC (cross) | ARM32 | NEON | ✅ Cross-compile required |
| Windows | MSVC 2022 | x86-64 | AVX2 | ✅ Native tests required |
| macOS | Apple Clang | x86-64 | AVX2 | ✅ Native tests required |
| macOS | Apple Clang | ARM64 | NEON | ✅ Native tests required |

## Test Infrastructure

### Build System
- CMake 3.16+ for test compilation
- Tests built with `-DCMAKE_BUILD_TYPE=Release`

### Execution
- Unit tests: `ctest --test-dir build --output-on-failure -C Release`
- Benchmarks: `./build/benchmarks/bitcal_benchmark` (when `BITCAL_BUILD_BENCHMARKS=ON`)

### CI Integration
- GitHub Actions workflow: `.github/workflows/ci.yml`
- CI runs on scoped `push` and `pull_request` events for code, build, dependency, and workflow changes
- Native test execution is required on retained first-class CI platforms; ARM targets are cross-compiled in CI and rely on documented NEON validation paths for runtime assurance

## Retained Contract Validation Requirements

### Requirement: Testing scope SHALL match the retained public contract
BitCal SHALL focus automated testing on the public behaviors, platforms, and invariants that remain part of the final maintained contract after cleanup.

#### Scenario: A behavior is retained as supported
- **WHEN** the project keeps a public operation, platform claim, or backend guarantee
- **THEN** the automated test strategy MUST include validation for that retained contract

### Requirement: Validation workflows SHALL match the documented support policy
BitCal SHALL ensure that CI validation, manual verification instructions, and testing specs describe the same maintained support matrix.

#### Scenario: Maintainers review the support matrix
- **WHEN** support claims are listed in testing specs or documentation
- **THEN** the workflow configuration and verification instructions MUST align with those claims and omit unsupported combinations

## Acceptance Criteria

A feature is considered complete when:
1. ✅ Retained automated validation passes and any documented manual verification steps are completed
2. ✅ Performance benchmarks meet or exceed targets
3. ✅ No regressions in existing tests
4. ✅ Updated tests cover the retained public contract for the changed behavior
5. ✅ Edge cases documented and tested
