#pragma once

#include "bit_block.hpp"
#include "detail/word_ops.hpp"

#include <span>
#include <type_traits>

namespace bitcal {

namespace detail {

// 静态宽度 word range 的 extent：std::array / C 数组 / 静态 std::span。
// 主模板无 value 成员，未覆盖的类型经 static_word_range concept 被 SFINAE 排除。
template <typename T>
struct static_word_extent {};
template <std::size_t N>
struct static_word_extent<std::array<std::uint64_t, N>> : std::integral_constant<std::size_t, N> {};
template <std::size_t N>
struct static_word_extent<std::uint64_t[N]> : std::integral_constant<std::size_t, N> {};
template <std::size_t N>
struct static_word_extent<std::span<std::uint64_t, N>> : std::integral_constant<std::size_t, N> {};
template <std::size_t N>
struct static_word_extent<std::span<const std::uint64_t, N>> : std::integral_constant<std::size_t, N> {};

template <typename T>
concept static_word_range = requires { static_word_extent<std::remove_cvref_t<T>>::value; };

template <typename T>
constexpr std::size_t static_word_extent_v = static_word_extent<std::remove_cvref_t<T>>::value;

template <typename T>
constexpr std::span<const std::uint64_t, static_word_extent_v<T>> as_word_span(const T& range) noexcept {
    return std::span<const std::uint64_t, static_word_extent_v<T>>(std::data(range), static_word_extent_v<T>);
}

// `vector_op`（AVX2 lane 对）与 `word_op`（uint64 对）表达同一语义操作，
// 分别用于 SIMD 路径（>= 4 字）和 scalar 尾部 / 小宽度路径。`vector_op`
// 是 generic lambda，非 AVX2 目标不会实例化其引用 intrinsics 的函数体。
template <std::size_t Bits, typename VectorOp, typename WordOp>
[[nodiscard]] inline bit_block<Bits> compose_binary_block(const std::span<const std::uint64_t, Bits / 64> lhs,
                                                          const std::span<const std::uint64_t, Bits / 64> rhs,
                                                          VectorOp&& vector_op, WordOp&& word_op) noexcept {
    bit_block<Bits> out;
    detail::binary_words(const_bit_view(lhs.data(), lhs.size()), const_bit_view(rhs.data(), rhs.size()), out.view(),
                         std::forward<VectorOp>(vector_op), std::forward<WordOp>(word_op));
    return out;
}

// 静态宽度 range 入口：宽度从 extent 推导，两侧宽度不一致是编译错误。
template <typename Lhs, typename Rhs, typename VectorOp, typename WordOp>
[[nodiscard]] inline auto compose_binary_range(const Lhs& lhs, const Rhs& rhs, VectorOp&& vector_op,
                                               WordOp&& word_op) noexcept {
    constexpr std::size_t lhs_words = static_word_extent_v<Lhs>;
    constexpr std::size_t rhs_words = static_word_extent_v<Rhs>;
    static_assert(lhs_words == rhs_words, "bitcal: word counts must match");
    return compose_binary_block<lhs_words * 64>(as_word_span(lhs), as_word_span(rhs), std::forward<VectorOp>(vector_op),
                                                std::forward<WordOp>(word_op));
}

// Shared shape for shift_left / shift_right: copy the input into a fresh
// block, then apply the in-place array shifter. The two shifts differ only
// in which `shift_*_array` is invoked, so we route that through `ShiftOp`
// rather than duplicating the copy + dispatch skeleton. Short-circuits the
// full-width-or-more case to avoid a pointless copy before the shifter
// zeroes the block.
template <std::size_t Bits, typename ShiftOp>
[[nodiscard]] inline bit_block<Bits> compose_shifted_block(const std::span<const std::uint64_t, Bits / 64> value,
                                                           const std::size_t count, ShiftOp&& shift_op) noexcept {
    constexpr std::size_t wc = bit_block<Bits>::word_count;

    if (count >= Bits) {
        return bit_block<Bits>{};  // all bits shifted out -> zero block
    }

    bit_block<Bits> out;
    auto* out_data = out.view().data();
    const auto* in_data = value.data();
    for (std::size_t i = 0; i < wc; ++i) {
        out_data[i] = in_data[i];
    }
    shift_op(out_data, count);
    return out;
}

// 每对 vector/word 操作只定义一次，*_into 与 bit_* 共享。
// generic lambda 作为 static constexpr 成员在 C++23 合法；非 AVX2 目标下
// lambda 体不实例化（binary_into_x64 的 #else 分支从不调用 vector）。
struct and_ops {
    static constexpr auto vector = [](const auto a, const auto b) noexcept { return _mm256_and_si256(a, b); };
    static constexpr auto word = [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & b; };
};

struct or_ops {
    static constexpr auto vector = [](const auto a, const auto b) noexcept { return _mm256_or_si256(a, b); };
    static constexpr auto word = [](const std::uint64_t a, const std::uint64_t b) noexcept { return a | b; };
};

struct xor_ops {
    static constexpr auto vector = [](const auto a, const auto b) noexcept { return _mm256_xor_si256(a, b); };
    static constexpr auto word = [](const std::uint64_t a, const std::uint64_t b) noexcept { return a ^ b; };
};

struct andnot_ops {
    // AVX2 andnot takes (rhs, lhs) to compute lhs & ~rhs; scalar form mirrors that.
    static constexpr auto vector = [](const auto a, const auto b) noexcept { return _mm256_andnot_si256(b, a); };
    static constexpr auto word = [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & ~b; };
};

}  // namespace detail

inline void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::binary_words(lhs, rhs, out, detail::and_ops::vector, detail::and_ops::word);
}

inline void or_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::binary_words(lhs, rhs, out, detail::or_ops::vector, detail::or_ops::word);
}

inline void xor_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::binary_words(lhs, rhs, out, detail::xor_ops::vector, detail::xor_ops::word);
}

inline void andnot_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::binary_words(lhs, rhs, out, detail::andnot_ops::vector, detail::andnot_ops::word);
}

template <detail::static_word_range Lhs, detail::static_word_range Rhs>
[[nodiscard]] inline auto bit_and(const Lhs& lhs, const Rhs& rhs) noexcept {
    return detail::compose_binary_range(lhs, rhs, detail::and_ops::vector, detail::and_ops::word);
}

template <detail::static_word_range Lhs, detail::static_word_range Rhs>
[[nodiscard]] inline auto bit_or(const Lhs& lhs, const Rhs& rhs) noexcept {
    return detail::compose_binary_range(lhs, rhs, detail::or_ops::vector, detail::or_ops::word);
}

template <detail::static_word_range Lhs, detail::static_word_range Rhs>
[[nodiscard]] inline auto bit_xor(const Lhs& lhs, const Rhs& rhs) noexcept {
    return detail::compose_binary_range(lhs, rhs, detail::xor_ops::vector, detail::xor_ops::word);
}

template <detail::static_word_range Lhs, detail::static_word_range Rhs>
[[nodiscard]] inline auto bit_andnot(const Lhs& lhs, const Rhs& rhs) noexcept {
    return detail::compose_binary_range(lhs, rhs, detail::andnot_ops::vector, detail::andnot_ops::word);
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

template <detail::static_word_range Words>
[[nodiscard]] inline auto shift_left(const Words& value, const std::size_t count) noexcept {
    constexpr std::size_t wc = detail::static_word_extent_v<Words>;
    return detail::compose_shifted_block<wc * 64>(
        detail::as_word_span(value), count,
        [](std::uint64_t* data, std::size_t c) noexcept { detail::shift_left_array<wc>(data, c); });
}

template <detail::static_word_range Words>
[[nodiscard]] inline auto shift_right(const Words& value, const std::size_t count) noexcept {
    constexpr std::size_t wc = detail::static_word_extent_v<Words>;
    return detail::compose_shifted_block<wc * 64>(
        detail::as_word_span(value), count,
        [](std::uint64_t* data, std::size_t c) noexcept { detail::shift_right_array<wc>(data, c); });
}

// --- bit_block overloads (CTAD on Bits) -------------------------------------
// 静态宽度形态从 range extent 推导宽度；bit_block 重载再省一步 .words()：
//
//     auto c = bit_and(a.words(), b.words());   // 静态宽度形态（std::array / C 数组 / span 同理）
//     auto c = bit_and(a, b);                   // 拥有型形态

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_and(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_and(lhs.words(), rhs.words());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_or(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_or(lhs.words(), rhs.words());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_xor(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_xor(lhs.words(), rhs.words());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_andnot(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_andnot(lhs.words(), rhs.words());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_left(const bit_block<Bits>& value, const std::size_t count) noexcept {
    return shift_left(value.words(), count);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_right(const bit_block<Bits>& value, const std::size_t count) noexcept {
    return shift_right(value.words(), count);
}

}  // namespace bitcal
