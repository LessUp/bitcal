#pragma once

#include "config.hpp"
#include "simd_traits.hpp"

#if BITCAL_HAS_SSE2

#include <emmintrin.h>
#if defined(__SSSE3__)
#include <tmmintrin.h>
#endif

namespace bitcal {
namespace sse {

BITCAL_FORCEINLINE __m128i load(const uint64_t* ptr) noexcept {
    return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
}

BITCAL_FORCEINLINE __m128i load_aligned(const uint64_t* ptr) noexcept {
    return _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
}

BITCAL_FORCEINLINE void store(uint64_t* ptr, __m128i value) noexcept {
    _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr), value);
}

BITCAL_FORCEINLINE void store_aligned(uint64_t* ptr, __m128i value) noexcept {
    _mm_store_si128(reinterpret_cast<__m128i*>(ptr), value);
}

BITCAL_FORCEINLINE __m128i bit_and(__m128i a, __m128i b) noexcept {
    return _mm_and_si128(a, b);
}

BITCAL_FORCEINLINE __m128i bit_or(__m128i a, __m128i b) noexcept {
    return _mm_or_si128(a, b);
}

BITCAL_FORCEINLINE __m128i bit_xor(__m128i a, __m128i b) noexcept {
    return _mm_xor_si128(a, b);
}

BITCAL_FORCEINLINE __m128i bit_not(__m128i a) noexcept {
    return _mm_xor_si128(a, _mm_set1_epi32(-1));
}

BITCAL_FORCEINLINE __m128i shift_left_64(__m128i a, int count) noexcept {
    return _mm_slli_epi64(a, count);
}

BITCAL_FORCEINLINE __m128i shift_right_64(__m128i a, int count) noexcept {
    return _mm_srli_epi64(a, count);
}

BITCAL_FORCEINLINE void shift_left_128(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 128) {
        data[0] = data[1] = 0;
        return;
    }
    
    __m128i v = load(data);
    
    if (count >= 64) {
        __m128i low = _mm_slli_si128(v, 8);
        v = _mm_slli_epi64(low, count - 64);
    } else {
        __m128i high_part = _mm_slli_si128(v, 8);
        high_part = _mm_srli_epi64(high_part, 64 - count);
        v = _mm_slli_epi64(v, count);
        v = _mm_or_si128(v, high_part);
    }
    
    store(data, v);
}

BITCAL_FORCEINLINE void shift_right_128(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 128) {
        data[0] = data[1] = 0;
        return;
    }
    
    __m128i v = load(data);
    
    if (count >= 64) {
        __m128i high = _mm_srli_si128(v, 8);
        v = _mm_srli_epi64(high, count - 64);
    } else {
        __m128i low_part = _mm_srli_si128(v, 8);
        low_part = _mm_slli_epi64(low_part, 64 - count);
        v = _mm_srli_epi64(v, count);
        v = _mm_or_si128(v, low_part);
    }
    
    store(data, v);
}

BITCAL_FORCEINLINE void bit_and_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i va = load(a);
    __m128i vb = load(b);
    store(out, bit_and(va, vb));
}

BITCAL_FORCEINLINE void bit_or_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i va = load(a);
    __m128i vb = load(b);
    store(out, bit_or(va, vb));
}

BITCAL_FORCEINLINE void bit_xor_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i va = load(a);
    __m128i vb = load(b);
    store(out, bit_xor(va, vb));
}

BITCAL_FORCEINLINE void shift_left_256(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 256) {
        data[0] = data[1] = data[2] = data[3] = 0;
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
    
    __m128i lo = load(data);
    __m128i hi = load(data + 2);
    
    uint64_t cross_carry = data[1] >> (64 - count);
    
    __m128i lo_carry = _mm_slli_si128(lo, 8);
    lo_carry = _mm_srli_epi64(lo_carry, 64 - count);
    lo = _mm_slli_epi64(lo, count);
    lo = _mm_or_si128(lo, lo_carry);
    
    __m128i hi_carry = _mm_slli_si128(hi, 8);
    hi_carry = _mm_srli_epi64(hi_carry, 64 - count);
    hi = _mm_slli_epi64(hi, count);
    hi = _mm_or_si128(hi, hi_carry);
    
    hi = _mm_or_si128(hi, _mm_set_epi64x(0, static_cast<int64_t>(cross_carry)));
    
    store(data, lo);
    store(data + 2, hi);
}

BITCAL_FORCEINLINE void shift_right_256(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 256) {
        data[0] = data[1] = data[2] = data[3] = 0;
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
    
    __m128i lo = load(data);
    __m128i hi = load(data + 2);
    
    uint64_t cross_carry = data[2] << (64 - count);
    
    __m128i lo_carry = _mm_srli_si128(lo, 8);
    lo_carry = _mm_slli_epi64(lo_carry, 64 - count);
    lo = _mm_srli_epi64(lo, count);
    lo = _mm_or_si128(lo, lo_carry);
    
    __m128i hi_carry = _mm_srli_si128(hi, 8);
    hi_carry = _mm_slli_epi64(hi_carry, 64 - count);
    hi = _mm_srli_epi64(hi, count);
    hi = _mm_or_si128(hi, hi_carry);
    
    lo = _mm_or_si128(lo, _mm_set_epi64x(static_cast<int64_t>(cross_carry), 0));
    
    store(data, lo);
    store(data + 2, hi);
}

BITCAL_FORCEINLINE void bit_and_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i a0 = load(a);
    __m128i a1 = load(a + 2);
    __m128i b0 = load(b);
    __m128i b1 = load(b + 2);
    store(out, bit_and(a0, b0));
    store(out + 2, bit_and(a1, b1));
}

BITCAL_FORCEINLINE void bit_or_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i a0 = load(a);
    __m128i a1 = load(a + 2);
    __m128i b0 = load(b);
    __m128i b1 = load(b + 2);
    store(out, bit_or(a0, b0));
    store(out + 2, bit_or(a1, b1));
}

BITCAL_FORCEINLINE void bit_xor_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i a0 = load(a);
    __m128i a1 = load(a + 2);
    __m128i b0 = load(b);
    __m128i b1 = load(b + 2);
    store(out, bit_xor(a0, b0));
    store(out + 2, bit_xor(a1, b1));
}

BITCAL_FORCEINLINE void bit_not_128(const uint64_t* in, uint64_t* out) noexcept {
    __m128i v = load(in);
    store(out, bit_not(v));
}

BITCAL_FORCEINLINE void bit_not_256(const uint64_t* in, uint64_t* out) noexcept {
    __m128i v0 = load(in);
    __m128i v1 = load(in + 2);
    store(out, bit_not(v0));
    store(out + 2, bit_not(v1));
}

BITCAL_FORCEINLINE bool is_zero_128(const uint64_t* data) noexcept {
    __m128i v = load(data);
    return _mm_movemask_epi8(_mm_cmpeq_epi8(v, _mm_setzero_si128())) == 0xFFFF;
}

BITCAL_FORCEINLINE bool is_zero_256(const uint64_t* data) noexcept {
    __m128i v0 = load(data);
    __m128i v1 = load(data + 2);
    __m128i combined = _mm_or_si128(v0, v1);
    return _mm_movemask_epi8(_mm_cmpeq_epi8(combined, _mm_setzero_si128())) == 0xFFFF;
}

BITCAL_FORCEINLINE void bit_andnot_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i va = load(a);
    __m128i vb = load(b);
    store(out, _mm_andnot_si128(vb, va));
}

BITCAL_FORCEINLINE void bit_andnot_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    __m128i a0 = load(a);
    __m128i a1 = load(a + 2);
    __m128i b0 = load(b);
    __m128i b1 = load(b + 2);
    store(out, _mm_andnot_si128(b0, a0));
    store(out + 2, _mm_andnot_si128(b1, a1));
}

}
}

#endif
