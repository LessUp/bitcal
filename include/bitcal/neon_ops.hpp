#pragma once

#include "config.hpp"
#include "simd_traits.hpp"

#if BITCAL_HAS_NEON

#include <arm_neon.h>

namespace bitcal {
namespace neon {

BITCAL_FORCEINLINE uint64x2_t load(const uint64_t* ptr) noexcept {
    return vld1q_u64(ptr);
}

BITCAL_FORCEINLINE void store(uint64_t* ptr, uint64x2_t value) noexcept {
    vst1q_u64(ptr, value);
}

BITCAL_FORCEINLINE uint64x2_t bit_and(uint64x2_t a, uint64x2_t b) noexcept {
    return vandq_u64(a, b);
}

BITCAL_FORCEINLINE uint64x2_t bit_or(uint64x2_t a, uint64x2_t b) noexcept {
    return vorrq_u64(a, b);
}

BITCAL_FORCEINLINE uint64x2_t bit_xor(uint64x2_t a, uint64x2_t b) noexcept {
    return veorq_u64(a, b);
}

BITCAL_FORCEINLINE uint64x2_t bit_not(uint64x2_t a) noexcept {
    return veorq_u64(a, vdupq_n_u64(~0ULL));
}

BITCAL_FORCEINLINE uint64x2_t shift_left_64(uint64x2_t a, int count) noexcept {
    return vshlq_u64(a, vdupq_n_s64(count));
}

BITCAL_FORCEINLINE uint64x2_t shift_right_64(uint64x2_t a, int count) noexcept {
    return vshlq_u64(a, vdupq_n_s64(-count));
}

BITCAL_FORCEINLINE void shift_left_128(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 128) {
        data[0] = data[1] = 0;
        return;
    }
    
    uint64x2_t v = load(data);
    
    if (count >= 64) {
        uint64_t low = vgetq_lane_u64(v, 0);
        v = vsetq_lane_u64(0, v, 0);
        v = vsetq_lane_u64(low << (count - 64), v, 1);
    } else {
        uint64_t lo = vgetq_lane_u64(v, 0);
        uint64_t hi = vgetq_lane_u64(v, 1);
        
        uint64_t new_lo = lo << count;
        uint64_t new_hi = (hi << count) | (lo >> (64 - count));
        
        v = vsetq_lane_u64(new_lo, v, 0);
        v = vsetq_lane_u64(new_hi, v, 1);
    }
    
    store(data, v);
}

BITCAL_FORCEINLINE void shift_right_128(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 128) {
        data[0] = data[1] = 0;
        return;
    }
    
    uint64x2_t v = load(data);
    
    if (count >= 64) {
        uint64_t high = vgetq_lane_u64(v, 1);
        v = vsetq_lane_u64(high >> (count - 64), v, 0);
        v = vsetq_lane_u64(0, v, 1);
    } else {
        uint64_t lo = vgetq_lane_u64(v, 0);
        uint64_t hi = vgetq_lane_u64(v, 1);
        
        uint64_t new_lo = (lo >> count) | (hi << (64 - count));
        uint64_t new_hi = hi >> count;
        
        v = vsetq_lane_u64(new_lo, v, 0);
        v = vsetq_lane_u64(new_hi, v, 1);
    }
    
    store(data, v);
}

BITCAL_FORCEINLINE void bit_and_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t va = load(a);
    uint64x2_t vb = load(b);
    store(out, bit_and(va, vb));
}

BITCAL_FORCEINLINE void bit_or_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t va = load(a);
    uint64x2_t vb = load(b);
    store(out, bit_or(va, vb));
}

BITCAL_FORCEINLINE void bit_xor_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t va = load(a);
    uint64x2_t vb = load(b);
    store(out, bit_xor(va, vb));
}

BITCAL_FORCEINLINE void shift_left_256(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 256) {
        data[0] = data[1] = data[2] = data[3] = 0;
        return;
    }

    const int word_shift = count / 64;
    const int bit_shift = count % 64;

    if (word_shift > 0) {
        for (int i = 3; i >= word_shift; --i) {
            data[i] = data[i - word_shift];
        }
        for (int i = 0; i < word_shift; ++i) {
            data[i] = 0;
        }
    }

    if (bit_shift == 0) return;

    uint64x2_t lo = load(data);
    uint64x2_t hi = load(data + 2);

    // Calculate cross-vector carry
    uint64_t carry = (bit_shift > 0) ? (vgetq_lane_u64(lo, 1) >> (64 - bit_shift)) : 0;

    // Shift each 128-bit vector
    uint64_t lo_lo = vgetq_lane_u64(lo, 0);
    uint64_t lo_hi = vgetq_lane_u64(lo, 1);
    uint64_t hi_lo = vgetq_lane_u64(hi, 0);
    uint64_t hi_hi = vgetq_lane_u64(hi, 1);

    lo = vsetq_lane_u64(lo_lo << bit_shift, lo, 0);
    lo = vsetq_lane_u64((lo_hi << bit_shift) | (lo_lo >> (64 - bit_shift)), lo, 1);
    hi = vsetq_lane_u64((hi_lo << bit_shift) | carry, hi, 0);
    hi = vsetq_lane_u64((hi_hi << bit_shift) | (hi_lo >> (64 - bit_shift)), hi, 1);

    store(data, lo);
    store(data + 2, hi);
}

BITCAL_FORCEINLINE void shift_right_256(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 256) {
        data[0] = data[1] = data[2] = data[3] = 0;
        return;
    }

    const int word_shift = count / 64;
    const int bit_shift = count % 64;

    if (word_shift > 0) {
        for (size_t i = 0; i < 4 - word_shift; ++i) {
            data[i] = data[i + word_shift];
        }
        for (size_t i = 4 - word_shift; i < 4; ++i) {
            data[i] = 0;
        }
    }

    if (bit_shift == 0) return;

    uint64x2_t lo = load(data);
    uint64x2_t hi = load(data + 2);

    // Calculate cross-vector carry
    uint64_t carry = (bit_shift > 0) ? (vgetq_lane_u64(hi, 0) << (64 - bit_shift)) : 0;

    // Shift each 128-bit vector
    uint64_t lo_lo = vgetq_lane_u64(lo, 0);
    uint64_t lo_hi = vgetq_lane_u64(lo, 1);
    uint64_t hi_lo = vgetq_lane_u64(hi, 0);
    uint64_t hi_hi = vgetq_lane_u64(hi, 1);

    lo = vsetq_lane_u64((lo_lo >> bit_shift) | (lo_hi << (64 - bit_shift)), lo, 0);
    lo = vsetq_lane_u64((lo_hi >> bit_shift) | carry, lo, 1);
    hi = vsetq_lane_u64((hi_lo >> bit_shift) | (hi_hi << (64 - bit_shift)), hi, 0);
    hi = vsetq_lane_u64(hi_hi >> bit_shift, hi, 1);

    store(data, lo);
    store(data + 2, hi);
}

BITCAL_FORCEINLINE void bit_and_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t a0 = load(a);
    uint64x2_t a1 = load(a + 2);
    uint64x2_t b0 = load(b);
    uint64x2_t b1 = load(b + 2);
    store(out, bit_and(a0, b0));
    store(out + 2, bit_and(a1, b1));
}

BITCAL_FORCEINLINE void bit_or_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t a0 = load(a);
    uint64x2_t a1 = load(a + 2);
    uint64x2_t b0 = load(b);
    uint64x2_t b1 = load(b + 2);
    store(out, bit_or(a0, b0));
    store(out + 2, bit_or(a1, b1));
}

BITCAL_FORCEINLINE void bit_xor_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t a0 = load(a);
    uint64x2_t a1 = load(a + 2);
    uint64x2_t b0 = load(b);
    uint64x2_t b1 = load(b + 2);
    store(out, bit_xor(a0, b0));
    store(out + 2, bit_xor(a1, b1));
}

BITCAL_FORCEINLINE void shift_left_512(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 512) {
        for (int i = 0; i < 8; ++i) data[i] = 0;
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

    uint64x2_t v0 = load(data);
    uint64x2_t v1 = load(data + 2);
    uint64x2_t v2 = load(data + 4);
    uint64x2_t v3 = load(data + 6);

    // Calculate cross-vector carries
    uint64_t carry0 = (bit_shift > 0) ? (vgetq_lane_u64(v0, 1) >> (64 - bit_shift)) : 0;
    uint64_t carry1 = (bit_shift > 0) ? (vgetq_lane_u64(v1, 1) >> (64 - bit_shift)) : 0;
    uint64_t carry2 = (bit_shift > 0) ? (vgetq_lane_u64(v2, 1) >> (64 - bit_shift)) : 0;

    // Shift each 128-bit vector
    uint64_t v0_lo = vgetq_lane_u64(v0, 0);
    uint64_t v0_hi = vgetq_lane_u64(v0, 1);
    uint64_t v1_lo = vgetq_lane_u64(v1, 0);
    uint64_t v1_hi = vgetq_lane_u64(v1, 1);
    uint64_t v2_lo = vgetq_lane_u64(v2, 0);
    uint64_t v2_hi = vgetq_lane_u64(v2, 1);
    uint64_t v3_lo = vgetq_lane_u64(v3, 0);
    uint64_t v3_hi = vgetq_lane_u64(v3, 1);

    v0 = vsetq_lane_u64(v0_lo << bit_shift, v0, 0);
    v0 = vsetq_lane_u64((v0_hi << bit_shift) | (v0_lo >> (64 - bit_shift)), v0, 1);
    v1 = vsetq_lane_u64((v1_lo << bit_shift) | carry0, v1, 0);
    v1 = vsetq_lane_u64((v1_hi << bit_shift) | (v1_lo >> (64 - bit_shift)), v1, 1);
    v2 = vsetq_lane_u64((v2_lo << bit_shift) | carry1, v2, 0);
    v2 = vsetq_lane_u64((v2_hi << bit_shift) | (v2_lo >> (64 - bit_shift)), v2, 1);
    v3 = vsetq_lane_u64((v3_lo << bit_shift) | carry2, v3, 0);
    v3 = vsetq_lane_u64((v3_hi << bit_shift) | (v3_lo >> (64 - bit_shift)), v3, 1);

    store(data, v0);
    store(data + 2, v1);
    store(data + 4, v2);
    store(data + 6, v3);
}

BITCAL_FORCEINLINE void shift_right_512(uint64_t* data, int count) noexcept {
    if (count <= 0) return;
    if (count >= 512) {
        for (int i = 0; i < 8; ++i) data[i] = 0;
        return;
    }

    const int word_shift = count / 64;
    const int bit_shift = count % 64;

    if (word_shift > 0) {
        for (size_t i = 0; i < 8 - word_shift; ++i) {
            data[i] = data[i + word_shift];
        }
        for (size_t i = 8 - word_shift; i < 8; ++i) {
            data[i] = 0;
        }
    }

    if (bit_shift == 0) return;

    uint64x2_t v0 = load(data);
    uint64x2_t v1 = load(data + 2);
    uint64x2_t v2 = load(data + 4);
    uint64x2_t v3 = load(data + 6);

    // Calculate cross-vector carries
    uint64_t carry0 = (bit_shift > 0) ? (vgetq_lane_u64(v1, 0) << (64 - bit_shift)) : 0;
    uint64_t carry1 = (bit_shift > 0) ? (vgetq_lane_u64(v2, 0) << (64 - bit_shift)) : 0;
    uint64_t carry2 = (bit_shift > 0) ? (vgetq_lane_u64(v3, 0) << (64 - bit_shift)) : 0;

    // Shift each 128-bit vector
    uint64_t v0_lo = vgetq_lane_u64(v0, 0);
    uint64_t v0_hi = vgetq_lane_u64(v0, 1);
    uint64_t v1_lo = vgetq_lane_u64(v1, 0);
    uint64_t v1_hi = vgetq_lane_u64(v1, 1);
    uint64_t v2_lo = vgetq_lane_u64(v2, 0);
    uint64_t v2_hi = vgetq_lane_u64(v2, 1);
    uint64_t v3_lo = vgetq_lane_u64(v3, 0);
    uint64_t v3_hi = vgetq_lane_u64(v3, 1);

    v0 = vsetq_lane_u64((v0_lo >> bit_shift) | (v0_hi << (64 - bit_shift)), v0, 0);
    v0 = vsetq_lane_u64((v0_hi >> bit_shift) | carry0, v0, 1);
    v1 = vsetq_lane_u64((v1_lo >> bit_shift) | (v1_hi << (64 - bit_shift)), v1, 0);
    v1 = vsetq_lane_u64((v1_hi >> bit_shift) | carry1, v1, 1);
    v2 = vsetq_lane_u64((v2_lo >> bit_shift) | (v2_hi << (64 - bit_shift)), v2, 0);
    v2 = vsetq_lane_u64((v2_hi >> bit_shift) | carry2, v2, 1);
    v3 = vsetq_lane_u64((v3_lo >> bit_shift) | (v3_hi << (64 - bit_shift)), v3, 0);
    v3 = vsetq_lane_u64(v3_hi >> bit_shift, v3, 1);

    store(data, v0);
    store(data + 2, v1);
    store(data + 4, v2);
    store(data + 6, v3);
}

BITCAL_FORCEINLINE void bit_and_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (int i = 0; i < 4; ++i) {
        uint64x2_t va = load(a + i * 2);
        uint64x2_t vb = load(b + i * 2);
        store(out + i * 2, bit_and(va, vb));
    }
}

BITCAL_FORCEINLINE void bit_or_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (int i = 0; i < 4; ++i) {
        uint64x2_t va = load(a + i * 2);
        uint64x2_t vb = load(b + i * 2);
        store(out + i * 2, bit_or(va, vb));
    }
}

BITCAL_FORCEINLINE void bit_xor_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (int i = 0; i < 4; ++i) {
        uint64x2_t va = load(a + i * 2);
        uint64x2_t vb = load(b + i * 2);
        store(out + i * 2, bit_xor(va, vb));
    }
}

BITCAL_FORCEINLINE void bit_not_128(const uint64_t* in, uint64_t* out) noexcept {
    uint64x2_t v = load(in);
    store(out, bit_not(v));
}

BITCAL_FORCEINLINE void bit_not_256(const uint64_t* in, uint64_t* out) noexcept {
    uint64x2_t v0 = load(in);
    uint64x2_t v1 = load(in + 2);
    store(out, bit_not(v0));
    store(out + 2, bit_not(v1));
}

BITCAL_FORCEINLINE void bit_not_512(const uint64_t* in, uint64_t* out) noexcept {
    for (int i = 0; i < 4; ++i) {
        uint64x2_t v = load(in + i * 2);
        store(out + i * 2, bit_not(v));
    }
}

BITCAL_FORCEINLINE void bit_andnot_128(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t va = load(a);
    uint64x2_t vb = load(b);
    store(out, vbicq_u64(va, vb));
}

BITCAL_FORCEINLINE void bit_andnot_256(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    uint64x2_t a0 = load(a);
    uint64x2_t a1 = load(a + 2);
    uint64x2_t b0 = load(b);
    uint64x2_t b1 = load(b + 2);
    store(out, vbicq_u64(a0, b0));
    store(out + 2, vbicq_u64(a1, b1));
}

BITCAL_FORCEINLINE void bit_andnot_512(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (int i = 0; i < 4; ++i) {
        uint64x2_t va = load(a + i * 2);
        uint64x2_t vb = load(b + i * 2);
        store(out + i * 2, vbicq_u64(va, vb));
    }
}

BITCAL_FORCEINLINE bool is_zero_128(const uint64_t* data) noexcept {
    uint64x2_t v = load(data);
    uint64_t lo = vgetq_lane_u64(v, 0);
    uint64_t hi = vgetq_lane_u64(v, 1);
    return (lo | hi) == 0;
}

BITCAL_FORCEINLINE bool is_zero_256(const uint64_t* data) noexcept {
    uint64x2_t v0 = load(data);
    uint64x2_t v1 = load(data + 2);
    // OR the two vectors together, then check if result is zero
    uint64x2_t combined = vorrq_u64(v0, v1);
    uint64_t lo = vgetq_lane_u64(combined, 0);
    uint64_t hi = vgetq_lane_u64(combined, 1);
    return (lo | hi) == 0;
}

BITCAL_FORCEINLINE bool is_zero_512(const uint64_t* data) noexcept {
    uint64x2_t combined = vdupq_n_u64(0);
    for (int i = 0; i < 4; ++i) {
        combined = vorrq_u64(combined, load(data + i * 2));
    }
    uint64_t lo = vgetq_lane_u64(combined, 0);
    uint64_t hi = vgetq_lane_u64(combined, 1);
    return (lo | hi) == 0;
}

}
}

#endif
