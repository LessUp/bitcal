#pragma once

#include "bit_block.hpp"

#include <bit>
#include <cassert>

namespace bitcal {

template <std::size_t Bits>
[[nodiscard]] constexpr bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    auto out_view = out.view();
    auto* out_words = out_view.data();

    for (std::size_t i = 0; i < bit_block<Bits>::word_count; ++i) {
        out_words[i] = lhs.word(i) & rhs.word(i);
    }

    return out;
}

[[nodiscard]] constexpr bool is_zero(const const_bit_view value) noexcept {
    for (std::size_t i = 0; i < value.word_count(); ++i) {
        if (value.word(i) != 0) {
            return false;
        }
    }

    return true;
}

[[nodiscard]] constexpr std::uint64_t popcount(const const_bit_view value) noexcept {
    std::uint64_t total = 0;

    for (std::size_t i = 0; i < value.word_count(); ++i) {
        total += static_cast<std::uint64_t>(std::popcount(value.word(i)));
    }

    return total;
}

}  // namespace bitcal
