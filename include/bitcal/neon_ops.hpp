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
    return vmvnq_u64(a);
}

BITCAL_FORCEINLINE uint64x2_t shift_left_64(uint64x2_t a, int count) noexcept {
    return vshlq_n_u64(a, count);
}

BITCAL_FORCEINLINE uint64x2_t shift_right_64(uint64x2_t a, int count) noexcept {
    return vshrq_n_u64(a, count);
}

BITCAL_FORCEINLINE void shift_left_128(uint64_t* data, int count) noexcept {
    if (count == 0) return;
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
    if (count == 0) return;
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
    if (count == 0) return;
    if (count >= 256) {
        data[0] = data[1] = data[2] = data[3] = 0;
        return;
    }
    
    uint64x2_t lo = load(data);
    uint64x2_t hi = load(data + 2);
    
    if (count >= 128) {
        hi = lo;
        lo = vdupq_n_u64(0);
        count -= 128;
    }
    
    if (count >= 64) {
        uint64_t carry = vgetq_lane_u64(lo, 1);
        lo = vsetq_lane_u64(vgetq_lane_u64(lo, 0), lo, 1);
        lo = vsetq_lane_u64(0, lo, 0);
        
        uint64_t hi_lo = vgetq_lane_u64(hi, 0);
        uint64_t hi_hi = vgetq_lane_u64(hi, 1);
        hi = vsetq_lane_u64(hi_lo, hi, 1);
        hi = vsetq_lane_u64(carry, hi, 0);
        
        if (count > 64) {
            count -= 64;
            lo = vshlq_n_u64(lo, count);
            
            uint64_t hi_lo_val = vgetq_lane_u64(hi, 0);
            uint64_t hi_hi_val = vgetq_lane_u64(hi, 1);
            uint64_t new_hi_lo = (hi_lo_val << count);
            uint64_t new_hi_hi = (hi_hi_val << count) | (hi_lo_val >> (64 - count));
            hi = vsetq_lane_u64(new_hi_lo, hi, 0);
            hi = vsetq_lane_u64(new_hi_hi, hi, 1);
        }
    } else if (count > 0) {
        uint64_t lo_lo = vgetq_lane_u64(lo, 0);
        uint64_t lo_hi = vgetq_lane_u64(lo, 1);
        uint64_t hi_lo = vgetq_lane_u64(hi, 0);
        uint64_t hi_hi = vgetq_lane_u64(hi, 1);
        
        lo = vsetq_lane_u64(lo_lo << count, lo, 0);
        lo = vsetq_lane_u64((lo_hi << count) | (lo_lo >> (64 - count)), lo, 1);
        hi = vsetq_lane_u64((hi_lo << count) | (lo_hi >> (64 - count)), hi, 0);
        hi = vsetq_lane_u64((hi_hi << count) | (hi_lo >> (64 - count)), hi, 1);
    }
    
    store(data, lo);
    store(data + 2, hi);
}

BITCAL_FORCEINLINE void shift_right_256(uint64_t* data, int count) noexcept {
    if (count == 0) return;
    if (count >= 256) {
        data[0] = data[1] = data[2] = data[3] = 0;
        return;
    }
    
    uint64x2_t lo = load(data);
    uint64x2_t hi = load(data + 2);
    
    if (count >= 128) {
        lo = hi;
        hi = vdupq_n_u64(0);
        count -= 128;
    }
    
    if (count >= 64) {
        uint64_t carry = vgetq_lane_u64(hi, 0);
        hi = vsetq_lane_u64(vgetq_lane_u64(hi, 1), hi, 0);
        hi = vsetq_lane_u64(0, hi, 1);
        
        lo = vsetq_lane_u64(carry, lo, 1);
        lo = vsetq_lane_u64(vgetq_lane_u64(lo, 1), lo, 0);
        
        if (count > 64) {
            count -= 64;
            hi = vshrq_n_u64(hi, count);
            
            uint64_t lo_lo_val = vgetq_lane_u64(lo, 0);
            uint64_t lo_hi_val = vgetq_lane_u64(lo, 1);
            lo = vsetq_lane_u64((lo_lo_val >> count) | (lo_hi_val << (64 - count)), lo, 0);
            lo = vsetq_lane_u64(lo_hi_val >> count, lo, 1);
        }
    } else if (count > 0) {
        uint64_t lo_lo = vgetq_lane_u64(lo, 0);
        uint64_t lo_hi = vgetq_lane_u64(lo, 1);
        uint64_t hi_lo = vgetq_lane_u64(hi, 0);
        uint64_t hi_hi = vgetq_lane_u64(hi, 1);
        
        lo = vsetq_lane_u64((lo_lo >> count) | (lo_hi << (64 - count)), lo, 0);
        lo = vsetq_lane_u64((lo_hi >> count) | (hi_lo << (64 - count)), lo, 1);
        hi = vsetq_lane_u64((hi_lo >> count) | (hi_hi << (64 - count)), hi, 0);
        hi = vsetq_lane_u64(hi_hi >> count, hi, 1);
    }
    
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
    if (count == 0) return;
    if (count >= 512) {
        for (int i = 0; i < 8; ++i) data[i] = 0;
        return;
    }
    
    uint64x2_t v0 = load(data);
    uint64x2_t v1 = load(data + 2);
    uint64x2_t v2 = load(data + 4);
    uint64x2_t v3 = load(data + 6);
    
    if (count >= 256) {
        v3 = v1;
        v2 = v0;
        v1 = vdupq_n_u64(0);
        v0 = vdupq_n_u64(0);
        count -= 256;
    }
    
    if (count >= 128) {
        v3 = v2;
        v2 = v1;
        v1 = v0;
        v0 = vdupq_n_u64(0);
        count -= 128;
    }
    
    if (count > 0) {
        uint64_t carry01 = (count < 64) ? (vgetq_lane_u64(v0, 1) >> (64 - count)) : 0;
        uint64_t carry12 = (count < 64) ? (vgetq_lane_u64(v1, 1) >> (64 - count)) : 0;
        uint64_t carry23 = (count < 64) ? (vgetq_lane_u64(v2, 1) >> (64 - count)) : 0;
        
        if (count >= 64) {
            v3 = vsetq_lane_u64(vgetq_lane_u64(v2, 1), v3, 0);
            v3 = vsetq_lane_u64((vgetq_lane_u64(v3, 0) << (count - 64)), v3, 1);
            v3 = vsetq_lane_u64(vgetq_lane_u64(v2, 1) << (count - 64), v3, 0);
            
            v2 = vsetq_lane_u64(vgetq_lane_u64(v1, 1), v2, 0);
            v2 = vsetq_lane_u64((vgetq_lane_u64(v2, 0) << (count - 64)), v2, 1);
            v2 = vsetq_lane_u64(vgetq_lane_u64(v1, 1) << (count - 64), v2, 0);
            
            v1 = vsetq_lane_u64(vgetq_lane_u64(v0, 1), v1, 0);
            v1 = vsetq_lane_u64((vgetq_lane_u64(v1, 0) << (count - 64)), v1, 1);
            v1 = vsetq_lane_u64(vgetq_lane_u64(v0, 1) << (count - 64), v1, 0);
            
            v0 = vdupq_n_u64(0);
        } else {
            uint64_t v0_lo = vgetq_lane_u64(v0, 0);
            uint64_t v0_hi = vgetq_lane_u64(v0, 1);
            uint64_t v1_lo = vgetq_lane_u64(v1, 0);
            uint64_t v1_hi = vgetq_lane_u64(v1, 1);
            uint64_t v2_lo = vgetq_lane_u64(v2, 0);
            uint64_t v2_hi = vgetq_lane_u64(v2, 1);
            uint64_t v3_lo = vgetq_lane_u64(v3, 0);
            uint64_t v3_hi = vgetq_lane_u64(v3, 1);
            
            v0 = vsetq_lane_u64(v0_lo << count, v0, 0);
            v0 = vsetq_lane_u64((v0_hi << count) | (v0_lo >> (64 - count)), v0, 1);
            v1 = vsetq_lane_u64((v1_lo << count) | carry01, v1, 0);
            v1 = vsetq_lane_u64((v1_hi << count) | (v1_lo >> (64 - count)), v1, 1);
            v2 = vsetq_lane_u64((v2_lo << count) | carry12, v2, 0);
            v2 = vsetq_lane_u64((v2_hi << count) | (v2_lo >> (64 - count)), v2, 1);
            v3 = vsetq_lane_u64((v3_lo << count) | carry23, v3, 0);
            v3 = vsetq_lane_u64((v3_hi << count) | (v3_lo >> (64 - count)), v3, 1);
        }
    }
    
    store(data, v0);
    store(data + 2, v1);
    store(data + 4, v2);
    store(data + 6, v3);
}

BITCAL_FORCEINLINE void shift_right_512(uint64_t* data, int count) noexcept {
    if (count == 0) return;
    if (count >= 512) {
        for (int i = 0; i < 8; ++i) data[i] = 0;
        return;
    }
    
    uint64x2_t v0 = load(data);
    uint64x2_t v1 = load(data + 2);
    uint64x2_t v2 = load(data + 4);
    uint64x2_t v3 = load(data + 6);
    
    if (count >= 256) {
        v0 = v2;
        v1 = v3;
        v2 = vdupq_n_u64(0);
        v3 = vdupq_n_u64(0);
        count -= 256;
    }
    
    if (count >= 128) {
        v0 = v1;
        v1 = v2;
        v2 = v3;
        v3 = vdupq_n_u64(0);
        count -= 128;
    }
    
    if (count > 0) {
        if (count >= 64) {
            count -= 64;
            v0 = vsetq_lane_u64((vgetq_lane_u64(v0, 1) >> count) | (vgetq_lane_u64(v1, 0) << (64 - count)), v0, 0);
            v0 = vsetq_lane_u64(vgetq_lane_u64(v1, 0) >> count, v0, 1);
            v1 = vsetq_lane_u64((vgetq_lane_u64(v1, 1) >> count) | (vgetq_lane_u64(v2, 0) << (64 - count)), v1, 0);
            v1 = vsetq_lane_u64(vgetq_lane_u64(v2, 0) >> count, v1, 1);
            v2 = vsetq_lane_u64((vgetq_lane_u64(v2, 1) >> count) | (vgetq_lane_u64(v3, 0) << (64 - count)), v2, 0);
            v2 = vsetq_lane_u64(vgetq_lane_u64(v3, 0) >> count, v2, 1);
            v3 = vsetq_lane_u64(vgetq_lane_u64(v3, 1) >> count, v3, 0);
            v3 = vsetq_lane_u64(0, v3, 1);
        } else {
            uint64_t v0_lo = vgetq_lane_u64(v0, 0);
            uint64_t v0_hi = vgetq_lane_u64(v0, 1);
            uint64_t v1_lo = vgetq_lane_u64(v1, 0);
            uint64_t v1_hi = vgetq_lane_u64(v1, 1);
            uint64_t v2_lo = vgetq_lane_u64(v2, 0);
            uint64_t v2_hi = vgetq_lane_u64(v2, 1);
            uint64_t v3_lo = vgetq_lane_u64(v3, 0);
            uint64_t v3_hi = vgetq_lane_u64(v3, 1);
            
            v0 = vsetq_lane_u64((v0_lo >> count) | (v0_hi << (64 - count)), v0, 0);
            v0 = vsetq_lane_u64((v0_hi >> count) | (v1_lo << (64 - count)), v0, 1);
            v1 = vsetq_lane_u64((v1_lo >> count) | (v1_hi << (64 - count)), v1, 0);
            v1 = vsetq_lane_u64((v1_hi >> count) | (v2_lo << (64 - count)), v1, 1);
            v2 = vsetq_lane_u64((v2_lo >> count) | (v2_hi << (64 - count)), v2, 0);
            v2 = vsetq_lane_u64((v2_hi >> count) | (v3_lo << (64 - count)), v2, 1);
            v3 = vsetq_lane_u64((v3_lo >> count) | (v3_hi << (64 - count)), v3, 0);
            v3 = vsetq_lane_u64(v3_hi >> count, v3, 1);
        }
    }
    
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

}
}

#endif
