# RFC 0002: SIMD Performance Optimizations (v2.1)

## Status

**Accepted** - Implemented in v2.1.0

## Context

BitCal v2.0 provided a solid foundation with compile-time SIMD dispatch, but several operations could be further optimized:

1. `operator~` (NOT) used scalar loop implementation
2. `is_zero()` checked words individually
3. `reverse()` allocated a temporary array
4. No native ANDNOT instruction support
5. MSVC SSE2 detection was broken on x64
6. NEON had compatibility issues with certain operations

## Decision

### 1. SIMD-Optimized NOT Operation

Replace scalar loop with native SIMD XOR against all-ones:

| Backend | Instruction | Implementation |
|---------|-------------|----------------|
| SSE2 | `_mm_xor_si128` | XOR with `_mm_set1_epi8(-1)` |
| AVX2 | `_mm256_xor_si256` | XOR with `_mm256_set1_epi8(-1)` |
| NEON | `veorq_u64` | XOR with `vdupq_n_u64(-1)` |

**Performance Impact:**
- 128-bit: 3.5ns → 2.1ns (1.67× faster)
- 256-bit: Similar improvement

### 2. Single-Instruction `is_zero()`

Use SIMD test instructions to check all bits at once:

| Backend | Instruction | Description |
|---------|-------------|-------------|
| SSE2 | `_mm_movemask_epi8` + comparison | Check if all bytes zero |
| AVX2 | `_mm256_testz_si256` | Test if AND with self is zero |
| NEON | `vceqq_u64` + lane check | Compare with zero, check results |

**Performance Impact:**
- 256-bit: 4.2ns → 1.8ns (2.33× faster)

### 3. In-Place `reverse()`

Eliminate temporary array allocation by using two-pass algorithm:

```cpp
void reverse() {
    // Step 1: Reverse bits within each 64-bit word
    for (size_t i = 0; i < u64_count; ++i) {
        data_[i] = reverse_bits_64(data_[i]);
    }

    // Step 2: Swap words from both ends
    for (size_t i = 0, j = u64_count - 1; i < j; ++i, --j) {
        std::swap(data_[i], data_[j]);
    }
}
```

**Performance Impact:**
- 256-bit: 12.1ns → 8.5ns (1.42× faster)
- Reduced memory allocation

### 4. Native ANDNOT Support

Add `andnot()` method using native SIMD instructions:

| Backend | Instruction |
|---------|-------------|
| SSE2 | `_mm_andnot_si128` |
| AVX2 | `_mm256_andnot_si256` |
| NEON | `vbicq_u64` (bit clear) |

**API:**
```cpp
bitarray andnot(const bitarray& mask) const;
```

**Performance Impact:**
- ~2× faster than separate NOT + AND operations

### 5. Cross-Lane Shift Fix (AVX2)

Fixed critical bug where `_mm256_slli_si256` operates independently on 128-bit lanes, not shifting across lanes.

**Solution:** Two-phase approach:
1. Word-level shift (count ≥ 64): Scalar whole-word movement
2. Bit-level shift (count < 64): SIMD parallel shift + carry propagation

```
Phase 1: word-shift (scalar)
  data[3] = data[2], data[2] = data[1], data[1] = data[0], data[0] = 0

Phase 2: bit-shift (AVX2)
  shifted = _mm256_slli_epi64(v, count)
  carry   = _mm256_permute4x64_epi64(v, 0x93)
  carry   = _mm256_srli_epi64(carry, 64-count)
  carry   = _mm256_blend_epi32(carry, zero, 0x03)
  result  = shifted | carry
```

### 6. Platform Bug Fixes

#### MSVC SSE2 Detection
- **Problem:** MSVC doesn't define `__SSE2__` on x64
- **Fix:** Detect via `_M_X64` macro

```cpp
#if defined(_M_X64)
    #define BITCAL_HAS_SSE2 1
#endif
```

#### NEON Compatibility
- **Problem:** `vmvnq_u64` not available on all ARM targets
- **Fix:** Use `veorq_u64` XOR with all-ones

- **Problem:** Variable shifts not working
- **Fix:** Use `vshlq_u64` instead of `vshlq_n_u64`

## Consequences

### Performance Improvements (Summary)

| Operation | v2.0 | v2.1 | Speedup |
|-----------|------|------|---------|
| NOT (256-bit) | 3.5 ns | 2.1 ns | **1.67×** |
| is_zero (256-bit) | 4.2 ns | 1.8 ns | **2.33×** |
| reverse (256-bit) | 12.1 ns | 8.5 ns | **1.42×** |
| ANDNOT (new) | N/A | ~2ns | **2× vs NOT+AND** |

### API Additions
- New `andnot()` method on all `bitarray` types
- New benchmark suite using Google Benchmark

### Backward Compatibility
- **100% backward compatible** with v2.0.x
- No breaking changes to existing APIs
- All new features are additive

## Alternatives Considered

### Alternative 1: Runtime SIMD Detection
- **Rejected:** Adds overhead, compile-time selection is sufficient

### Alternative 2: Separate Shift Implementations Per Backend
- **Rejected:** Code duplication, two-phase approach is cleaner

## References

- [SIMD Dispatch](../../docs/architecture/simd-dispatch.md)
- [Core Operations](../../docs/api/core-operations.md)
- [Shift Operations](../../docs/api/shift-operations.md)
