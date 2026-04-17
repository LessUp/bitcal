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

// 512-bit shift operations
BITCAL_FORCEINLINE void shift_left_512(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 512) {
        store(data, _mm512_setzero_si512());
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

    __m512i v = load(data);
    __m512i v_shifted = _mm512_slli_epi64(v, bit_shift);

    // Create carry vector - each element gets the high bits from the previous element
    // Using permute to shift elements right by 1 (with zero fill for first element)
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

    if (word_shift > 0) {
        for (int i = 0; i < 8 - word_shift; ++i) {
            data[i] = data[i + word_shift];
        }
        for (int i = 8 - word_shift; i < 8; ++i) {
            data[i] = 0;
        }
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

// is_zero operations using AVX-512 mask registers
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
