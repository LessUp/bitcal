#pragma once

#include <cstddef>
#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define BITCAL_ARCH_X86 1
#else
#define BITCAL_ARCH_X86 0
#endif

#if BITCAL_ARCH_X86
#include <immintrin.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
#define BITCAL_FORCEINLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define BITCAL_FORCEINLINE __forceinline
#else
#define BITCAL_FORCEINLINE inline
#endif

#define BITCAL_VERSION_MAJOR 4
#define BITCAL_VERSION_MINOR 0
#define BITCAL_VERSION_PATCH 0
#define BITCAL_VERSION ((BITCAL_VERSION_MAJOR << 16) | (BITCAL_VERSION_MINOR << 8) | BITCAL_VERSION_PATCH)

#if BITCAL_ARCH_X86
#if defined(__SSE2__) || defined(_M_X64)
#define BITCAL_HAS_SSE2 1
#else
#define BITCAL_HAS_SSE2 0
#endif

#if defined(__AVX2__)
#define BITCAL_HAS_AVX2 1
#else
#define BITCAL_HAS_AVX2 0
#endif

#if defined(__AVX512F__)
#define BITCAL_HAS_AVX512 1
#else
#define BITCAL_HAS_AVX512 0
#endif

#if defined(__AVX512VPOPCNTDQ__)
#define BITCAL_HAS_AVX512_VPOPCNTDQ 1
#else
#define BITCAL_HAS_AVX512_VPOPCNTDQ 0
#endif

#define BITCAL_HAS_NEON 0
#else
#define BITCAL_HAS_SSE2 0
#define BITCAL_HAS_AVX2 0
#define BITCAL_HAS_AVX512 0
#define BITCAL_HAS_AVX512_VPOPCNTDQ 0

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#define BITCAL_HAS_NEON 1
#else
#define BITCAL_HAS_NEON 0
#endif
#endif

namespace bitcal {

enum class simd_backend {
    scalar,
    sse2,
    avx2,
    avx512,
    neon
};

enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512,
};

// Optimal alignment based on bit width
template <size_t Bits>
constexpr size_t get_optimal_alignment() noexcept {
    if constexpr (Bits <= 64)
        return 8;       // 64-bit: 8-byte alignment
    else if constexpr (Bits <= 128)
        return 16;      // 128-bit: 16-byte alignment (SSE)
    else if constexpr (Bits <= 256)
        return 32;      // 256-bit: 32-byte alignment (AVX)
    else
        return 64;      // 512-bit+: 64-byte alignment (AVX-512)
}

[[nodiscard]] constexpr simd_backend get_default_simd_backend() noexcept {
#if BITCAL_HAS_AVX512
    return simd_backend::avx512;
#elif BITCAL_HAS_AVX2
    return simd_backend::avx2;
#elif BITCAL_HAS_SSE2
    return simd_backend::sse2;
#elif BITCAL_HAS_NEON
    return simd_backend::neon;
#else
    return simd_backend::scalar;
#endif
}

[[nodiscard]] constexpr backend_kind default_backend() noexcept {
#if BITCAL_ARCH_X86
#if defined(__AVX512F__)
    return backend_kind::avx512;
#elif defined(__AVX2__)
    return backend_kind::avx2;
#elif defined(__SSE2__) || defined(_M_X64)
    return backend_kind::sse2;
#else
    return backend_kind::scalar;
#endif
#else
    return backend_kind::scalar;
#endif
}

}  // namespace bitcal
