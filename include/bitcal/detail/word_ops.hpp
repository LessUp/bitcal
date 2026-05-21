#pragma once

#include "../scalar_ops.hpp"
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

inline void or_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());

    or_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
}

inline void xor_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());

    xor_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count());
}

inline void andnot_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());

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
    assert(lhs.word_count() == rhs.word_count());

    for (std::size_t i = 0; i < lhs.word_count(); ++i) {
        if (lhs.word(i) != rhs.word(i)) {
            return false;
        }
    }

    return true;
}

template <std::size_t Bits>
inline void shift_left_words(const const_bit_view value, bit_view out, const int count) noexcept {
    static_assert(Bits >= 64, "Bits must be at least 64");
    static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");
    constexpr std::size_t word_count = Bits / 64;
    assert(count >= 0);
    assert(value.word_count() == word_count);
    assert(out.word_count() == word_count);

    auto* out_data = out.data();
    for (std::size_t i = 0; i < word_count; ++i) {
        out_data[i] = value.word(i);
    }

    scalar::shift_left_array<word_count>(out_data, count);
}

template <std::size_t Bits>
inline void shift_right_words(const const_bit_view value, bit_view out, const int count) noexcept {
    static_assert(Bits >= 64, "Bits must be at least 64");
    static_assert(Bits % 64 == 0, "Bits must be a multiple of 64");
    constexpr std::size_t word_count = Bits / 64;
    assert(count >= 0);
    assert(value.word_count() == word_count);
    assert(out.word_count() == word_count);

    auto* out_data = out.data();
    for (std::size_t i = 0; i < word_count; ++i) {
        out_data[i] = value.word(i);
    }

    scalar::shift_right_array<word_count>(out_data, count);
}

}  // namespace bitcal::detail
