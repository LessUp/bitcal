# Changelog

<p align="center">
  <strong>English</strong> | <a href="CHANGELOG.zh-CN.md">简体中文</a>
</p>

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

## [2.1.0] - 2026-04-16

### 🚀 Highlights

- **New:** ANDNOT operation with native SIMD instructions across all backends
- **Performance:** Up to 2.3× faster `is_zero()`, 1.7× faster `~`, 1.4× faster `reverse()`
- **Testing:** Full bit1024 test coverage
- **Infrastructure:** ARM32 CI support, CMake flag auto-detection

### ✨ Added

- **ANDNOT operation** — `bitarray::andnot(mask)` using native SIMD instructions
  - SSE: `_mm_andnot_si128`
  - AVX: `_mm256_andnot_si256`  
  - NEON: `vbicq_u64`
- **Unified binary operation dispatch** — `dispatch_binop<Op>` template reduces code duplication by ~40%
- **bit1024 unit tests** — Full test coverage for 1024-bit operations
- **Benchmarks directory** — Performance testing suite with Google Benchmark
- **Documentation improvements** — Professional bilingual (EN/ZH) documentation

### ⚡ Performance

| Optimization | v2.0 | v2.1 | Improvement |
|--------------|------|------|-------------|
| `operator~` | Scalar fallback | SIMD NOT | **1.7×** (256-bit) |
| `is_zero()` | Multi-instruction | Single instruction | **2.3×** (256-bit) |
| `clear()` | Loop-based | `std::memset` | **1.9×** (1024-bit) |
| `reverse()` | With temp array | In-place | **1.4×** (256-bit) |

### 🔧 Fixed

| Issue | Platform | Resolution |
|-------|----------|------------|
| MSVC SSE2 detection | Windows | Added `_M_X64` check + `<intrin.h>` |
| NEON NOT operation | ARM | `vmvnq_u64` → `veorq_u64` (compatibility) |
| NEON variable shift | ARM | `vshlq_n_u64` → `vshlq_u64` (runtime support) |
| SSE 256-bit shift carry | x86 | Fixed 4-qword carry chain |
| AVX 256/512-bit shift | x86 | Fixed cross-lane carry propagation |
| Include order | All | Intrinsics must be outside namespace scope |

### 🏗️ Infrastructure

- Added ARM32 cross-compile CI job
- CMake auto-detection of SIMD compiler flags
- Professional documentation rewrite (English & Chinese)

### 🔗 Links

- **Full Changelog**: [v2.0.0...v2.1.0](https://github.com/LessUp/bitcal/compare/v2.0.0...v2.1.0)
- **Download**: [v2.1.0 Release](https://github.com/LessUp/bitcal/releases/tag/v2.1.0)
- **Documentation**: [API Reference](docs/en/README.md)

---

## [2.0.0] - 2026-01-08

### 🔄 Complete Rewrite

This is a **complete architectural redesign** from OOP inheritance to modern C++17 template-based design.

### 🏗️ Architecture Changes

| Aspect | v1.x | v2.0 |
|--------|------|------|
| Design Pattern | Inheritance + Virtual | Templates + `if constexpr` |
| Dispatch | Runtime | Compile-time |
| Dependencies | External (spdlog) | None (header-only) |
| API Style | Singleton pattern | Value types + operators |

### ✨ Added

- **Header-only design** — Zero compilation dependencies
- **Compile-time SIMD selection** — Automatic backend selection via `if constexpr`
- **Full ARM NEON support** — 128/256/512-bit operations on ARM
- **Operator overloading** — `&`, `|`, `^`, `~`, `<<`, `>>`
- **Bit counting** — `popcount()`, `count_leading_zeros()`, `count_trailing_zeros()`
- **Bit manipulation** — `get_bit()`, `set_bit()`, `flip_bit()`, `is_zero()`, `clear()`, `reverse()`
- **Type aliases** — `bit64`, `bit128`, `bit256`, `bit512`, `bit1024`
- **Low-level API** — `ops` namespace for raw pointer operations

### 📊 Performance Comparison

| Platform | Operation | Scalar | SIMD | Speedup |
|----------|-----------|--------|------|---------|
| x86 AVX2 | AND-256 | 12.3 ns | 2.1 ns | **5.9×** |
| x86 AVX2 | XOR-512 | 24.8 ns | 4.3 ns | **5.8×** |
| ARM NEON | AND-128 | 8.4 ns | 3.2 ns | **2.6×** |

### ❌ Removed

- Virtual function overhead
- Singleton pattern
- Runtime type checking
- External dependencies (spdlog)

### ✅ Platform Support

- Linux (x86-64, ARM64, ARM32)
- Windows (x86-64, MSVC 2017+)
- macOS (x86-64, ARM64 Apple Silicon)

### 🔗 Links

- **Full Changelog**: [v1.0...v2.0.0](https://github.com/LessUp/bitcal/releases/tag/v2.0.0)
- **Migration Guide**: [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)

### ⚠️ Migration Notes

v1.x is **no longer maintained**. Users should migrate to v2.x.

See [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) for detailed migration instructions.

---

## [1.x] - Legacy

The original implementation based on inheritance and virtual functions.

**Status**: No longer maintained. Users should migrate to v2.x.

---

## Version History

| Version | Date | Status | Highlights |
|---------|------|--------|------------|
| v2.1.0 | 2026-04-16 | ✅ Stable | ANDNOT, performance improvements |
| v2.0.0 | 2026-01-08 | ✅ Stable | Complete rewrite, header-only |
| v1.x | 2025 | ⚠️ Legacy | Inheritance-based design |

---

[Unreleased]: https://github.com/LessUp/bitcal/compare/v2.1.0...HEAD
[2.1.0]: https://github.com/LessUp/bitcal/compare/v2.0.0...v2.1.0
[2.0.0]: https://github.com/LessUp/bitcal/releases/tag/v2.0.0
