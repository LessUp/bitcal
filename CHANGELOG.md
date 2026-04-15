# Changelog

All notable changes to BitCal will be documented in this file.

## [2.1.0] - 2026-02-27

### Added
- **ANDNOT operation** — `bitarray::andnot(mask)` using native SIMD instructions
  - SSE: `_mm_andnot_si128`
  - AVX: `_mm256_andnot_si256`
  - NEON: `vbicq_u64`
- **Unified binary operation dispatch** — `dispatch_binop<Op>` template reduces code duplication
- **bit1024 unit tests** — Full test coverage for 1024-bit operations
- **Benchmarks directory** — Performance testing suite

### Performance
- `operator~` uses SIMD NOT instructions for 128/256/512-bit
- `is_zero()` uses `_mm_movemask_epi8` / `_mm256_testz` single-instruction detection
- `clear()` uses `std::memset` for better optimization
- `reverse()` in-place operation, eliminates temporary array

### Fixed
- MSVC SSE2 macro detection — added `_M_X64` check + `<intrin.h>`
- NEON `vmvnq_u64` — changed to `veorq_u64` XOR with all-ones
- NEON `vshlq_n_u64` — changed to `vshlq_u64` for runtime variable shift
- SSE 256-bit shift carry propagation bug
- AVX 256/512-bit shift cross-lane carry bug
- Include order bug — intrinsics must be outside namespace

### CI/CD
- Added ARM32 cross-compile job
- CMake auto-detection of SIMD compiler flags

## [2.0.0] - 2026-01-08

### Changed — Complete Rewrite

This is a **complete rewrite** from OOP inheritance design to modern C++17 template design.

### Architecture

**Old Design (v1.x)**:
```cpp
// Inheritance + virtual functions
class BitCalBase { virtual uint64_t bitAnd64(...) = 0; };
BitCal::getInstance().bitAnd64(a, b);  // Runtime dispatch
```

**New Design (v2.0)**:
```cpp
// Templates + compile-time dispatch
template<size_t Bits, simd_backend Backend>
class bitarray { ... };
bitcal::bit256 result = a & b;  // Zero-overhead abstraction
```

### Added
- **Header-only design** — Zero compilation dependencies
- **Compile-time SIMD selection** — Automatic backend selection via `if constexpr`
- **Full ARM NEON support** — 128/256/512-bit operations
- **Operator overloading** — `&`, `|`, `^`, `~`, `<<`, `>>`
- **Bit counting** — `popcount()`, `count_leading_zeros()`, `count_trailing_zeros()`
- **Bit manipulation** — `get_bit()`, `set_bit()`, `flip_bit()`, `is_zero()`, `clear()`, `reverse()`
- **Type aliases** — `bit64`, `bit128`, `bit256`, `bit512`, `bit1024`

### Performance
| Platform | Operation | Speedup |
|----------|-----------|---------|
| x86 AVX2 | AND-256 | **5.9×** |
| x86 AVX2 | XOR-512 | **5.8×** |
| x86 AVX2 | Shift-256 | **3.6×** |
| ARM NEON | AND-128 | **2.6×** |

### Removed
- Virtual function overhead
- Singleton pattern
- Runtime type checking
- External dependencies (spdlog)

### Platform Support
- ✅ Linux (x86-64, ARM64, ARM32)
- ✅ Windows (x86-64)
- ✅ macOS (x86-64, ARM64)

## [1.x] - Legacy

The original implementation based on inheritance and virtual functions.

**Status**: No longer maintained. Users should migrate to v2.x.

See [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) for migration instructions.
