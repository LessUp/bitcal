#pragma once

#include "bit_block.hpp"
#include "detail/word_ops.hpp"

#include <cassert>

namespace bitcal {

namespace detail {

template <std::size_t Bits, typename BinaryOp>
[[nodiscard]] inline bit_block<Bits> compose_binary_block(const const_bit_view lhs, const const_bit_view rhs,
                                                          BinaryOp&& binary_op) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    binary_op(lhs, rhs, out.view());
    return out;
}

template <std::size_t Bits, typename ShiftOp>
[[nodiscard]] inline bit_block<Bits> compose_shifted_block(const const_bit_view value, const int count,
                                                           ShiftOp&& shift_op) noexcept {
    assert(value.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    shift_op(value, out.view(), count);
    return out;
}

}  // namespace detail

inline void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::and_words(lhs, rhs, out);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(lhs, rhs, [](const const_bit_view lhs_view, const const_bit_view rhs_view,
                                                           bit_view out_view) noexcept {
        and_into(lhs_view, rhs_view, out_view);
    });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(lhs, rhs, detail::or_words);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_xor(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(lhs, rhs, detail::xor_words);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_andnot(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(lhs, rhs, detail::andnot_words);
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
    return detail::compose_shifted_block<Bits>(value, count, detail::shift_left_words<Bits>);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_right(const const_bit_view value, const int count) noexcept {
    return detail::compose_shifted_block<Bits>(value, count, detail::shift_right_words<Bits>);
}

}  // namespace bitcal
