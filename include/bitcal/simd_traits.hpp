#pragma once

#include "config.hpp"
#include <type_traits>

namespace bitcal {
namespace detail {

template<simd_backend Backend>
struct simd_traits;

template<>
struct simd_traits<simd_backend::scalar> {
    static constexpr bool available = true;
    static constexpr size_t register_width = 64;
    static constexpr size_t alignment = 8;

    using u64_t = uint64_t;
    using u32_t = uint32_t;
    using u8_t = uint8_t;
};

#if BITCAL_HAS_SSE2
template<>
struct simd_traits<simd_backend::sse2> {
    static constexpr bool available = true;
    static constexpr size_t register_width = 128;
    static constexpr size_t alignment = 16;

    using u128_t = __m128i;
};
#endif

#if BITCAL_HAS_AVX2
template<>
struct simd_traits<simd_backend::avx2> {
    static constexpr bool available = true;
    static constexpr size_t register_width = 256;
    static constexpr size_t alignment = 32;

    using u256_t = __m256i;
};
#endif

#if BITCAL_HAS_AVX512
template<>
struct simd_traits<simd_backend::avx512> {
    static constexpr bool available = true;
    static constexpr size_t register_width = 512;
    static constexpr size_t alignment = 64;

    using u512_t = __m512i;
};
#endif

#if BITCAL_HAS_NEON
template<>
struct simd_traits<simd_backend::neon> {
    static constexpr bool available = true;
    static constexpr size_t register_width = 128;
    static constexpr size_t alignment = 16;

    using u128_t = uint64x2_t;
};
#endif

}
}
