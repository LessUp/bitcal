#pragma once

#include "../bit_view.hpp"
#include "scalar_impl.hpp"
#include "x64_dispatch.hpp"

#include <bit>
#include <cassert>
#include <cstdint>

namespace bitcal::detail {

inline void assert_binary_word_layout(const const_bit_view lhs, const const_bit_view rhs, const bit_view out) noexcept {
    assert(lhs.word_count() == rhs.word_count());
    assert(lhs.word_count() == out.word_count());
}

template <typename VectorOp, typename WordOp>
inline void binary_words(const const_bit_view lhs, const const_bit_view rhs, bit_view out,
                         VectorOp&& vector_op, WordOp&& word_op) noexcept {
    assert_binary_word_layout(lhs, rhs, out);
    binary_into_x64(lhs.data(), rhs.data(), out.data(), out.word_count(),
                    std::forward<VectorOp>(vector_op), std::forward<WordOp>(word_op));
}

[[nodiscard]] constexpr bool is_zero_words(const const_bit_view value) noexcept {
    if consteval {
        for (std::size_t i = 0; i < value.word_count(); ++i) {
            if (value.word(i) != 0) {
                return false;
            }
        }
        return true;
    } else {
        return is_zero_x64(value.data(), value.word_count());
    }
}

[[nodiscard]] constexpr std::uint64_t popcount_words(const const_bit_view value) noexcept {
    if consteval {
        std::uint64_t total = 0;
        for (std::size_t i = 0; i < value.word_count(); ++i) {
            total += static_cast<std::uint64_t>(std::popcount(value.word(i)));
        }
        return total;
    } else {
        return popcount_x64(value.data(), value.word_count());
    }
}

[[nodiscard]] constexpr bool equals_words(const const_bit_view lhs, const const_bit_view rhs) noexcept {
    if (lhs.word_count() != rhs.word_count()) {
        return false;
    }

    if consteval {
        for (std::size_t i = 0; i < lhs.word_count(); ++i) {
            if (lhs.word(i) != rhs.word(i)) {
                return false;
            }
        }
        return true;
    } else {
        return equals_x64(lhs.data(), rhs.data(), lhs.word_count());
    }
}

}  // namespace bitcal::detail
