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

    static BITCAL_FORCEINLINE int count_leading_zeros(const uint64_t* data) noexcept {
        return scalar::count_leading_zeros_array<N>(data);
    }

    static BITCAL_FORCEINLINE int count_trailing_zeros(const uint64_t* data) noexcept {
        return scalar::count_trailing_zeros_array<N>(data);
    }
};

// ============================================================================
// X-Macro helpers for backend specializations
// ============================================================================

// Helper macro to generate bit operations for a specific backend and bits
#define BITCAL_GEN_BIT_OPS(NAMESPACE, BITS) \
    static BITCAL_FORCEINLINE void bit_and(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept { \
        NAMESPACE::bit_and_##BITS(a, b, out); \
    } \
    static BITCAL_FORCEINLINE void bit_or(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept { \
        NAMESPACE::bit_or_##BITS(a, b, out); \
    } \
    static BITCAL_FORCEINLINE void bit_xor(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept { \
        NAMESPACE::bit_xor_##BITS(a, b, out); \
    } \
    static BITCAL_FORCEINLINE void bit_not(const uint64_t* in, uint64_t* out) noexcept { \
        NAMESPACE::bit_not_##BITS(in, out); \
    } \
    static BITCAL_FORCEINLINE void bit_andnot(const uint64_t* a, const uint64_t* b, uint64_t* out) noexcept { \
        NAMESPACE::bit_andnot_##BITS(a, b, out); \
    }

#define BITCAL_GEN_SHIFT_OPS(NAMESPACE, BITS) \
    static BITCAL_FORCEINLINE void shift_left(uint64_t* data, int count) noexcept { \
        NAMESPACE::shift_left_##BITS(data, count); \
    } \
    static BITCAL_FORCEINLINE void shift_right(uint64_t* data, int count) noexcept { \
        NAMESPACE::shift_right_##BITS(data, count); \
    }

#define BITCAL_GEN_QUERY_OPS(NAMESPACE, BITS) \
    static BITCAL_FORCEINLINE bool is_zero(const uint64_t* data) noexcept { \
        return NAMESPACE::is_zero_##BITS(data); \
    } \
    static BITCAL_FORCEINLINE bool all(const uint64_t* data) noexcept { \
        return NAMESPACE::all_##BITS(data); \
    } \
    static BITCAL_FORCEINLINE bool equals(const uint64_t* a, const uint64_t* b) noexcept { \
        return NAMESPACE::equals_##BITS(a, b); \
    }

#define BITCAL_GEN_POPCOUNT_SCALAR(WORD_COUNT) \
    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept { \
        return scalar::popcount_array<WORD_COUNT>(data); \
    } \
    static BITCAL_FORCEINLINE int count_leading_zeros(const uint64_t* data) noexcept { \
        return scalar::count_leading_zeros_array<WORD_COUNT>(data); \
    } \
    static BITCAL_FORCEINLINE int count_trailing_zeros(const uint64_t* data) noexcept { \
        return scalar::count_trailing_zeros_array<WORD_COUNT>(data); \
    }

#define BITCAL_GEN_POPCOUNT_SIMD(NAMESPACE, BITS) \
    static BITCAL_FORCEINLINE uint64_t popcount(const uint64_t* data) noexcept { \
        return NAMESPACE::popcount_##BITS(data); \
    } \
    static BITCAL_FORCEINLINE int count_leading_zeros(const uint64_t* data) noexcept { \
        return scalar::count_leading_zeros_array<BITS / 64>(data); \
    } \
    static BITCAL_FORCEINLINE int count_trailing_zeros(const uint64_t* data) noexcept { \
        return scalar::count_trailing_zeros_array<BITS / 64>(data); \
    }

// ============================================================================
// SSE2 backend - 128-bit and 256-bit operations
// ============================================================================

#if BITCAL_HAS_SSE2

template<>
struct ops<128, simd_backend::sse2> {
    BITCAL_GEN_BIT_OPS(sse, 128)
    BITCAL_GEN_SHIFT_OPS(sse, 128)
    BITCAL_GEN_QUERY_OPS(sse, 128)
    BITCAL_GEN_POPCOUNT_SCALAR(2)  // SSE2 没有 popcount 指令，使用 scalar
};

template<>
struct ops<256, simd_backend::sse2> {
    BITCAL_GEN_BIT_OPS(sse, 256)
    BITCAL_GEN_SHIFT_OPS(sse, 256)
    BITCAL_GEN_QUERY_OPS(sse, 256)
    BITCAL_GEN_POPCOUNT_SCALAR(4)
};

#endif // BITCAL_HAS_SSE2

// ============================================================================
// AVX2 backend - 256-bit and 512-bit operations
// ============================================================================

#if BITCAL_HAS_AVX2

template<>
struct ops<256, simd_backend::avx2> {
    BITCAL_GEN_BIT_OPS(avx, 256)
    BITCAL_GEN_SHIFT_OPS(avx, 256)
    BITCAL_GEN_QUERY_OPS(avx, 256)
    BITCAL_GEN_POPCOUNT_SIMD(avx, 256)
};

template<>
struct ops<512, simd_backend::avx2> {
    BITCAL_GEN_BIT_OPS(avx, 512)
    BITCAL_GEN_SHIFT_OPS(avx, 512)
    BITCAL_GEN_QUERY_OPS(avx, 512)
    BITCAL_GEN_POPCOUNT_SIMD(avx, 512)
};

#endif // BITCAL_HAS_AVX2

// ============================================================================
// AVX-512 backend - 256-bit and 512-bit operations
// ============================================================================

#if BITCAL_HAS_AVX512

template<>
struct ops<256, simd_backend::avx512> {
    BITCAL_GEN_BIT_OPS(avx512, 256)
    BITCAL_GEN_SHIFT_OPS(avx512, 256)
    BITCAL_GEN_QUERY_OPS(avx512, 256)
    BITCAL_GEN_POPCOUNT_SIMD(avx512, 256)
};

template<>
struct ops<512, simd_backend::avx512> {
    BITCAL_GEN_BIT_OPS(avx512, 512)
    BITCAL_GEN_SHIFT_OPS(avx512, 512)
    BITCAL_GEN_QUERY_OPS(avx512, 512)
    BITCAL_GEN_POPCOUNT_SIMD(avx512, 512)
};

#endif // BITCAL_HAS_AVX512

// ============================================================================
// NEON backend - 128-bit, 256-bit, and 512-bit operations
// ============================================================================

#if BITCAL_HAS_NEON

template<>
struct ops<128, simd_backend::neon> {
    BITCAL_GEN_BIT_OPS(neon, 128)
    BITCAL_GEN_SHIFT_OPS(neon, 128)
    BITCAL_GEN_QUERY_OPS(neon, 128)
    BITCAL_GEN_POPCOUNT_SCALAR(2)  // NEON 没有 popcount 指令，使用 scalar
};

template<>
struct ops<256, simd_backend::neon> {
    BITCAL_GEN_BIT_OPS(neon, 256)
    BITCAL_GEN_SHIFT_OPS(neon, 256)
    BITCAL_GEN_QUERY_OPS(neon, 256)
    BITCAL_GEN_POPCOUNT_SCALAR(4)
};

template<>
struct ops<512, simd_backend::neon> {
    BITCAL_GEN_BIT_OPS(neon, 512)
    BITCAL_GEN_SHIFT_OPS(neon, 512)
    BITCAL_GEN_QUERY_OPS(neon, 512)
    BITCAL_GEN_POPCOUNT_SCALAR(8)
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

// Undefine helper macros
#undef BITCAL_GEN_BIT_OPS
#undef BITCAL_GEN_SHIFT_OPS
#undef BITCAL_GEN_QUERY_OPS
#undef BITCAL_GEN_POPCOUNT_SCALAR
#undef BITCAL_GEN_POPCOUNT_SIMD

} // namespace backend
} // namespace bitcal
