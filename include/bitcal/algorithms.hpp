#pragma once

#include "bit_block.hpp"
#include "detail/word_ops.hpp"

#include <cassert>

namespace bitcal {

namespace detail {

template <std::size_t Bits, typename BinaryOp, typename WordOp>
[[nodiscard]] inline bit_block<Bits> compose_binary_block(const const_bit_view lhs, const const_bit_view rhs,
                                                          BinaryOp&& binary_op, WordOp&& word_op) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    constexpr std::size_t wc = bit_block<Bits>::word_count;
    bit_block<Bits> out;

    if constexpr (wc < 4) {
        // Fast path for small widths: direct word-by-word computation.
        // Bypasses SIMD dispatch entirely, allowing the compiler to inline
        // the word operation and eliminate the zero-initialization of `out`
        // (since it can see all words are written before return).
        auto* out_data = out.view().data();
        const auto* lhs_data = lhs.data();
        const auto* rhs_data = rhs.data();
        for (std::size_t i = 0; i < wc; ++i) {
            out_data[i] = word_op(lhs_data[i], rhs_data[i]);
        }
    } else {
        // Large widths: use SIMD dispatch path for AVX2 acceleration.
        binary_op(lhs, rhs, out.view());
    }

    return out;
}

}  // namespace detail

inline void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::and_words(lhs, rhs, out);
}

inline void or_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::or_words(lhs, rhs, out);
}

inline void xor_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::xor_words(lhs, rhs, out);
}

inline void andnot_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::andnot_words(lhs, rhs, out);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, detail::and_words,
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & b; });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, detail::or_words,
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a | b; });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_xor(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, detail::xor_words,
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a ^ b; });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_andnot(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, detail::andnot_words,
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & ~b; });
}

[[nodiscard]] constexpr bool equals(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::equals_words(lhs, rhs);
}

[[nodiscard]] constexpr bool is_zero(const const_bit_view value) noexcept {
    return detail::is_zero_words(value);
}

[[nodiscard]] constexpr std::uint64_t popcount(const const_bit_view value) noexcept {
    return detail::popcount_words(value);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_left(const const_bit_view value, const int count) noexcept {
    constexpr std::size_t wc = bit_block<Bits>::word_count;
    assert(value.word_count() == wc);
    assert(count >= 0);

    bit_block<Bits> out;
    auto* out_data = out.view().data();
    const auto* in_data = value.data();
    for (std::size_t i = 0; i < wc; ++i) {
        out_data[i] = in_data[i];
    }
    detail::shift_left_array<wc>(out_data, count);
    return out;
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_right(const const_bit_view value, const int count) noexcept {
    constexpr std::size_t wc = bit_block<Bits>::word_count;
    assert(value.word_count() == wc);
    assert(count >= 0);

    bit_block<Bits> out;
    auto* out_data = out.view().data();
    const auto* in_data = value.data();
    for (std::size_t i = 0; i < wc; ++i) {
        out_data[i] = in_data[i];
    }
    detail::shift_right_array<wc>(out_data, count);
    return out;
}

}  // namespace bitcal
