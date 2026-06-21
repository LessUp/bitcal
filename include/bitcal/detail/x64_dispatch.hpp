#pragma once

#include "../config.hpp"
#include "scalar_impl.hpp"

namespace bitcal::detail {

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
template <typename VectorOp, typename ScalarWordOp>
BITCAL_FORCEINLINE void binary_into_avx2_or_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs,
                                                   std::uint64_t* out, const std::size_t word_count,
                                                   VectorOp&& vector_op, ScalarWordOp&& scalar_word_op) noexcept {
    std::size_t i = 0;
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        const auto out_vec = vector_op(lhs_vec, rhs_vec);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i), out_vec);
    }

    for (; i < word_count; ++i) {
        out[i] = scalar_word_op(lhs[i], rhs[i]);
    }
}
#endif

BITCAL_FORCEINLINE void and_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                     const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count, [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept {
            return _mm256_and_si256(lhs_vec, rhs_vec);
        },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word & rhs_word; });
#else
    and_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void or_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                    const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count, [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept {
            return _mm256_or_si256(lhs_vec, rhs_vec);
        },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word | rhs_word; });
#else
    or_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void xor_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                     const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count, [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept {
            return _mm256_xor_si256(lhs_vec, rhs_vec);
        },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word ^ rhs_word; });
#else
    xor_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void andnot_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count, [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept {
            return _mm256_andnot_si256(rhs_vec, lhs_vec);
        },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word & ~rhs_word; });
#else
    andnot_into_scalar(lhs, rhs, out, word_count);
#endif
}

}  // namespace bitcal::detail
