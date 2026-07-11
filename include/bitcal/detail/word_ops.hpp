#pragma once

#include "../bit_view.hpp"
#include "../config.hpp"

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

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

        for (std::size_t i = N - 1; i >= word_shift; --i) {
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

// --- view-level word ops ----------------------------------------------------

inline void assert_binary_word_layout(const const_bit_view lhs, const const_bit_view rhs, const bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());
}

template <typename VectorOp, typename WordOp>
inline void binary_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out,
                         VectorOp&& vector_op, WordOp&& word_op) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    binary_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count(),
                    std::forward<VectorOp>(vector_op), std::forward<WordOp>(word_op));
}

[[nodiscard]] constexpr bool is_zero_words(const const_bit_view value) noexcept {
    for (std::size_t i = 0; i < value.word_count(); ++i) {
        if (value.word(i) != 0) {
            return false;
        }
    }

    return true;
}

[[nodiscard]] constexpr std::uint64_t popcount_words(const const_bit_view value) noexcept {
    std::uint64_t total = 0;

    for (std::size_t i = 0; i < value.word_count(); ++i) {
        total += static_cast<std::uint64_t>(std::popcount(value.word(i)));
    }

    return total;
}

[[nodiscard]] constexpr bool equals_words(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    if (lhs.word_count() != rhs.word_count()) {
        return false;
    }

    for (std::size_t i = 0; i < lhs.word_count(); ++i) {
        if (lhs.word(i) != rhs.word(i)) {
            return false;
        }
    }

    return true;
}

}  // namespace bitcal::detail
