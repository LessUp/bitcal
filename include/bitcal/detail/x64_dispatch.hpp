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

}  // namespace bitcal::detail
