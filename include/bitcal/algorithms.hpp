#pragma once

#include "bit_block.hpp"
#include "detail/word_ops.hpp"

#include <cassert>

namespace bitcal {

namespace detail {

// `vector_op`（AVX2 lane 对）与 `word_op`（uint64 对）表达同一语义操作，
// 分别用于 SIMD 路径（>= 4 字）和 scalar 尾部 / 小宽度路径。`vector_op`
// 是 generic lambda，非 AVX2 目标不会实例化其引用 intrinsics 的函数体。
template <std::size_t Bits, typename VectorOp, typename WordOp>
[[nodiscard]] inline bit_block<Bits> compose_binary_block(const const_bit_view lhs, const const_bit_view rhs,
                                                          VectorOp&& vector_op, WordOp&& word_op) noexcept {
    assert(lhs.word_count() == bit_block<Bits>::word_count);
    assert(rhs.word_count() == bit_block<Bits>::word_count);

    bit_block<Bits> out;
    detail::binary_words(lhs, rhs, out.view(), std::forward<VectorOp>(vector_op), std::forward<WordOp>(word_op));
    return out;
}

// Shared shape for shift_left / shift_right: copy the input into a fresh
// block, then apply the in-place array shifter. The two shifts differ only
// in which `shift_*_array` is invoked, so we route that through `ShiftOp`
// rather than duplicating the copy + dispatch skeleton. Short-circuits the
// full-width-or-more case to avoid a pointless copy before the shifter
// zeroes the block.
template <std::size_t Bits, typename ShiftOp>
[[nodiscard]] inline bit_block<Bits> compose_shifted_block(const const_bit_view value, const std::size_t count,
                                                           ShiftOp&& shift_op) noexcept {
    constexpr std::size_t wc = bit_block<Bits>::word_count;
    assert(value.word_count() == wc);

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

}  // namespace detail

inline void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::binary_words(
        lhs, rhs, out, [](const auto a, const auto b) noexcept { return _mm256_and_si256(a, b); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & b; });
}

inline void or_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::binary_words(
        lhs, rhs, out, [](const auto a, const auto b) noexcept { return _mm256_or_si256(a, b); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a | b; });
}

inline void xor_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    detail::binary_words(
        lhs, rhs, out, [](const auto a, const auto b) noexcept { return _mm256_xor_si256(a, b); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a ^ b; });
}

inline void andnot_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    // AVX2 andnot takes (rhs, lhs) to compute lhs & ~rhs; scalar form mirrors that.
    detail::binary_words(
        lhs, rhs, out, [](const auto a, const auto b) noexcept { return _mm256_andnot_si256(b, a); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & ~b; });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, [](const auto a, const auto b) noexcept { return _mm256_and_si256(a, b); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & b; });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, [](const auto a, const auto b) noexcept { return _mm256_or_si256(a, b); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a | b; });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_xor(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, [](const auto a, const auto b) noexcept { return _mm256_xor_si256(a, b); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a ^ b; });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_andnot(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    return detail::compose_binary_block<Bits>(
        lhs, rhs, [](const auto a, const auto b) noexcept { return _mm256_andnot_si256(b, a); },
        [](const std::uint64_t a, const std::uint64_t b) noexcept { return a & ~b; });
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
[[nodiscard]] inline bit_block<Bits> shift_left(const const_bit_view value, const std::size_t count) noexcept {
    constexpr std::size_t wc = bit_block<Bits>::word_count;
    return detail::compose_shifted_block<Bits>(
        value, count, [](std::uint64_t* data, std::size_t c) noexcept { detail::shift_left_array<wc>(data, c); });
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_right(const const_bit_view value, const std::size_t count) noexcept {
    constexpr std::size_t wc = bit_block<Bits>::word_count;
    return detail::compose_shifted_block<Bits>(
        value, count, [](std::uint64_t* data, std::size_t c) noexcept { detail::shift_right_array<wc>(data, c); });
}

// --- bit_block overloads (CTAD on Bits) -------------------------------------
// const_bit_view 不携带宽度信息，视图形态需显式 <Bits>；bit_block<Bits>
// 可推导，省去模板参数与 .view()：
//
//     auto c = bit_and<256>(a.view(), b.view());   // before
//     auto c = bit_and(a, b);                      // after

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_and(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_and<Bits>(lhs.view(), rhs.view());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_or(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_or<Bits>(lhs.view(), rhs.view());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_xor(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_xor<Bits>(lhs.view(), rhs.view());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> bit_andnot(const bit_block<Bits>& lhs, const bit_block<Bits>& rhs) noexcept {
    return bit_andnot<Bits>(lhs.view(), rhs.view());
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_left(const bit_block<Bits>& value, const std::size_t count) noexcept {
    return shift_left<Bits>(value.view(), count);
}

template <std::size_t Bits>
[[nodiscard]] inline bit_block<Bits> shift_right(const bit_block<Bits>& value, const std::size_t count) noexcept {
    return shift_right<Bits>(value.view(), count);
}

}  // namespace bitcal
