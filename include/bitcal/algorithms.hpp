#pragma once

#include "bit_block.hpp"

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

}  // namespace bitcal
