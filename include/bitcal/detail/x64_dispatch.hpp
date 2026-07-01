#pragma once

#include "../config.hpp"
#include "backend.hpp"

#include <bit>

namespace bitcal::detail {

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512
template <typename VectorOp, typename ScalarWordOp>
BITCAL_FORCEINLINE void binary_into_avx512_or_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs,
                                                     std::uint64_t* out, const std::size_t word_count,
                                                     VectorOp&& vector_op, ScalarWordOp&& scalar_word_op) noexcept {
    std::size_t i = 0;
    for (; i + 8 <= word_count; i += 8) {
        const auto lhs_vec = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(lhs + i));
        const auto rhs_vec = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(rhs + i));
        const auto out_vec = vector_op(lhs_vec, rhs_vec);
        _mm512_storeu_si512(reinterpret_cast<__m512i*>(out + i), out_vec);
    }

    for (; i < word_count; ++i) {
        out[i] = scalar_word_op(lhs[i], rhs[i]);
    }
}
#endif

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
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512
    binary_into_avx512_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m512i lhs_vec, const __m512i rhs_vec) noexcept { return _mm512_and_si512(lhs_vec, rhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word & rhs_word; });
#elif BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept { return _mm256_and_si256(lhs_vec, rhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word & rhs_word; });
#else
    and_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void or_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                    const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512
    binary_into_avx512_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m512i lhs_vec, const __m512i rhs_vec) noexcept { return _mm512_or_si512(lhs_vec, rhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word | rhs_word; });
#elif BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept { return _mm256_or_si256(lhs_vec, rhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word | rhs_word; });
#else
    or_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void xor_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                     const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512
    binary_into_avx512_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m512i lhs_vec, const __m512i rhs_vec) noexcept { return _mm512_xor_si512(lhs_vec, rhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word ^ rhs_word; });
#elif BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept { return _mm256_xor_si256(lhs_vec, rhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word ^ rhs_word; });
#else
    xor_into_scalar(lhs, rhs, out, word_count);
#endif
}

BITCAL_FORCEINLINE void andnot_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512
    binary_into_avx512_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m512i lhs_vec, const __m512i rhs_vec) noexcept { return _mm512_andnot_si512(rhs_vec, lhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word & ~rhs_word; });
#elif BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(
        lhs, rhs, out, word_count,
        [](const __m256i lhs_vec, const __m256i rhs_vec) noexcept { return _mm256_andnot_si256(rhs_vec, lhs_vec); },
        [](const std::uint64_t lhs_word, const std::uint64_t rhs_word) noexcept { return lhs_word & ~rhs_word; });
#else
    andnot_into_scalar(lhs, rhs, out, word_count);
#endif
}

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
BITCAL_FORCEINLINE __m256i popcount_lut_avx2() noexcept {
    return _mm256_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3,
                            4);
}

BITCAL_FORCEINLINE __m256i popcount_bytes_avx2(const __m256i value) noexcept {
    const auto lut = popcount_lut_avx2();
    const auto low_mask = _mm256_set1_epi8(0x0F);
    const auto low = _mm256_and_si256(value, low_mask);
    const auto high = _mm256_and_si256(_mm256_srli_epi16(value, 4), low_mask);
    return _mm256_add_epi8(_mm256_shuffle_epi8(lut, low), _mm256_shuffle_epi8(lut, high));
}

BITCAL_FORCEINLINE std::uint64_t sum_epu64_avx2(const __m256i value) noexcept {
    return static_cast<std::uint64_t>(_mm256_extract_epi64(value, 0)) +
           static_cast<std::uint64_t>(_mm256_extract_epi64(value, 1)) +
           static_cast<std::uint64_t>(_mm256_extract_epi64(value, 2)) +
           static_cast<std::uint64_t>(_mm256_extract_epi64(value, 3));
}
#endif

[[nodiscard]] BITCAL_FORCEINLINE std::uint64_t popcount_x64(const std::uint64_t* data,
                                                            const std::size_t word_count) noexcept {
    std::size_t i = 0;
    std::uint64_t total = 0;

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512_VPOPCNTDQ
    auto total_vec = _mm512_setzero_si512();
    for (; i + 8 <= word_count; i += 8) {
        const auto value = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data + i));
        total_vec = _mm512_add_epi64(total_vec, _mm512_popcnt_epi64(value));
    }

    alignas(64) std::uint64_t lanes[8]{};
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(lanes), total_vec);
    for (const auto lane : lanes) {
        total += lane;
    }
#elif BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    auto total_vec = _mm256_setzero_si256();
    const auto zero = _mm256_setzero_si256();
    for (; i + 4 <= word_count; i += 4) {
        const auto value = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        total_vec = _mm256_add_epi64(total_vec, _mm256_sad_epu8(popcount_bytes_avx2(value), zero));
    }
    total += sum_epu64_avx2(total_vec);
#endif

    for (; i < word_count; ++i) {
        total += static_cast<std::uint64_t>(std::popcount(data[i]));
    }
    return total;
}

[[nodiscard]] BITCAL_FORCEINLINE bool is_zero_x64(const std::uint64_t* data, const std::size_t word_count) noexcept {
    std::size_t i = 0;

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512
    for (; i + 8 <= word_count; i += 8) {
        const auto value = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data + i));
        if (_mm512_cmpeq_epi64_mask(value, _mm512_setzero_si512()) != 0xFF) {
            return false;
        }
    }
#elif BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    for (; i + 4 <= word_count; i += 4) {
        const auto value = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        if (_mm256_testz_si256(value, value) == 0) {
            return false;
        }
    }
#endif

    for (; i < word_count; ++i) {
        if (data[i] != 0) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] BITCAL_FORCEINLINE bool equals_x64(const std::uint64_t* lhs, const std::uint64_t* rhs,
                                                 const std::size_t word_count) noexcept {
    std::size_t i = 0;

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX512
    for (; i + 8 <= word_count; i += 8) {
        const auto lhs_vec = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(lhs + i));
        const auto rhs_vec = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(rhs + i));
        if (_mm512_cmpeq_epi64_mask(lhs_vec, rhs_vec) != 0xFF) {
            return false;
        }
    }
#elif BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    const auto all_ones = _mm256_set1_epi32(-1);
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        const auto cmp = _mm256_cmpeq_epi64(lhs_vec, rhs_vec);
        if (_mm256_testc_si256(cmp, all_ones) == 0) {
            return false;
        }
    }
#endif

    for (; i < word_count; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace bitcal::detail
