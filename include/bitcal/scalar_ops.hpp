#pragma once

#include "config.hpp"
#include <cstring>

namespace bitcal {
namespace scalar {

BITCAL_FORCEINLINE uint64_t shift_left(uint64_t value, int count) noexcept {
    return (count < 64) ? (value << count) : 0;
}

BITCAL_FORCEINLINE uint64_t shift_right(uint64_t value, int count) noexcept {
    return (count < 64) ? (value >> count) : 0;
}

BITCAL_FORCEINLINE uint64_t bit_and(uint64_t a, uint64_t b) noexcept {
    return a & b;
}

BITCAL_FORCEINLINE uint64_t bit_or(uint64_t a, uint64_t b) noexcept {
    return a | b;
}

BITCAL_FORCEINLINE uint64_t bit_xor(uint64_t a, uint64_t b) noexcept {
    return a ^ b;
}

BITCAL_FORCEINLINE uint64_t bit_not(uint64_t a) noexcept {
    return ~a;
}

BITCAL_FORCEINLINE uint64_t bit_andnot(uint64_t a, uint64_t b) noexcept {
    return a & ~b;
}

BITCAL_FORCEINLINE uint64_t popcount(uint64_t x) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(x);
#elif defined(_MSC_VER) && defined(_M_X64)
    return __popcnt64(x);
#else
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (x * 0x0101010101010101ULL) >> 56;
#endif
}

BITCAL_FORCEINLINE int count_leading_zeros(uint64_t x) noexcept {
    if (x == 0) return 64;
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_clzll(x);
#elif defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse64(&index, x);
    return 63 - static_cast<int>(index);
#else
    int count = 0;
    for (int i = 63; i >= 0; --i) {
        if (x & (1ULL << i)) break;
        ++count;
    }
    return count;
#endif
}

BITCAL_FORCEINLINE int count_trailing_zeros(uint64_t x) noexcept {
    if (x == 0) return 64;
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctzll(x);
#elif defined(_MSC_VER)
    unsigned long index;
    _BitScanForward64(&index, x);
    return static_cast<int>(index);
#else
    int count = 0;
    for (int i = 0; i < 64; ++i) {
        if (x & (1ULL << i)) break;
        ++count;
    }
    return count;
#endif
}

BITCAL_FORCEINLINE uint64_t byte_swap(uint64_t x) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(x);
#elif defined(_MSC_VER)
    return _byteswap_uint64(x);
#else
    return ((x & 0x00000000000000FFULL) << 56) |
           ((x & 0x000000000000FF00ULL) << 40) |
           ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x00000000FF000000ULL) << 8)  |
           ((x & 0x000000FF00000000ULL) >> 8)  |
           ((x & 0x0000FF0000000000ULL) >> 24) |
           ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0xFF00000000000000ULL) >> 56);
#endif
}

BITCAL_FORCEINLINE uint64_t reverse_bits(uint64_t x) noexcept {
    x = ((x & 0x5555555555555555ULL) << 1)  | ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
    x = ((x & 0x3333333333333333ULL) << 2)  | ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
    x = ((x & 0x0F0F0F0F0F0F0F0FULL) << 4)  | ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
    return byte_swap(x);
}

template<size_t N>
BITCAL_FORCEINLINE void shift_left_array(uint64_t* data, int count) noexcept {
    static_assert(N > 0, "Array size must be positive");
    
    if (count == 0) return;
    if (count >= 64) {
        const int word_shift = count / 64;
        const int bit_shift = count % 64;
        
        if (word_shift >= static_cast<int>(N)) {
            std::memset(data, 0, N * sizeof(uint64_t));
            return;
        }
        
        for (int i = N - 1; i >= word_shift; --i) {
            data[i] = data[i - word_shift];
        }
        for (int i = 0; i < word_shift; ++i) {
            data[i] = 0;
        }
        
        if (bit_shift == 0) return;
        count = bit_shift;
    }
    
    uint64_t carry = 0;
    for (size_t i = 0; i < N; ++i) {
        uint64_t next_carry = data[i] >> (64 - count);
        data[i] = (data[i] << count) | carry;
        carry = next_carry;
    }
}

template<size_t N>
BITCAL_FORCEINLINE void shift_right_array(uint64_t* data, int count) noexcept {
    static_assert(N > 0, "Array size must be positive");
    
    if (count == 0) return;
    if (count >= 64) {
        const int word_shift = count / 64;
        const int bit_shift = count % 64;
        
        if (word_shift >= static_cast<int>(N)) {
            std::memset(data, 0, N * sizeof(uint64_t));
            return;
        }
        
        for (size_t i = 0; i < N - word_shift; ++i) {
            data[i] = data[i + word_shift];
        }
        for (size_t i = N - word_shift; i < N; ++i) {
            data[i] = 0;
        }
        
        if (bit_shift == 0) return;
        count = bit_shift;
    }
    
    uint64_t carry = 0;
    for (int i = N - 1; i >= 0; --i) {
        uint64_t next_carry = data[i] << (64 - count);
        data[i] = (data[i] >> count) | carry;
        carry = next_carry;
    }
}

template<size_t N>
BITCAL_FORCEINLINE void bit_and_array(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (size_t i = 0; i < N; ++i) {
        out[i] = a[i] & b[i];
    }
}

template<size_t N>
BITCAL_FORCEINLINE void bit_or_array(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (size_t i = 0; i < N; ++i) {
        out[i] = a[i] | b[i];
    }
}

template<size_t N>
BITCAL_FORCEINLINE void bit_xor_array(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (size_t i = 0; i < N; ++i) {
        out[i] = a[i] ^ b[i];
    }
}

template<size_t N>
BITCAL_FORCEINLINE void bit_andnot_array(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
    for (size_t i = 0; i < N; ++i) {
        out[i] = a[i] & ~b[i];
    }
}

template<size_t N>
BITCAL_FORCEINLINE uint64_t popcount_array(const uint64_t* data) noexcept {
    uint64_t count = 0;
    for (size_t i = 0; i < N; ++i) {
        count += popcount(data[i]);
    }
    return count;
}

template<size_t N>
BITCAL_FORCEINLINE void reverse_bits_array(const uint64_t* in, uint64_t* out) noexcept {
    for (size_t i = 0; i < N; ++i) {
        out[N - 1 - i] = reverse_bits(in[i]);
    }
}

}
}
