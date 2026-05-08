#pragma once

#include "config.hpp"
#include <cstring>

/**
 * @file scalar_ops.hpp
 * @brief Reference implementation for all bit operations
 *
 * ============================================================================
 * REFERENCE IMPLEMENTATION - 权威参考实现
 * ============================================================================
 *
 * This file serves as the authoritative reference for all bit operations.
 * All SIMD backends (SSE2/AVX2/AVX-512/NEON) MUST produce results identical
 * to this implementation.
 *
 * ============================================================================
 * SHIFT OPERATIONS STRATEGY - 移位操作策略
 * ============================================================================
 *
 * Shift operations use a two-phase strategy:
 *
 * Phase 1: Word-level shift (count >= 64)
 *   - Move whole 64-bit words by (count / 64) positions
 *   - Zero-fill the vacated positions
 *   - Early return if no bit-level shift needed (count % 64 == 0)
 *
 * Phase 2: Bit-level shift (count < 64 or count % 64 != 0)
 *   - Shift each word by (count % 64) bits
 *   - Propagate carry from adjacent word
 *   - Left shift:  carry = data[i-1] >> (64 - bit_shift)
 *   - Right shift: carry = data[i+1] << (64 - bit_shift)
 *   - Formula: result[i] = (data[i] << bit_shift) | carry
 *
 * Key invariants:
 *   - shift_left(count >= Bits)  => all zeros
 *   - shift_right(count >= Bits) => all zeros
 *   - shift_left(0)  => no change
 *   - shift_right(0) => no change
 *   - bit_shift ∈ [1, 63] for Phase 2, ensuring safe shift operations
 *
 * ============================================================================
 * POPCOUNT STRATEGY - 位计数策略
 * ============================================================================
 *
 * Uses compiler intrinsics when available:
 *   - GCC/Clang: __builtin_popcountll()
 *   - MSVC: __popcnt64()
 *   - Fallback: parallel bit counting algorithm
 *
 * ============================================================================
 * CLZ/CTZ STRATEGY - 前导零/尾部零计数策略
 * ============================================================================
 *
 * Uses compiler intrinsics when available:
 *   - GCC/Clang: __builtin_clzll() / __builtin_ctzll()
 *   - MSVC: _BitScanReverse64() / _BitScanForward64()
 *   - Fallback: bit-by-bit scanning
 *
 * Special case: CLZ/CTZ on zero returns Bits (not UB)
 *
 * ============================================================================
 */

namespace bitcal {
namespace scalar {

BITCAL_FORCEINLINE uint64_t shift_left(uint64_t value, int count) noexcept {
    if (count <= 0) return value;
    if (count >= 64) return 0;
    return value << count;
}

BITCAL_FORCEINLINE uint64_t shift_right(uint64_t value, int count) noexcept {
    if (count <= 0) return value;
    if (count >= 64) return 0;
    return value >> count;
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
#elif defined(_MSC_VER) && defined(_M_X64)
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
#elif defined(_MSC_VER) && defined(_M_X64)
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

    if (count <= 0) return;
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

        // Early return when no bit-level shift needed
        // This also ensures bit_shift ∈ [1, 63] for the code below,
        // preventing potential shift-by-64 UB in `data[i] >> (64 - count)`
        if (bit_shift == 0) return;
        count = bit_shift;
    }

    // At this point: count ∈ [1, 63]
    // Therefore (64 - count) ∈ [1, 63], safe for shift operations
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

    if (count <= 0) return;
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

        // Early return when no bit-level shift needed
        // This also ensures bit_shift ∈ [1, 63] for the code below,
        // preventing potential shift-by-64 UB in `data[i] << (64 - count)`
        if (bit_shift == 0) return;
        count = bit_shift;
    }

    // At this point: count ∈ [1, 63]
    // Therefore (64 - count) ∈ [1, 63], safe for shift operations
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

// ============================================================================
// Equality comparison
// ============================================================================

template<size_t N>
BITCAL_FORCEINLINE bool equals_array(const uint64_t* a, const uint64_t* b) noexcept {
    for (size_t i = 0; i < N; ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// ============================================================================
// Zero and all checks
// ============================================================================

template<size_t N>
BITCAL_FORCEINLINE bool is_zero_array(const uint64_t* data) noexcept {
    for (size_t i = 0; i < N; ++i) {
        if (data[i] != 0) return false;
    }
    return true;
}

template<size_t N>
BITCAL_FORCEINLINE bool all_array(const uint64_t* data) noexcept {
    for (size_t i = 0; i < N; ++i) {
        if (data[i] != ~0ULL) return false;
    }
    return true;
}



}
}
