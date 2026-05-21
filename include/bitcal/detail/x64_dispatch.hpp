#pragma once

#include "../config.hpp"
#include "backend.hpp"

namespace bitcal::detail {

BITCAL_FORCEINLINE void and_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                     const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    std::size_t i = 0;
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        const auto out_vec = _mm256_and_si256(lhs_vec, rhs_vec);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i), out_vec);
    }

    for (; i < word_count; ++i) {
        out[i] = lhs[i] & rhs[i];
    }
#else
    and_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void or_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                    const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    std::size_t i = 0;
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        const auto out_vec = _mm256_or_si256(lhs_vec, rhs_vec);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i), out_vec);
    }

    for (; i < word_count; ++i) {
        out[i] = lhs[i] | rhs[i];
    }
#else
    or_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void xor_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                     const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    std::size_t i = 0;
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        const auto out_vec = _mm256_xor_si256(lhs_vec, rhs_vec);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i), out_vec);
    }

    for (; i < word_count; ++i) {
        out[i] = lhs[i] ^ rhs[i];
    }
#else
    xor_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void andnot_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    std::size_t i = 0;
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        const auto out_vec = _mm256_andnot_si256(rhs_vec, lhs_vec);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i), out_vec);
    }

    for (; i < word_count; ++i) {
        out[i] = lhs[i] & ~rhs[i];
    }
#else
    andnot_into_scalar(lhs, rhs, out, word_count);
#endif
}

}  // namespace bitcal::detail
