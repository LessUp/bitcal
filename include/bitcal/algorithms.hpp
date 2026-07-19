#pragma once

#include "bit_block.hpp"
#include "detail/word_ops.hpp"

#include <cassert>

namespace bitcal {

namespace detail {

// Why two operation parameters for the same logical operation:
// `vector_op` operates on a pair of AVX2 lanes and is used on the SIMD
// dispatch path (>= 4 words). `word_op` operates on a single uint64 pair
// and is used by the scalar tail / small-width path. They must express the
// same semantic operation; callers below keep them in sync per bitwise kind.
// `vector_op` is a generic lambda (auto params) so that non-AVX2 targets
// never instantiate its body (which references AVX2 intrinsics).
//
// Both the in-place `*_into` and the returning `bit_*` algorithms route
// through `binary_words` so there is a single dispatch path. For small
// widths (< 4 words) the AVX2 loop body never executes and the scalar tail
// handles everything; because `binary_into_x64` is force-inlined, the
// compiler sees the full word-by-word write of `out` and can eliminate the
// zero-initialization of the returned block as a dead store.
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

}  // namespace bitcal
