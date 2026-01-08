#pragma once

#include "config.hpp"
#include "simd_traits.hpp"

#if BITCAL_HAS_AVX2

#include <immintrin.h>

namespace bitcal {
namespace avx {

BITCAL_FORCEINLINE __m256i load(const uint64_t* ptr) noexcept {
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
}

BITCAL_FORCEINLINE void store(uint64_t* ptr, __m256i value) noexcept {
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), value);
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
    if (count == 0) return;
    if (count >= 256) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        return;
    }
    
    __m256i v = load(data);
    
    if (count >= 64) {
        const int byte_shift = (count / 64) * 8;
        v = _mm256_permute4x64_epi64(v, 0x93);
        v = _mm256_slli_si256(v, byte_shift);
        count %= 64;
    }
    
    if (count > 0) {
        __m256i v_shifted = _mm256_slli_epi64(v, count);
        __m256i carry = _mm256_permute4x64_epi64(v, 0x93);
        carry = _mm256_srli_epi64(carry, 64 - count);
        carry = _mm256_blend_epi32(carry, _mm256_setzero_si256(), 0x03);
        v = _mm256_or_si256(v_shifted, carry);
    }
    
    store(data, v);
}

BITCAL_FORCEINLINE void shift_right_256(uint64_t* data, int count) noexcept {
    if (count == 0) return;
    if (count >= 256) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        return;
    }
    
    __m256i v = load(data);
    
    if (count >= 64) {
        const int byte_shift = (count / 64) * 8;
        v = _mm256_permute4x64_epi64(v, 0x39);
        v = _mm256_srli_si256(v, byte_shift);
        count %= 64;
    }
    
    if (count > 0) {
        __m256i v_shifted = _mm256_srli_epi64(v, count);
        __m256i carry = _mm256_permute4x64_epi64(v, 0x39);
        carry = _mm256_slli_epi64(carry, 64 - count);
        carry = _mm256_blend_epi32(carry, _mm256_setzero_si256(), 0xC0);
        v = _mm256_or_si256(v_shifted, carry);
    }
    
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
    if (count == 0) return;
    if (count >= 512) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data + 4), _mm256_setzero_si256());
        return;
    }
    
    __m256i lo = load(data);
    __m256i hi = load(data + 4);
    
    if (count >= 256) {
        hi = lo;
        lo = _mm256_setzero_si256();
        count -= 256;
    }
    
    if (count >= 64) {
        const int qword_shift = count / 64;
        
        __m256i lo_extract = _mm256_permute2x128_si256(lo, lo, 0x08);
        lo_extract = _mm256_alignr_epi8(lo, lo_extract, 16 - qword_shift * 8);
        
        __m256i hi_carry = _mm256_permute2x128_si256(lo, hi, 0x21);
        hi_carry = _mm256_alignr_epi8(hi, hi_carry, 16 - qword_shift * 8);
        
        lo = _mm256_slli_si256(lo, qword_shift * 8);
        hi = hi_carry;
        
        count %= 64;
    }
    
    if (count > 0) {
        __m256i lo_shifted = _mm256_slli_epi64(lo, count);
        __m256i hi_shifted = _mm256_slli_epi64(hi, count);
        
        __m256i lo_carry = _mm256_permute4x64_epi64(lo, 0x93);
        lo_carry = _mm256_srli_epi64(lo_carry, 64 - count);
        lo_carry = _mm256_blend_epi32(lo_carry, _mm256_setzero_si256(), 0x03);
        
        __m256i hi_carry = _mm256_permute4x64_epi64(hi, 0x93);
        hi_carry = _mm256_srli_epi64(hi_carry, 64 - count);
        hi_carry = _mm256_blend_epi32(hi_carry, _mm256_setzero_si256(), 0x03);
        
        uint64_t cross_carry = _mm256_extract_epi64(lo, 3) >> (64 - count);
        
        lo = _mm256_or_si256(lo_shifted, lo_carry);
        hi = _mm256_or_si256(hi_shifted, hi_carry);
        hi = _mm256_insert_epi64(hi, _mm256_extract_epi64(hi, 0) | cross_carry, 0);
    }
    
    store(data, lo);
    store(data + 4, hi);
}

BITCAL_FORCEINLINE void shift_right_512(uint64_t* data, int count) noexcept {
    if (count == 0) return;
    if (count >= 512) {
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data), _mm256_setzero_si256());
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(data + 4), _mm256_setzero_si256());
        return;
    }
    
    __m256i lo = load(data);
    __m256i hi = load(data + 4);
    
    if (count >= 256) {
        lo = hi;
        hi = _mm256_setzero_si256();
        count -= 256;
    }
    
    if (count >= 64) {
        const int qword_shift = count / 64;
        lo = _mm256_srli_si256(lo, qword_shift * 8);
        hi = _mm256_srli_si256(hi, qword_shift * 8);
        count %= 64;
    }
    
    if (count > 0) {
        __m256i lo_shifted = _mm256_srli_epi64(lo, count);
        __m256i hi_shifted = _mm256_srli_epi64(hi, count);
        
        __m256i lo_carry = _mm256_permute4x64_epi64(lo, 0x39);
        lo_carry = _mm256_slli_epi64(lo_carry, 64 - count);
        lo_carry = _mm256_blend_epi32(lo_carry, _mm256_setzero_si256(), 0xC0);
        
        __m256i hi_carry = _mm256_permute4x64_epi64(hi, 0x39);
        hi_carry = _mm256_slli_epi64(hi_carry, 64 - count);
        hi_carry = _mm256_blend_epi32(hi_carry, _mm256_setzero_si256(), 0xC0);
        
        uint64_t cross_carry = _mm256_extract_epi64(hi, 0) << (64 - count);
        
        lo = _mm256_or_si256(lo_shifted, lo_carry);
        hi = _mm256_or_si256(hi_shifted, hi_carry);
        lo = _mm256_insert_epi64(lo, _mm256_extract_epi64(lo, 3) | cross_carry, 3);
    }
    
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

}
}

#endif
