#pragma once

#include "config.hpp"
#include "simd_traits.hpp"
#include "scalar_ops.hpp"  // Required for popcount fallback

#if BITCAL_HAS_AVX2

#include <immintrin.h>

namespace bitcal {
namespace avx {

BITCAL_FORCEINLINE __m256i load(const uint64_t* ptr) noexcept {
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
}

BITCAL_FORCEINLINE __m256i load_aligned(const uint64_t* ptr) noexcept {
    return _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
}

BITCAL_FORCEINLINE void store(uint64_t* ptr, __m256i value) noexcept {
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), value);
}

BITCAL_FORCEINLINE void store_aligned(uint64_t* ptr, __m256i value) noexcept {
    _mm256_store_si256(reinterpret_cast<__m256i*>(ptr), value);
}

BITCAL_FORCEINLINE __m256i bit_and(__m256i a, __m256i b) noexcept {
    return _mm256_and_si256(a, b);
}

BITCAL_FORCEINLINE __m256i bit_or(__m256i a, __m256i b) noexcept {
    return _mm256_or_si256(a, b);
}

BITCAL_FORCEINLINE __m256i bit_xor(__m256i a, __m256i b) noexcept {
    return _mm256_xor_si256(a, b);
}

BITCAL_FORCEINLINE __m256i bit_not(__m256i a) noexcept {
    return _mm256_xor_si256(a, _mm256_set1_epi32(-1));
}

BITCAL_FORCEINLINE __m256i shift_left_64(__m256i a, int count) noexcept {
    return _mm256_slli_epi64(a, count);
}

BITCAL_FORCEINLINE __m256i shift_right_64(__m256i a, int count) noexcept {
    return _mm256_srli_epi64(a, count);
}

BITCAL_FORCEINLINE void shift_left_256(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 256) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        return;
    }
    
    if (count >= 128) {
        data[3] = data[1];
        data[2] = data[0];
        data[1] = 0;
        data[0] = 0;
        count -= 128;
        if (count <= 0) return;
    }
    
    if (count >= 64) {
        data[3] = data[2];
        data[2] = data[1];
        data[1] = data[0];
        data[0] = 0;
        count -= 64;
        if (count <= 0) return;
    }
    
    __m256i v = load(data);
    __m256i v_shifted = _mm256_slli_epi64(v, count);
    __m256i carry = _mm256_permute4x64_epi64(v, 0x93);
    carry = _mm256_srli_epi64(carry, 64 - count);
    carry = _mm256_blend_epi32(carry, _mm256_setzero_si256(), 0x03);
    v = _mm256_or_si256(v_shifted, carry);
    
    store(data, v);
}

BITCAL_FORCEINLINE void shift_right_256(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 256) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        return;
    }
    
    if (count >= 128) {
        data[0] = data[2];
        data[1] = data[3];
        data[2] = 0;
        data[3] = 0;
        count -= 128;
        if (count <= 0) return;
    }
    
    if (count >= 64) {
        data[0] = data[1];
        data[1] = data[2];
        data[2] = data[3];
        data[3] = 0;
        count -= 64;
        if (count <= 0) return;
    }
    
    __m256i v = load(data);
    __m256i v_shifted = _mm256_srli_epi64(v, count);
    __m256i carry = _mm256_permute4x64_epi64(v, 0x39);
    carry = _mm256_slli_epi64(carry, 64 - count);
    carry = _mm256_blend_epi32(carry, _mm256_setzero_si256(), 0xC0);
    v = _mm256_or_si256(v_shifted, carry);
    
    store(data, v);
}

BITCAL_FORCEINLINE void bit_and_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = load(a);
    __m256i vb = load(b);
    store(out, bit_and(va, vb));
}

BITCAL_FORCEINLINE void bit_or_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = load(a);
    __m256i vb = load(b);
    store(out, bit_or(va, vb));
}

BITCAL_FORCEINLINE void bit_xor_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = load(a);
    __m256i vb = load(b);
    store(out, bit_xor(va, vb));
}

BITCAL_FORCEINLINE void shift_left_512(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 512) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data + 4), _mm256_setzero_si256());
        return;
    }
    
    const int word_shift = count / 64;
    const int bit_shift = count % 64;
    
    if (word_shift > 0) {
        for (int i = 7; i >= word_shift; --i) {
            data[i] = data[i - word_shift];
        }
        for (int i = 0; i < word_shift; ++i) {
            data[i] = 0;
        }
    }
    
    if (bit_shift == 0) return;
    
    __m256i lo = load(data);
    __m256i hi = load(data + 4);
    
    uint64_t cross_carry = data[3] >> (64 - bit_shift);
    
    __m256i lo_shifted = _mm256_slli_epi64(lo, bit_shift);
    __m256i lo_carry = _mm256_permute4x64_epi64(lo, 0x93);
    lo_carry = _mm256_srli_epi64(lo_carry, 64 - bit_shift);
    lo_carry = _mm256_blend_epi32(lo_carry, _mm256_setzero_si256(), 0x03);
    lo = _mm256_or_si256(lo_shifted, lo_carry);
    
    __m256i hi_shifted = _mm256_slli_epi64(hi, bit_shift);
    __m256i hi_carry = _mm256_permute4x64_epi64(hi, 0x93);
    hi_carry = _mm256_srli_epi64(hi_carry, 64 - bit_shift);
    hi_carry = _mm256_blend_epi32(hi_carry, _mm256_setzero_si256(), 0x03);
    hi = _mm256_or_si256(hi_shifted, hi_carry);
    
    hi = _mm256_insert_epi64(hi, _mm256_extract_epi64(hi, 0) | cross_carry, 0);
    
    store(data, lo);
    store(data + 4, hi);
}

BITCAL_FORCEINLINE void shift_right_512(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 512) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data + 4), _mm256_setzero_si256());
        return;
    }
    
    const int word_shift = count / 64;
    const int bit_shift = count % 64;
    
    if (word_shift > 0) {
        for (int i = 0; i < 8 - word_shift; ++i) {
            data[i] = data[i + word_shift];
        }
        for (int i = 8 - word_shift; i < 8; ++i) {
            data[i] = 0;
        }
    }
    
    if (bit_shift == 0) return;
    
    __m256i lo = load(data);
    __m256i hi = load(data + 4);
    
    uint64_t cross_carry = data[4] << (64 - bit_shift);
    
    __m256i lo_shifted = _mm256_srli_epi64(lo, bit_shift);
    __m256i lo_carry = _mm256_permute4x64_epi64(lo, 0x39);
    lo_carry = _mm256_slli_epi64(lo_carry, 64 - bit_shift);
    lo_carry = _mm256_blend_epi32(lo_carry, _mm256_setzero_si256(), 0xC0);
    lo = _mm256_or_si256(lo_shifted, lo_carry);
    
    __m256i hi_shifted = _mm256_srli_epi64(hi, bit_shift);
    __m256i hi_carry = _mm256_permute4x64_epi64(hi, 0x39);
    hi_carry = _mm256_slli_epi64(hi_carry, 64 - bit_shift);
    hi_carry = _mm256_blend_epi32(hi_carry, _mm256_setzero_si256(), 0xC0);
    hi = _mm256_or_si256(hi_shifted, hi_carry);
    
    lo = _mm256_insert_epi64(lo, _mm256_extract_epi64(lo, 3) | cross_carry, 3);
    
    store(data, lo);
    store(data + 4, hi);
}

BITCAL_FORCEINLINE void bit_and_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i a0 = load(a);
    __m256i a1 = load(a + 4);
    __m256i b0 = load(b);
    __m256i b1 = load(b + 4);
    store(out, bit_and(a0, b0));
    store(out + 4, bit_and(a1, b1));
}

BITCAL_FORCEINLINE void bit_or_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i a0 = load(a);
    __m256i a1 = load(a + 4);
    __m256i b0 = load(b);
    __m256i b1 = load(b + 4);
    store(out, bit_or(a0, b0));
    store(out + 4, bit_or(a1, b1));
}

BITCAL_FORCEINLINE void bit_xor_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i a0 = load(a);
    __m256i a1 = load(a + 4);
    __m256i b0 = load(b);
    __m256i b1 = load(b + 4);
    store(out, bit_xor(a0, b0));
    store(out + 4, bit_xor(a1, b1));
}

BITCAL_FORCEINLINE void bit_not_256(const uint64_t* in, uint64_t* out) noexcept {
    __m256i v = load(in);
    store(out, bit_not(v));
}

BITCAL_FORCEINLINE void bit_not_512(const uint64_t* in, uint64_t* out) noexcept {
    __m256i v0 = load(in);
    __m256i v1 = load(in + 4);
    store(out, bit_not(v0));
    store(out + 4, bit_not(v1));
}

BITCAL_FORCEINLINE bool is_zero_256(const uint64_t* data) noexcept {
    __m256i v = load(data);
    return _mm256_testz_si256(v, v) != 0;
}

BITCAL_FORCEINLINE bool is_zero_512(const uint64_t* data) noexcept {
    __m256i v0 = load(data);
    __m256i v1 = load(data + 4);
    __m256i combined = _mm256_or_si256(v0, v1);
    return _mm256_testz_si256(combined, combined) != 0;
}

BITCAL_FORCEINLINE void bit_andnot_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = load(a);
    __m256i vb = load(b);
    store(out, _mm256_andnot_si256(vb, va));
}

BITCAL_FORCEINLINE void bit_andnot_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i a0 = load(a);
    __m256i a1 = load(a + 4);
    __m256i b0 = load(b);
    __m256i b1 = load(b + 4);
    store(out, _mm256_andnot_si256(b0, a0));
    store(out + 4, _mm256_andnot_si256(b1, a1));
}

// ============================================================================
// Popcount using SIMD vectorized approach (Harley-Seal algorithm variant)
// ============================================================================

namespace detail {

// Lookup table for popcount of 4-bit nibbles (used by SSSE3 PSHUFB method)
// This is stored as a constant for the Harley-Seal SIMD popcount
BITCAL_FORCEINLINE __m256i popcount_lut() noexcept {
    // 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 repeated for 16 bytes
    return _mm256_setr_epi8(
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
    );
}

// Mask for low 4 bits of each byte
BITCAL_FORCEINLINE __m256i low_nibble_mask() noexcept {
    return _mm256_set1_epi8(0x0F);
}

}  // namespace detail

#if defined(__SSSE3__)
// SSSE3 PSHUFB-based popcount - true SIMD vectorization
BITCAL_FORCEINLINE __m256i popcount_bytes(__m256i v) noexcept {
    __m256i lut = detail::popcount_lut();
    __m256i low_mask = detail::low_nibble_mask();

    // Process low nibbles
    __m256i lo = _mm256_and_si256(v, low_mask);
    __m256i pop_lo = _mm256_shuffle_epi8(lut, lo);

    // Process high nibbles (shift right 4 bits)
    __m256i hi = _mm256_and_si256(_mm256_srli_epi16(v, 4), low_mask);
    __m256i pop_hi = _mm256_shuffle_epi8(lut, hi);

    // Sum byte popcounts
    return _mm256_add_epi8(pop_lo, pop_hi);
}

// Horizontal sum of all 32 bytes in a __m256i
BITCAL_FORCEINLINE uint64_t hsum_bytes_256(__m256i v) noexcept {
    // PSADBW computes sum of absolute differences for each 128-bit lane
    // For each lane: result = sum(abs(v[i] - 0)) for i in 0..7
    // So we get sum of first 8 bytes in each lane

    // First, get sum of bytes 0-7 and 16-23
    __m256i sad1 = _mm256_sad_epu8(v, _mm256_setzero_si256());

    // Shift right by 8 bytes to get bytes 8-15 and 24-31
    __m256i v_shifted = _mm256_srli_si256(v, 8);
    __m256i sad2 = _mm256_sad_epu8(v_shifted, _mm256_setzero_si256());

    // sad1 = [sum0_07, 0, sum1_1617, 0] (64-bit elements)
    // sad2 = [sum0_815, 0, sum1_2431, 0]
    // Total = sad1[0] + sad1[2] + sad2[0] + sad2[2]
    return _mm256_extract_epi64(sad1, 0) + _mm256_extract_epi64(sad1, 2) +
           _mm256_extract_epi64(sad2, 0) + _mm256_extract_epi64(sad2, 2);
}

BITCAL_FORCEINLINE uint64_t popcount_256(const uint64_t* data) noexcept {
    __m256i v = load(data);
    __m256i byte_counts = popcount_bytes(v);
    return hsum_bytes_256(byte_counts);
}

BITCAL_FORCEINLINE uint64_t popcount_512(const uint64_t* data) noexcept {
    __m256i v0 = load(data);
    __m256i v1 = load(data + 4);

    __m256i byte_counts0 = popcount_bytes(v0);
    __m256i byte_counts1 = popcount_bytes(v1);

    return hsum_bytes_256(byte_counts0) + hsum_bytes_256(byte_counts1);
}

#else
// Fallback for pre-SSSE3: extract and use scalar popcount
BITCAL_FORCEINLINE uint64_t popcount_256(const uint64_t* data) noexcept {
    __m256i v = load(data);
    return scalar::popcount(_mm256_extract_epi64(v, 0)) +
           scalar::popcount(_mm256_extract_epi64(v, 1)) +
           scalar::popcount(_mm256_extract_epi64(v, 2)) +
           scalar::popcount(_mm256_extract_epi64(v, 3));
}

BITCAL_FORCEINLINE uint64_t popcount_512(const uint64_t* data) noexcept {
    __m256i v0 = load(data);
    __m256i v1 = load(data + 4);

    return scalar::popcount(_mm256_extract_epi64(v0, 0)) +
           scalar::popcount(_mm256_extract_epi64(v0, 1)) +
           scalar::popcount(_mm256_extract_epi64(v0, 2)) +
           scalar::popcount(_mm256_extract_epi64(v0, 3)) +
           scalar::popcount(_mm256_extract_epi64(v1, 0)) +
           scalar::popcount(_mm256_extract_epi64(v1, 1)) +
           scalar::popcount(_mm256_extract_epi64(v1, 2)) +
           scalar::popcount(_mm256_extract_epi64(v1, 3));
}
#endif  // __SSSE3__

// ============================================================================
// all() operations - check if all bits are set (using SIMD)
// ============================================================================

BITCAL_FORCEINLINE bool all_256(const uint64_t* data) noexcept {
    __m256i v = load(data);
    __m256i all_ones = _mm256_set1_epi32(-1);
    __m256i cmp = _mm256_cmpeq_epi64(v, all_ones);
    // Check if all 4 elements are equal to all_ones (cmp result is all ones)
    return _mm256_testc_si256(cmp, all_ones) != 0;
}

BITCAL_FORCEINLINE bool all_512(const uint64_t* data) noexcept {
    __m256i v0 = load(data);
    __m256i v1 = load(data + 4);
    __m256i all_ones = _mm256_set1_epi32(-1);

    __m256i cmp0 = _mm256_cmpeq_epi64(v0, all_ones);
    __m256i cmp1 = _mm256_cmpeq_epi64(v1, all_ones);

    __m256i combined = _mm256_and_si256(cmp0, cmp1);
    return _mm256_testc_si256(combined, all_ones) != 0;
}

// ============================================================================
// Equality comparison using SIMD
// ============================================================================

BITCAL_FORCEINLINE bool equals_256(const uint64_t* a, const uint64_t* b) noexcept {
    __m256i va = load(a);
    __m256i vb = load(b);
    __m256i cmp = _mm256_cmpeq_epi64(va, vb);
    // Check if all 4 elements are equal (cmp result is all ones)
    return _mm256_testc_si256(cmp, _mm256_set1_epi32(-1)) != 0;
}

BITCAL_FORCEINLINE bool equals_512(const uint64_t* a, const uint64_t* b) noexcept {
    __m256i va0 = load(a);
    __m256i va1 = load(a + 4);
    __m256i vb0 = load(b);
    __m256i vb1 = load(b + 4);
    
    __m256i cmp0 = _mm256_cmpeq_epi64(va0, vb0);
    __m256i cmp1 = _mm256_cmpeq_epi64(va1, vb1);
    
    __m256i combined = _mm256_and_si256(cmp0, cmp1);
    return _mm256_testc_si256(combined, _mm256_set1_epi32(-1)) != 0;
}

}
}

#endif
