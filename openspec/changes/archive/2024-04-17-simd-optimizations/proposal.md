# Proposal: SIMD Performance Optimizations (v2.1)

## Why

BitCal v2.0 provided a solid foundation with compile-time SIMD dispatch, but several operations could be further optimized:
1. `operator~` (NOT) used scalar loop implementation
2. `is_zero()` checked words individually
3. `reverse()` allocated a temporary array
4. No native ANDNOT instruction support
5. MSVC SSE2 detection was broken on x64
6. NEON had compatibility issues with certain operations

## What

Optimize all identified performance bottlenecks:
- SIMD-optimized NOT operation
- Single-instruction is_zero()
- In-place reverse()
- Native ANDNOT support
- Cross-lane shift fix for AVX2
- Platform bug fixes for MSVC and NEON

## Impact

- Performance improvements: NOT 1.67×, is_zero 2.33×, reverse 1.42×
- New API: `andnot()` method
- 100% backward compatible with v2.0.x
- Bug fixes for MSVC and NEON backends

## Related

- RFC 0002: SIMD Optimizations
- Product Spec: Bit Manipulation Library
