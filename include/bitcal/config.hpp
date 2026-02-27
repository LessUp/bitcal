#pragma once

#include <cstddef>
#include <cstdint>

namespace bitcal {

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    #define BITCAL_ARCH_X86 1
    #define BITCAL_ARCH_ARM 0
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
    #define BITCAL_ARCH_X86 0
    #define BITCAL_ARCH_ARM 1
#else
    #define BITCAL_ARCH_X86 0
    #define BITCAL_ARCH_ARM 0
#endif

#if BITCAL_ARCH_X86
    #if defined(_MSC_VER)
        #include <intrin.h>
    #endif
    #include <immintrin.h>
    
    #if defined(__SSE2__) || (defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64)))
        #define BITCAL_HAS_SSE2 1
    #else
        #define BITCAL_HAS_SSE2 0
    #endif
    
    #if defined(__AVX__) || (defined(_MSC_VER) && defined(__AVX__))
        #define BITCAL_HAS_AVX 1
    #else
        #define BITCAL_HAS_AVX 0
    #endif
    
    #if defined(__AVX2__) || (defined(_MSC_VER) && defined(__AVX2__))
        #define BITCAL_HAS_AVX2 1
    #else
        #define BITCAL_HAS_AVX2 0
    #endif
    
    #if defined(__AVX512F__) || (defined(_MSC_VER) && defined(__AVX512F__))
        #define BITCAL_HAS_AVX512 1
    #else
        #define BITCAL_HAS_AVX512 0
    #endif
#endif

#if BITCAL_ARCH_ARM
    #include <arm_neon.h>
    #if defined(__ARM_NEON) || defined(__ARM_NEON__)
        #define BITCAL_HAS_NEON 1
    #else
        #define BITCAL_HAS_NEON 0
    #endif
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define BITCAL_INLINE __attribute__((always_inline)) inline
    #define BITCAL_FORCEINLINE __attribute__((always_inline)) inline
    #define BITCAL_LIKELY(x) __builtin_expect(!!(x), 1)
    #define BITCAL_UNLIKELY(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER)
    #define BITCAL_INLINE __forceinline
    #define BITCAL_FORCEINLINE __forceinline
    #define BITCAL_LIKELY(x) (x)
    #define BITCAL_UNLIKELY(x) (x)
#else
    #define BITCAL_INLINE inline
    #define BITCAL_FORCEINLINE inline
    #define BITCAL_LIKELY(x) (x)
    #define BITCAL_UNLIKELY(x) (x)
#endif

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
    #define BITCAL_CONSTEXPR constexpr
#else
    #define BITCAL_CONSTEXPR inline
#endif

#ifndef BITCAL_ALIGNMENT
    #define BITCAL_ALIGNMENT 64
#endif

enum class simd_backend {
    scalar,
    sse2,
    avx,
    avx2,
    avx512,
    neon
};

BITCAL_CONSTEXPR simd_backend get_default_backend() noexcept {
#if BITCAL_HAS_AVX512
    return simd_backend::avx512;
#elif BITCAL_HAS_AVX2
    return simd_backend::avx2;
#elif BITCAL_HAS_AVX
    return simd_backend::avx;
#elif BITCAL_HAS_SSE2
    return simd_backend::sse2;
#elif BITCAL_HAS_NEON
    return simd_backend::neon;
#else
    return simd_backend::scalar;
#endif
}

template<typename T>
struct alignas(BITCAL_ALIGNMENT) aligned_array {
    T data[1];
};

}
