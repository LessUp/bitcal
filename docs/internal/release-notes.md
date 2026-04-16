# BitCal Release Notes

## v2.1.0 (2026-04-16)

### Highlights

#### New Features
- ✨ **ANDNOT Operation** — Native SIMD instruction support for `a & ~b`
  - Up to 2× faster than separate NOT + AND
  - Available on SSE2, AVX2, and NEON backends
- 📊 **Performance Benchmarks** — New benchmark suite using Google Benchmark
- 🧪 **bit1024 Test Coverage** — Full unit test coverage for 1024-bit operations

#### Performance Improvements

| Operation | v2.0 | v2.1 | Speedup |
|-----------|------|------|---------|
| NOT (256-bit) | 3.5 ns | 2.1 ns | **1.67×** |
| is_zero (256-bit) | 4.2 ns | 1.8 ns | **2.33×** |
| reverse (256-bit) | 12.1 ns | 8.5 ns | **1.42×** |

- SIMD-optimized `operator~` for 128/256/512-bit
- Single-instruction `is_zero()` via `_mm256_testz`
- In-place `reverse()` eliminating temporary allocation

#### Bug Fixes
- ✅ Fixed MSVC SSE2 macro detection on x64
- ✅ Fixed NEON `vmvnq_u64` compatibility (ARM)
- ✅ Fixed NEON variable shift support
- ✅ Fixed AVX2 cross-lane carry propagation in shifts

### Compatibility

- **C++ Standard**: C++17 minimum
- **Breaking Changes**: None (backward compatible with v2.0.x)
- **Platforms**: Linux, Windows, macOS (x86-64, ARM64, ARM32)

---

## v2.0.0 (2026-01-08)

### Major Redesign

Complete rewrite from OOP inheritance to C++17 template design.

#### Key Changes
- 🔄 **Architecture**: Inheritance → Templates
- ⚡ **Dispatch**: Runtime → Compile-time (`if constexpr`)
- 📦 **Distribution**: Compiled library → Header-only
- 🎯 **API**: Method calls → Operator overloading
- 🌍 **Platforms**: x86 only → x86 + ARM

#### Performance
- 5-6× speedup for 256-bit operations (AVX2)
- Zero runtime overhead from dispatch
- Automatic SIMD backend selection

#### Breaking Changes
- Complete API redesign
- Singleton pattern removed
- New include path: `<bitcal/bitcal.hpp>`

See [Migration Guide](../setup/migration-guide.md) for upgrade path.
