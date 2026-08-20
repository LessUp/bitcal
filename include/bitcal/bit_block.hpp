#pragma once

#include "bit_view.hpp"

#include <cassert>
#include <cstring>

#include <array>
#include <span>

namespace bitcal {

namespace detail {

// 桥接 constexpr 与运行时字拷贝路径。在 consteval 语境下字节拷贝 intrinsics
// 不可用，故回退为逐字循环；运行时委托给 `ByteCopy`（from_words 用 memcpy，
// copy_words_to 用 memmove——后者必须支持自重叠）。`if consteval` 分派集中在
// 这里，bit_block 的两个调用方无需各自重复 constexpr/运行时分叉。
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

    [[nodiscard]] static constexpr bit_block from_words(std::span<const std::uint64_t, word_count> words) noexcept {
        bit_block out;
        detail::copy_words_helper<word_count>(
            words.data(), out.words_.data(),
            [](void* dst, const void* src, std::size_t n) noexcept { std::memcpy(dst, src, n); });
        return out;
    }

    [[nodiscard]] constexpr bit_view view() noexcept { return bit_view(words_.data(), word_count); }
    [[nodiscard]] constexpr const_bit_view view() const noexcept { return const_bit_view(words_.data(), word_count); }

    // 静态 extent 的 span 形态与 from_words/copy_words_to 对称，喂给 span 形态
    // 算法（bit_and(words, words) 从中推导宽度）。
    [[nodiscard]] constexpr std::span<std::uint64_t, word_count> words() noexcept { return words_; }
    [[nodiscard]] constexpr std::span<const std::uint64_t, word_count> words() const noexcept { return words_; }

    [[nodiscard]] constexpr std::uint64_t word(const std::size_t index) const noexcept {
        assert(index < word_count);
        return words_[index];
    }

    constexpr void copy_words_to(std::span<std::uint64_t, word_count> out) const noexcept {
        // 用 memmove（而非 memcpy），因为调用方可能传入块自身的视图（自拷贝）；
        // 见 test_block_copy_words_to_supports_self_copy。
        detail::copy_words_helper<word_count>(
            words_.data(), out.data(),
            [](void* dst, const void* src, std::size_t n) noexcept { std::memmove(dst, src, n); });
    }

private:
    alignas(storage_alignment) std::array<std::uint64_t, word_count> words_{};
};

}  // namespace bitcal
