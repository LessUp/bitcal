#pragma once

#include "config.hpp"
#include "simd_traits.hpp"

#if BITCAL_HAS_AVX512

#include <immintrin.h>

namespace bitcal {
namespace avx512 {

// Load/store operations
BITCAL_FORCEINLINE __m512i load(const uint64_t* ptr) noexcept {
    return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr));
}

BITCAL_FORCEINLINE __m512i load_aligned(const uint64_t* ptr) noexcept {
    return _mm512_load_si512(reinterpret_cast<const __m512i*>(ptr));
}

BITCAL_FORCEINLINE void store(uint64_t* ptr, __m512i value) noexcept {
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(ptr), value);
}

BITCAL_FORCEINLINE void store_aligned(uint64_t* ptr, __m512i value) noexcept {
    _mm512_store_si512(reinterpret_cast<__m512i*>(ptr), value);
}

// Basic bitwise operations
BITCAL_FORCEINLINE __m512i bit_and(__m512i a, __m512i b) noexcept {
    return _mm512_and_si512(a, b);
}

BITCAL_FORCEINLINE __m512i bit_or(__m512i a, __m512i b) noexcept {
    return _mm512_or_si512(a, b);
}

BITCAL_FORCEINLINE __m512i bit_xor(__m512i a, __m512i b) noexcept {
    return _mm512_xor_si512(a, b);
}

BITCAL_FORCEINLINE __m512i bit_not(__m512i a) noexcept {
    return _mm512_xor_si512(a, _mm512_set1_epi32(-1));
}

// Shift operations (64-bit elements)
BITCAL_FORCEINLINE __m512i shift_left_64(__m512i a, int count) noexcept {
    return _mm512_slli_epi64(a, count);
}

BITCAL_FORCEINLINE __m512i shift_right_64(__m512i a, int count) noexcept {
    return _mm512_srli_epi64(a, count);
}

// 512-bit shift operations - optimized with AVX-512 instructions
BITCAL_FORCEINLINE void shift_left_512(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 512) {
        store(data, _mm512_setzero_si512());
        return;
    }

    const int word_shift = count / 64;
    const int bit_shift = count % 64;

    // Use AVX-512 alignr for word-level shift (if word_shift > 0)
    if (word_shift > 0) {
        __m512i v = load(data);
        // _mm512_alignr_epi64 shifts elements within the register
        // For left shift by N words, we need to shift right by (8-N) and zero low elements
        __m512i shifted = _mm512_maskz_alignr_epi64(
            (0xFF << word_shift),  // mask: keep upper (8-word_shift) elements
            v, v,
            8 - word_shift  // alignr index
        );
        store(data, shifted);
    }

    if (bit_shift == 0) return;

    __m512i v = load(data);
    __m512i v_shifted = _mm512_slli_epi64(v, bit_shift);

    // Create carry vector - each element gets the high bits from the previous element
    __m512i carry = _mm512_permutexvar_epi64(
        _mm512_set_epi64(6, 5, 4, 3, 2, 1, 0, -1),  // permutation indices
        v
    );
    carry = _mm512_srli_epi64(carry, 64 - bit_shift);

    // Mask to zero out the lowest element's carry (no previous element)
    __mmask8 mask = 0xFE;  // 11111110 - zero out element 0
    carry = _mm512_mask_blend_epi64(mask, _mm512_setzero_si512(), carry);

    v = _mm512_or_si512(v_shifted, carry);
    store(data, v);
}

BITCAL_FORCEINLINE void shift_right_512(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 512) {
        store(data, _mm512_setzero_si512());
        return;
    }

    const int word_shift = count / 64;
    const int bit_shift = count % 64;

    // Use AVX-512 alignr for word-level shift (if word_shift > 0)
    if (word_shift > 0) {
        __m512i v = load(data);
        // For right shift by N words, shift left by N and zero high elements
        __m512i shifted = _mm512_maskz_alignr_epi64(
            (0xFF >> word_shift),  // mask: keep lower (8-word_shift) elements
            v, v,
            word_shift  // alignr index
        );
        store(data, shifted);
    }

    if (bit_shift == 0) return;

    __m512i v = load(data);
    __m512i v_shifted = _mm512_srli_epi64(v, bit_shift);

    // Create carry vector - each element gets the low bits from the next element
    __m512i carry = _mm512_permutexvar_epi64(
        _mm512_set_epi64(-1, 7, 6, 5, 4, 3, 2, 1),  // permutation indices
        v
    );
    carry = _mm512_slli_epi64(carry, 64 - bit_shift);

    // Mask to zero out the highest element's carry (no next element)
    __mmask8 mask = 0x7F;  // 01111111 - zero out element 7
    carry = _mm512_mask_blend_epi64(mask, _mm512_setzero_si512(), carry);

    v = _mm512_or_si512(v_shifted, carry);
    store(data, v);
}

// 256-bit operations using VL (Vector Length) extensions
BITCAL_FORCEINLINE void shift_left_256_vl(uint64_t* data, int count) noexcept {
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

    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
    __m256i v_shifted = _mm256_slli_epi64(v, count);

    // Permute to get carry from adjacent elements
    __m256i carry = _mm256_permute4x64_epi64(v, 0x93);  // 10 01 00 11 = 0x93
    carry = _mm256_srli_epi64(carry, 64 - count);
    carry = _mm256_blend_epi32(carry, _mm256_setzero_si256(), 0x03);  // Zero first 64 bits

    v = _mm256_or_si256(v_shifted, carry);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), v);
}

BITCAL_FORCEINLINE void shift_right_256_vl(uint64_t* data, int count) noexcept {
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

    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
    __m256i v_shifted = _mm256_srli_epi64(v, count);

    __m256i carry = _mm256_permute4x64_epi64(v, 0x39);  // 00 11 10 01 = 0x39
    carry = _mm256_slli_epi64(carry, 64 - count);
    carry = _mm256_blend_epi32(carry, _mm256_setzero_si256(), 0xC0);  // Zero last 64 bits

    v = _mm256_or_si256(v_shifted, carry);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), v);
}

// Bitwise operations on 512-bit arrays
BITCAL_FORCEINLINE void bit_and_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    store(out, bit_and(load(a), load(b)));
}

BITCAL_FORCEINLINE void bit_or_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    store(out, bit_or(load(a), load(b)));
}

BITCAL_FORCEINLINE void bit_xor_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    store(out, bit_xor(load(a), load(b)));
}

BITCAL_FORCEINLINE void bit_not_512(const uint64_t* in, uint64_t* out) noexcept {
    store(out, bit_not(load(in)));
}

BITCAL_FORCEINLINE void bit_andnot_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    // AVX-512 andnot: a & ~b (note: order differs from AVX2)
    store(out, _mm512_andnot_si512(load(b), load(a)));
}

// 256-bit operations
BITCAL_FORCEINLINE void bit_and_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(b));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), _mm256_and_si256(va, vb));
}

BITCAL_FORCEINLINE void bit_or_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(b));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), _mm256_or_si256(va, vb));
}

BITCAL_FORCEINLINE void bit_xor_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(b));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), _mm256_xor_si256(va, vb));
}

BITCAL_FORCEINLINE void bit_not_256(const uint64_t* in, uint64_t* out) noexcept {
    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), _mm256_xor_si256(v, _mm256_set1_epi32(-1)));
}

BITCAL_FORCEINLINE void bit_andnot_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m256i va = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(a));
    __m256i vb = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(b));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), _mm256_andnot_si256(vb, va));
}

// ============================================================================
// Popcount using AVX-512 VPOPCNTDQ instructions
// ============================================================================

#if defined(__AVX512VPOPCNTDQ__) || (defined(_MSC_VER) && defined(__AVX512VPOPCNTDQ__))
#define BITCAL_HAS_VPOPCNTDQ 1

BITCAL_FORCEINLINE uint64_t popcount_512(const uint64_t* data) noexcept {
    __m512i v = load(data);
    // VPOPCNTDQ: count bits set in each 64-bit element
    __m512i counts = _mm512_popcnt_epi64(v);
    // Reduce: sum all 8 elements
    // Use horizontal add by adding pairs recursively
    __m256i lo = _mm512_castsi512_si256(counts);
    __m256i hi = _mm512_extracti64x4_epi64(counts, 1);
    __m256i sum256 = _mm256_add_epi64(lo, hi);
    __m128i sum128 = _mm_add_epi64(
        _mm256_castsi256_si128(sum256),
        _mm256_extracti128_si256(sum256, 1)
    );
    sum128 = _mm_add_epi64(sum128, _mm_unpackhi_epi64(sum128, sum128));
    return _mm_cvtsi128_si64(sum128);
}

BITCAL_FORCEINLINE uint64_t popcount_256(const uint64_t* data) noexcept {
    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
    // Extend to 512-bit, use VPOPCNTDQ, then extract result
    __m512i v512 = _mm512_castsi256_si512(v);
    __m512i counts = _mm512_popcnt_epi64(v512);
    __m256i counts256 = _mm512_castsi512_si256(counts);
    // Sum 4 elements
    __m128i lo = _mm256_castsi256_si128(counts256);
    __m128i hi = _mm256_extracti128_si256(counts256, 1);
    __m128i sum = _mm_add_epi64(lo, hi);
    sum = _mm_add_epi64(sum, _mm_unpackhi_epi64(sum, sum));
    return _mm_cvtsi128_si64(sum);
}

#else
#define BITCAL_HAS_VPOPCNTDQ 0

// Fallback: extract and use scalar popcount
BITCAL_FORCEINLINE uint64_t popcount_512(const uint64_t* data) noexcept {
    __m512i v = load(data);
    return scalar::popcount(_mm512_extract_epi64(v, 0)) +
           scalar::popcount(_mm512_extract_epi64(v, 1)) +
           scalar::popcount(_mm512_extract_epi64(v, 2)) +
           scalar::popcount(_mm512_extract_epi64(v, 3)) +
           scalar::popcount(_mm512_extract_epi64(v, 4)) +
           scalar::popcount(_mm512_extract_epi64(v, 5)) +
           scalar::popcount(_mm512_extract_epi64(v, 6)) +
           scalar::popcount(_mm512_extract_epi64(v, 7));
}

BITCAL_FORCEINLINE uint64_t popcount_256(const uint64_t* data) noexcept {
    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
    return scalar::popcount(_mm256_extract_epi64(v, 0)) +
           scalar::popcount(_mm256_extract_epi64(v, 1)) +
           scalar::popcount(_mm256_extract_epi64(v, 2)) +
           scalar::popcount(_mm256_extract_epi64(v, 3));
}

#endif  // BITCAL_HAS_VPOPCNTDQ

// ============================================================================
// all() operations - check if all bits are set
// ============================================================================

BITCAL_FORCEINLINE bool all_256(const uint64_t* data) noexcept {
    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
    __m256i all_ones = _mm256_set1_epi32(-1);
    __m256i cmp = _mm256_cmpeq_epi64(v, all_ones);
    return _mm256_testc_si256(cmp, all_ones) != 0;
}

BITCAL_FORCEINLINE bool all_512(const uint64_t* data) noexcept {
    __m512i v = load(data);
    __mmask8 mask = _mm512_cmpeq_epi64_mask(v, _mm512_set1_epi32(-1));
    return mask == 0xFF;
}

// ============================================================================
// is_zero operations using AVX-512 mask registers
// ============================================================================

BITCAL_FORCEINLINE bool is_zero_256(const uint64_t* data) noexcept {
    __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
    // Test if all bits are zero - returns mask
    return _mm256_testz_si256(v, v) != 0;
}

BITCAL_FORCEINLINE bool is_zero_512(const uint64_t* data) noexcept {
    __m512i v = load(data);
    // Compare with zero - returns mask of all 1s if equal
    __mmask8 mask = _mm512_cmpeq_epi64_mask(v, _mm512_setzero_si512());
    return mask == 0xFF;  // All 8 elements are zero
}

}  // namespace avx512
}  // namespace bitcal

#endif  // BITCAL_HAS_AVX512
