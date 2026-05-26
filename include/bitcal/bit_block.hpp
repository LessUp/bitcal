#pragma once

#include "bit_view.hpp"

#include <cassert>
#include <array>
#include <cstring>
#include <span>

namespace bitcal {

template <std::size_t Bits>
class bit_block {
    static_assert(Bits >= 64, "Bits must be at least 64");
    static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");

public:
    static constexpr std::size_t bits = Bits;
    static constexpr std::size_t word_count = Bits / 64;
    static constexpr std::size_t storage_alignment = get_optimal_alignment<Bits>();

    constexpr bit_block() noexcept = default;

    [[nodiscard]] static constexpr bit_block from_words(const std::span<const std::uint64_t> words) noexcept {
        assert(words.size() == word_count);

        bit_block out;
        if consteval {
            for (std::size_t i = 0; i < word_count; ++i) {
                out.words_[i] = words[i];
            }
        } else {
            std::memcpy(out.words_.data(), words.data(), word_count * sizeof(std::uint64_t));
        }

        return out;
    }

    [[nodiscard]] constexpr bit_view view() noexcept { return bit_view(words_.data(), word_count); }
    [[nodiscard]] constexpr const_bit_view view() const noexcept { return const_bit_view(words_.data(), word_count); }

    [[nodiscard]] constexpr std::uint64_t word(const std::size_t index) const noexcept {
        assert(index < word_count);
        return words_[index];
    }

    constexpr void copy_words_to(const std::span<std::uint64_t> out) const noexcept {
        assert(out.size() == word_count);

        if consteval {
            for (std::size_t i = 0; i < word_count; ++i) {
                out[i] = words_[i];
            }
        } else {
            std::memcpy(out.data(), words_.data(), word_count * sizeof(std::uint64_t));
        }
    }

private:
    alignas(storage_alignment) std::array<std::uint64_t, word_count> words_{};
};

}  // namespace bitcal
