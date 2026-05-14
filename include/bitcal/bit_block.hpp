#pragma once

#include "bit_view.hpp"

#include <cassert>
#include <array>

namespace bitcal {

template <std::size_t Bits>
class bit_block {
    static_assert(Bits >= 64, "Bits must be at least 64");
    static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");

public:
    static constexpr std::size_t bits = Bits;
    static constexpr std::size_t word_count = Bits / 64;

    constexpr bit_block() noexcept = default;

    [[nodiscard]] constexpr bit_view view() noexcept { return bit_view(words_.data(), word_count); }
    [[nodiscard]] constexpr const_bit_view view() const noexcept { return const_bit_view(words_.data(), word_count); }

    [[nodiscard]] constexpr std::uint64_t word(const std::size_t index) const noexcept {
        assert(index < word_count);
        return words_[index];
    }

private:
    std::array<std::uint64_t, word_count> words_{};
};

}  // namespace bitcal
