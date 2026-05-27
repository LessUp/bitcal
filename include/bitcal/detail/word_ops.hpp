#pragma once

#include "../scalar_ops.hpp"
#include "../bit_view.hpp"
#include "x64_dispatch.hpp"

#include <bit>
#include <cassert>
#include <cstdint>

namespace bitcal::detail {

inline void assert_binary_word_layout(const const_bit_view lhs, const const_bit_view rhs, const bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());
}

template <std::size_t Bits>
inline void assert_fixed_word_layout(const const_bit_view value, const bit_view out) noexcept {
    static_assert(Bits >= 64, "Bits must be at least 64");
    static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");
    constexpr std::size_t word_count = Bits / 64;
    assert(value.word_count() == word_count);
    assert(out.word_count() == word_count);
}

inline void copy_words(const const_bit_view value, bit_view out) noexcept {
    assert(value.word_count() == out.word_count());

    auto* out_data = out.data();
    for (std::size_t i = 0; i < out.word_count(); ++i) {
        out_data[i] = value.word(i);
    }
}

inline void and_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    and_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
}

inline void or_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    or_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
}

inline void xor_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    xor_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
}

inline void andnot_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    andnot_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
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

[[nodiscard]] constexpr bool equals_words(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    if (lhs.word_count() != rhs.word_count()) {
        return false;
    }

    for (std::size_t i = 0; i < lhs.word_count(); ++i) {
        if (lhs.word(i) != rhs.word(i)) {
            return false;
        }
    }

    return true;
}

template <std::size_t Bits>
inline void shift_left_words(const const_bit_view value, bit_view out, const int count) noexcept {
    assert(count >= 0);
    assert_fixed_word_layout<Bits>(value, out);
    copy_words(value, out);
    scalar::shift_left_array<Bits / 64>(out.data(), count);
}

template <std::size_t Bits>
inline void shift_right_words(const const_bit_view value, bit_view out, const int count) noexcept {
    assert(count >= 0);
    assert_fixed_word_layout<Bits>(value, out);
    copy_words(value, out);
    scalar::shift_right_array<Bits / 64>(out.data(), count);
}

}  // namespace bitcal::detail
