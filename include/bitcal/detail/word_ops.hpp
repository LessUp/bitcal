#pragma once

#include "../bit_view.hpp"
#include "x64_dispatch.hpp"

#include <bit>
#include <cassert>
#include <cstdint>

namespace bitcal::detail {

inline void and_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());

    and_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
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

}  // namespace bitcal::detail
