#pragma once

#include "../config.hpp"
#include "scalar_impl.hpp"

#include <bit>

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

// Unified x64 entry point for binary word ops. `vector_op` is only invoked on
// the AVX2 path; on scalar-only targets it is unused, so callers may pass a
// generic lambda referencing AVX2 intrinsics without breaking non-x86 builds
// (the lambda body is only instantiated when called).
template <typename VectorOp, typename WordOp>
BITCAL_FORCEINLINE void binary_into_x64(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count, VectorOp&& vector_op, WordOp&& word_op) noexcept {
#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    binary_into_avx2_or_scalar(lhs, rhs, out, word_count, std::forward<VectorOp>(vector_op),
                               std::forward<WordOp>(word_op));
#else
    (void)vector_op;
    binary_into_scalar(lhs, rhs, out, word_count, std::forward<WordOp>(word_op));
#endif
}

// ============================================================================
// AVX2-accelerated query/count operations (popcount, is_zero, equals)
// ============================================================================
// These mirror the binary dispatch pattern: 4-word AVX2 chunks with a scalar
// tail. `if consteval` in word_ops.hpp keeps the constexpr scalar path alive
// for compile-time evaluation.

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
// 256-bit LUT-based per-byte popcount: split each byte into low/high nibbles,
// look up popcount in a 16-entry table, then sum the byte counts into 64-bit
// lanes via _mm256_sad_epu8 (which produces 64-bit sums in each 128-bit half).
BITCAL_FORCEINLINE __m256i popcount_bytes_avx2(const __m256i value) noexcept {
    const auto lut = _mm256_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
                                      0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4);
    const auto low_mask = _mm256_set1_epi8(0x0F);
    const auto low = _mm256_and_si256(value, low_mask);
    const auto high = _mm256_and_si256(_mm256_srli_epi16(value, 4), low_mask);
    return _mm256_add_epi8(_mm256_shuffle_epi8(lut, low), _mm256_shuffle_epi8(lut, high));
}
#endif

[[nodiscard]] BITCAL_FORCEINLINE std::uint64_t popcount_x64(const std::uint64_t* data,
                                                            const std::size_t word_count) noexcept {
    std::size_t i = 0;
    std::uint64_t total = 0;

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    auto total_vec = _mm256_setzero_si256();
    const auto zero = _mm256_setzero_si256();
    for (; i + 4 <= word_count; i += 4) {
        const auto value = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        // popcount_bytes_avx2 gives per-byte counts; sad_epu8 against zero
        // horizontally sums each 8-byte group into a 64-bit lane.
        total_vec = _mm256_add_epi64(total_vec, _mm256_sad_epu8(popcount_bytes_avx2(value), zero));
    }
    total += static_cast<std::uint64_t>(_mm256_extract_epi64(total_vec, 0)) +
             static_cast<std::uint64_t>(_mm256_extract_epi64(total_vec, 1)) +
             static_cast<std::uint64_t>(_mm256_extract_epi64(total_vec, 2)) +
             static_cast<std::uint64_t>(_mm256_extract_epi64(total_vec, 3));
#endif

    for (; i < word_count; ++i) {
        total += static_cast<std::uint64_t>(std::popcount(data[i]));
    }
    return total;
}

[[nodiscard]] BITCAL_FORCEINLINE bool is_zero_x64(const std::uint64_t* data,
                                                  const std::size_t word_count) noexcept {
    std::size_t i = 0;

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    for (; i + 4 <= word_count; i += 4) {
        const auto value = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        // _mm256_testz_si256 returns 1 if (value & value) == 0, i.e. value is all zeros.
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

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
    const auto all_ones = _mm256_set1_epi32(-1);
    for (; i + 4 <= word_count; i += 4) {
        const auto lhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(lhs + i));
        const auto rhs_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rhs + i));
        // _mm256_testc_si256 returns 1 if (cmp & all_ones) == all_ones, i.e. all lanes equal.
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
