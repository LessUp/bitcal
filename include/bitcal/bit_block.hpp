#pragma once

#include "bit_view.hpp"

#include <cassert>
#include <cstring>

#include <array>
#include <span>

namespace bitcal {

namespace detail {

// Bridges constexpr and runtime word-copy paths. In a consteval context the
// byte-copy intrinsics are unavailable, so we fall back to a per-word loop;
// at runtime we delegate to `ByteCopy` (memcpy for from_words, memmove for
// copy_words_to, which must support self-overlap). The `if consteval`
// dispatch is centralized here so bit_block's two callers don't each repeat
// the constexpr/runtime fork.
template <std::size_t N, typename ByteCopy>
constexpr void copy_words_helper(const std::uint64_t* src, std::uint64_t* dst, ByteCopy&& byte_copy) noexcept {
    if consteval {
        for (std::size_t i = 0; i < N; ++i) {
            dst[i] = src[i];
        }
    } else {
        byte_copy(dst, src, N * sizeof(std::uint64_t));
    }
}

}  // namespace detail

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
        detail::copy_words_helper<word_count>(
            words.data(), out.words_.data(),
            [](void* dst, const void* src, std::size_t n) noexcept { std::memcpy(dst, src, n); });
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

        // memmove (not memcpy) because callers may pass the block's own view
        // (self-copy); see test_vnext_block_copy_words_to_supports_self_copy.
        detail::copy_words_helper<word_count>(
            words_.data(), out.data(),
            [](void* dst, const void* src, std::size_t n) noexcept { std::memmove(dst, src, n); });
    }

private:
    alignas(storage_alignment) std::array<std::uint64_t, word_count> words_{};
};

}  // namespace bitcal
