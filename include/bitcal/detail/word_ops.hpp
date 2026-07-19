#pragma once

#include "../bit_view.hpp"
#include "../config.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <bit>

namespace bitcal::detail {

// --- scalar kernels ---------------------------------------------------------

template <typename WordOp>
inline void binary_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                               const std::size_t word_count, WordOp&& word_op) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = word_op(lhs[i], rhs[i]);
    }
}

template <std::size_t N>
inline void shift_left_array(std::uint64_t* data, std::size_t count) noexcept {
    static_assert(N > 0, "Array size must be positive");

    if (count == 0) {
        return;
    }

    // Short-circuit: shifting by the full width (or more) clears everything.
    if (count >= N * 64) {
        std::memset(data, 0, N * sizeof(std::uint64_t));
        return;
    }

    if (count >= 64) {
        const std::size_t word_shift = count / 64;
        const std::size_t bit_shift = count % 64;

        // word_shift >= 1 here (count >= 64). Loop guards with `i < N`
        // so size_t underflow cannot turn into an infinite loop if this
        // guard is ever refactored away; mirrors shift_right_array style.
        for (std::size_t i = N; i-- > word_shift;) {
            data[i] = data[i - word_shift];
        }
        for (std::size_t i = 0; i < word_shift; ++i) {
            data[i] = 0;
        }

        if (bit_shift == 0) {
            return;
        }

        count = bit_shift;
    }

    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < N; ++i) {
        const std::uint64_t next_carry = data[i] >> (64 - count);
        data[i] = (data[i] << count) | carry;
        carry = next_carry;
    }
}

template <std::size_t N>
inline void shift_right_array(std::uint64_t* data, std::size_t count) noexcept {
    static_assert(N > 0, "Array size must be positive");

    if (count == 0) {
        return;
    }

    // Short-circuit: shifting by the full width (or more) clears everything.
    if (count >= N * 64) {
        std::memset(data, 0, N * sizeof(std::uint64_t));
        return;
    }

    if (count >= 64) {
        const std::size_t word_shift = count / 64;
        const std::size_t bit_shift = count % 64;

        for (std::size_t i = 0; i < N - word_shift; ++i) {
            data[i] = data[i + word_shift];
        }
        for (std::size_t i = N - word_shift; i < N; ++i) {
            data[i] = 0;
        }

        if (bit_shift == 0) {
            return;
        }

        count = bit_shift;
    }

    std::uint64_t carry = 0;
    for (std::size_t i = N - 1; i < N; --i) {
        const std::uint64_t next_carry = data[i] << (64 - count);
        data[i] = (data[i] >> count) | carry;
        carry = next_carry;
    }
}

// --- x86-64 dispatch --------------------------------------------------------
// `vector_op` is only invoked on the AVX2 path; on scalar-only targets it is
// unused, so callers may pass a generic lambda referencing AVX2 intrinsics
// without breaking non-x86 builds (the lambda body is only instantiated when
// called).

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

// --- AVX2-accelerated query/count operations --------------------------------
// popcount / is_zero / equals mirror the binary dispatch pattern: 4-word
// AVX2 chunks with a scalar tail. `if consteval` in the view-level wrappers
// below keeps the constexpr scalar path alive for compile-time evaluation.

#if BITCAL_ARCH_X86 && BITCAL_HAS_AVX2
// 256-bit LUT-based per-byte popcount: split each byte into low/high nibbles,
// look up popcount in a 16-entry table, then sum the byte counts into 64-bit
// lanes via _mm256_sad_epu8 (which produces 64-bit sums in each 128-bit half).
BITCAL_FORCEINLINE __m256i popcount_bytes_avx2(const __m256i value) noexcept {
    const auto lut = _mm256_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2,
                                      3, 2, 3, 3, 4);
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

[[nodiscard]] BITCAL_FORCEINLINE bool is_zero_x64(const std::uint64_t* data, const std::size_t word_count) noexcept {
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

// --- view-level word ops ----------------------------------------------------

inline void assert_binary_word_layout(const const_bit_view lhs, const const_bit_view rhs, const bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());
}

template <typename VectorOp, typename WordOp>
inline void binary_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out, VectorOp&& vector_op,
                         WordOp&& word_op) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    binary_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count(), std::forward<VectorOp>(vector_op),
                    std::forward<WordOp>(word_op));
}

[[nodiscard]] constexpr bool is_zero_words(const const_bit_view value) noexcept {
    if consteval {
        for (std::size_t i = 0; i < value.word_count(); ++i) {
            if (value.word(i) != 0) {
                return false;
            }
        }
        return true;
    } else {
        return is_zero_x64(value.data(), value.word_count());
    }
}

[[nodiscard]] constexpr std::uint64_t popcount_words(const const_bit_view value) noexcept {
    if consteval {
        std::uint64_t total = 0;
        for (std::size_t i = 0; i < value.word_count(); ++i) {
            total += static_cast<std::uint64_t>(std::popcount(value.word(i)));
        }
        return total;
    } else {
        return popcount_x64(value.data(), value.word_count());
    }
}

[[nodiscard]] constexpr bool equals_words(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    if (lhs.word_count() != rhs.word_count()) {
        return false;
    }

    if consteval {
        for (std::size_t i = 0; i < lhs.word_count(); ++i) {
            if (lhs.word(i) != rhs.word(i)) {
                return false;
            }
        }
        return true;
    } else {
        return equals_x64(lhs.data(), rhs.data(), lhs.word_count());
    }
}

}  // namespace bitcal::detail
