#pragma once

#include "bit_block.hpp"
#include "detail/backend.hpp"
#include "detail/x64_dispatch.hpp"

#include <bit>
#include <cassert>

namespace bitcal {

inline void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());

    detail::and_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    and_into(lhs, rhs, out.view());
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
