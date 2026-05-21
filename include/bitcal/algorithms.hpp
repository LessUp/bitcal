#pragma once

#include "bit_block.hpp"
#include "detail/word_ops.hpp"

#include <cassert>

namespace bitcal {

inline void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::and_words(lhs, rhs, out);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    and_into(lhs, rhs, out.view());
    return out;
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    detail::or_words(lhs, rhs, out.view());
    return out;
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_xor(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    detail::xor_words(lhs, rhs, out.view());
    return out;
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_andnot(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    detail::andnot_words(lhs, rhs, out.view());
    return out;
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
    assert(value.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    detail::shift_left_words<Bits>(value, out.view(), count);
    return out;
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_right(const const_bit_view value, const int count) noexcept {
    assert(value.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    detail::shift_right_words<Bits>(value, out.view(), count);
    return out;
}

}  // namespace bitcal
