#pragma once

#include "config.hpp"
#include "scalar_ops.hpp"

#if BITCAL_HAS_SSE2
#include "sse_ops.hpp"
#endif

#if BITCAL_HAS_AVX2
#include "avx_ops.hpp"
#endif

#if BITCAL_HAS_AVX512
#include "avx512_ops.hpp"
#endif

#if BITCAL_HAS_NEON
#include "neon_ops.hpp"
#endif

/**
 * @file backend_ops.hpp
 * @brief Unified SIMD backend interface
 *
 * ============================================================================
 * 统一后端接口
 * ============================================================================
 *
 * This file provides a unified interface for all SIMD backends.
 * Instead of calling backend-specific functions like `avx::bit_and_256()`,
 * use the unified template: `backend::ops<Bits, Backend>::bit_and()`.
 *
 * Benefits:
 * - Single point of dispatch in bitarray class
 * - No more repetitive if constexpr chains
 * - Easier to add new backends
 * - Better compile-time optimization
 *
 * ============================================================================
 */

namespace bitcal {
namespace backend {

// ============================================================================
// Forward declaration of the unified ops template
// ============================================================================

template<size_t Bits, simd_backend Backend>
struct ops;

// ============================================================================
// Scalar backend - 通用实现（适用于所有位宽）
// ============================================================================

template<size_t Bits>
struct ops<Bits, simd_backend::scalar> {
    static constexpr size_t N = Bits / 64;

    // ===== 位运算 =====

    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        for (size_t i = 0; i < N; ++i) {
            out[i] = a[i] & b[i];
        }
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        for (size_t i = 0; i < N; ++i) {
            out[i] = a[i] | b[i];
        }
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        for (size_t i = 0; i < N; ++i) {
            out[i] = a[i] ^ b[i];
        }
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        for (size_t i = 0; i < N; ++i) {
            out[i] = ~in[i];
        }
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        for (size_t i = 0; i < N; ++i) {
            out[i] = a[i] & ~b[i];
        }
    }

    // ===== 移位 =====

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        scalar::shift_left_array<N>(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        scalar::shift_right_array<N>(data, count);
    }

    // ===== 查询 =====

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return scalar::is_zero_array<N>(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return scalar::all_array<N>(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return scalar::equals_array<N>(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return scalar::popcount_array<N>(data);
    }
};

// ============================================================================
// SSE2 backend - 128-bit and 256-bit operations
// ============================================================================

#if BITCAL_HAS_SSE2

template<>
struct ops<128, simd_backend::sse2> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_and_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_or_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_xor_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        sse::bit_not_128(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_andnot_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        sse::shift_left_128(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        sse::shift_right_128(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return sse::is_zero_128(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return sse::all_128(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return sse::equals_128(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return scalar::popcount_array<2>(data);  // SSE2 没有 popcount 指令，使用 scalar
    }
};

template<>
struct ops<256, simd_backend::sse2> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_and_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_or_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_xor_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        sse::bit_not_256(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        sse::bit_andnot_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        sse::shift_left_256(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        sse::shift_right_256(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return sse::is_zero_256(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return sse::all_256(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return sse::equals_256(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return scalar::popcount_array<4>(data);
    }
};

#endif // BITCAL_HAS_SSE2

// ============================================================================
// AVX2 backend - 256-bit and 512-bit operations
// ============================================================================

#if BITCAL_HAS_AVX2

template<>
struct ops<256, simd_backend::avx2> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_and_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_or_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_xor_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        avx::bit_not_256(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_andnot_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        avx::shift_left_256(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        avx::shift_right_256(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return avx::is_zero_256(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return avx::all_256(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return avx::equals_256(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return avx::popcount_256(data);
    }
};

template<>
struct ops<512, simd_backend::avx2> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_and_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_or_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_xor_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        avx::bit_not_512(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx::bit_andnot_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        avx::shift_left_512(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        avx::shift_right_512(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return avx::is_zero_512(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return avx::all_512(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return avx::equals_512(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return avx::popcount_512(data);
    }
};

#endif // BITCAL_HAS_AVX2

// ============================================================================
// AVX-512 backend - 256-bit and 512-bit operations
// ============================================================================

#if BITCAL_HAS_AVX512

template<>
struct ops<256, simd_backend::avx512> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_and_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_or_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_xor_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        avx512::bit_not_256(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_andnot_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        avx512::shift_left_256(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        avx512::shift_right_256(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return avx512::is_zero_256(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return avx512::all_256(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return avx512::equals_256(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return avx512::popcount_256(data);
    }
};

template<>
struct ops<512, simd_backend::avx512> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_and_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_or_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_xor_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        avx512::bit_not_512(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        avx512::bit_andnot_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        avx512::shift_left_512(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        avx512::shift_right_512(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return avx512::is_zero_512(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return avx512::all_512(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return avx512::equals_512(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return avx512::popcount_512(data);
    }
};

#endif // BITCAL_HAS_AVX512

// ============================================================================
// NEON backend - 128-bit, 256-bit, and 512-bit operations
// ============================================================================

#if BITCAL_HAS_NEON

template<>
struct ops<128, simd_backend::neon> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_and_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_or_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_xor_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        neon::bit_not_128(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_andnot_128(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        neon::shift_left_128(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        neon::shift_right_128(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return neon::is_zero_128(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return neon::all_128(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return neon::equals_128(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return scalar::popcount_array<2>(data);  // NEON 没有 popcount 指令，使用 scalar
    }
};

template<>
struct ops<256, simd_backend::neon> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_and_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_or_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_xor_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        neon::bit_not_256(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_andnot_256(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        neon::shift_left_256(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        neon::shift_right_256(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return neon::is_zero_256(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return neon::all_256(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return neon::equals_256(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return scalar::popcount_array<4>(data);
    }
};

template<>
struct ops<512, simd_backend::neon> {
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_and_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_or_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_xor_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept {
        neon::bit_not_512(in, out);
    }

    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept {
        neon::bit_andnot_512(a, b, out);
    }

    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept {
        neon::shift_left_512(data, count);
    }

    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept {
        neon::shift_right_512(data, count);
    }

    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept {
        return neon::is_zero_512(data);
    }

    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept {
        return neon::all_512(data);
    }

    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept {
        return neon::equals_512(a, b);
    }

    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept {
        return scalar::popcount_array<8>(data);
    }
};

#endif // BITCAL_HAS_NEON

// ============================================================================
// Fallback for unsupported Bit/Backend combinations
// ============================================================================
// For bit widths that don't have SIMD-optimized versions (e.g., 1024, 2048),
// we fall back to the scalar implementation. This is done by inheriting from
// the scalar ops with the same bit width.

template<size_t Bits, simd_backend Backend>
struct ops : ops<Bits, simd_backend::scalar> {
    // Inherits all operations from scalar backend
    // This provides a working implementation for any bit width
};

// Explicit specializations take precedence over the generic template above
// So we need to make sure our explicit specializations are still valid

} // namespace backend
} // namespace bitcal
