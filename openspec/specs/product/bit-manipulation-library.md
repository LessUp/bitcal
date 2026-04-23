# Product Spec: BitCal - SIMD-Accelerated Bit Manipulation Library

## Overview

BitCal is a modern, high-performance C++17 header-only library for bit manipulation operations with automatic SIMD acceleration.

## Version

**Current:** v2.1.0

## Target Users

- C++ developers requiring high-performance bit manipulation
- Systems programmers working on compression, cryptography, or databases
- Developers targeting x86-64 and ARM architectures

## Core Features

### 1. SIMD Acceleration
- Automatic SSE2/AVX2 (x86) or NEON (ARM) selection
- Up to 6× performance improvement over scalar implementations
- Compile-time dispatch via `if constexpr` for zero runtime overhead

### 2. Available Bit Widths

| Type | Width | Words | Best For |
|------|-------|-------|----------|
| `bitcal::bit64` | 64-bit | 1 | Machine word |
| `bitcal::bit128` | 128-bit | 2 | SSE2/NEON native |
| `bitcal::bit256` | 256-bit | 4 | AVX2 native |
| `bitcal::bit512` | 512-bit | 8 | Large parallel ops |
| `bitcal::bit1024` | 1024-bit | 16 | Very large ops |

Custom widths supported (must be multiple of 64).

### 3. Operations

#### Bitwise Operations
- AND, OR, XOR, NOT
- ANDNOT (optimized `a & ~b`)

#### Shift Operations
- Left shift (`<<`, `<<=`)
- Right shift (`>>`, `>>=`)
- Support for cross-word carry propagation

#### Bit Counting
- Population count (popcount)
- Count leading zeros (CLZ)
- Count trailing zeros (CTZ)

#### Bit Manipulation
- Get/set individual bits
- Flip individual bits
- Bit reversal

#### State Detection
- `is_zero()` - check if all bits are zero
- `clear()` - set all bits to zero

### 4. Cross-Platform Support

| Platform | Architecture | Compilers | SIMD | Status |
|----------|--------------|-----------|------|--------|
| Linux | x86-64 | GCC 7+, Clang 6+ | SSE2/AVX2 | ✅ Verified |
| Linux | ARM64 | GCC (cross) | NEON | ✅ Verified |
| Linux | ARM32 | GCC (cross) | NEON | ✅ Verified |
| Windows | x86-64 | MSVC 2017+ | SSE2/AVX2 | ✅ Verified |
| macOS | x86-64 | Apple Clang | SSE2/AVX2 | ✅ Verified |
| macOS | ARM64 (Apple Silicon) | Apple Clang | NEON | ✅ Verified |

## Acceptance Criteria

### AC-1: Performance Targets
- AND/OR/XOR operations on 256-bit: ≥5× speedup vs scalar (AVX2)
- Shift operations on 256-bit: ≥3× speedup vs scalar (AVX2)
- Popcount on 512-bit: ≥2× speedup vs scalar

### AC-2: API Completeness
- All predefined types support all core operations
- Single bit operations (get/set/flip) work for all bit positions
- Reverse operation works correctly for all predefined widths

### AC-3: Compilation Requirements
- Must compile with C++17 or later
- Must be header-only (no compilation required for use)
- Zero external dependencies

### AC-4: Platform Compatibility
- Must pass all unit tests on all supported platforms
- Must gracefully fall back to scalar when SIMD not available
- Must handle edge cases (shift by 0, shift by width, negative shift is UB)

## Non-Functional Requirements

### Performance
- Zero runtime overhead from dispatch (compile-time selection only)
- All critical paths force-inlined
- Cache-line aligned memory (64 bytes)

### Code Quality
- 100% test coverage for all predefined types
- CI verification on all supported platforms
- No compiler warnings with `-Wall -Wextra`

### Documentation
- API reference for all public functions
- Architecture documentation explaining SIMD dispatch
- Migration guide for users from previous versions

## Future Enhancements (Not in Current Scope)

- AVX-512 native support (currently falls back to AVX2)
- ARM SVE/SVE2 support
- Dynamic runtime CPU feature detection
- Bit widths not multiple of 64
