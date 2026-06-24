#pragma once

#include "../config.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace bitcal::detail {

template <typename WordOp>
inline void binary_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                               const std::size_t word_count, WordOp&& word_op) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = word_op(lhs[i], rhs[i]);
    }
}

template <std::size_t N>
inline void shift_left_array(std::uint64_t* data, std::size_t count) noexcept {
    static_assert(N > 0, "Array size must be positive");

    if (count == 0) {
        return;
    }

    // Short-circuit: shifting by the full width (or more) clears everything.
    if (count >= N * 64) {
        std::memset(data, 0, N * sizeof(std::uint64_t));
        return;
    }

    if (count >= 64) {
        const std::size_t word_shift = count / 64;
        const std::size_t bit_shift = count % 64;

        for (std::size_t i = N - 1; i >= word_shift; --i) {
            data[i] = data[i - word_shift];
        }
        for (std::size_t i = 0; i < word_shift; ++i) {
            data[i] = 0;
        }

        if (bit_shift == 0) {
            return;
        }

        count = bit_shift;
    }

    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < N; ++i) {
        const std::uint64_t next_carry = data[i] >> (64 - count);
        data[i] = (data[i] << count) | carry;
        carry = next_carry;
    }
}

template <std::size_t N>
inline void shift_right_array(std::uint64_t* data, std::size_t count) noexcept {
    static_assert(N > 0, "Array size must be positive");

    if (count == 0) {
        return;
    }

    // Short-circuit: shifting by the full width (or more) clears everything.
    if (count >= N * 64) {
        std::memset(data, 0, N * sizeof(std::uint64_t));
        return;
    }

    if (count >= 64) {
        const std::size_t word_shift = count / 64;
        const std::size_t bit_shift = count % 64;

        for (std::size_t i = 0; i < N - word_shift; ++i) {
            data[i] = data[i + word_shift];
        }
        for (std::size_t i = N - word_shift; i < N; ++i) {
            data[i] = 0;
        }

        if (bit_shift == 0) {
            return;
        }

        count = bit_shift;
    }

    std::uint64_t carry = 0;
    for (std::size_t i = N - 1; i < N; --i) {
        const std::uint64_t next_carry = data[i] << (64 - count);
        data[i] = (data[i] >> count) | carry;
        carry = next_carry;
    }
}

}  // namespace bitcal::detail
