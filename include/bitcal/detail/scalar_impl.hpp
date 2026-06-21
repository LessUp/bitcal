#pragma once

#include "../config.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace bitcal::detail {

BITCAL_FORCEINLINE void and_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = lhs[i] & rhs[i];
    }
}

BITCAL_FORCEINLINE void or_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                       const std::size_t word_count) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = lhs[i] | rhs[i];
    }
}

BITCAL_FORCEINLINE void xor_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                        const std::size_t word_count) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = lhs[i] ^ rhs[i];
    }
}

BITCAL_FORCEINLINE void andnot_into_scalar(const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* out,
                                           const std::size_t word_count) noexcept {
    for (std::size_t i = 0; i < word_count; ++i) {
        out[i] = lhs[i] & ~rhs[i];
    }
}

template <std::size_t N>
BITCAL_FORCEINLINE void shift_left_array(std::uint64_t* data, int count) noexcept {
    static_assert(N > 0, "Array size must be positive");

    if (count <= 0) {
        return;
    }

    if (count >= 64) {
        const int word_shift = count / 64;
        const int bit_shift = count % 64;

        if (word_shift >= static_cast<int>(N)) {
            std::memset(data, 0, N * sizeof(std::uint64_t));
            return;
        }

        for (int i = static_cast<int>(N) - 1; i >= word_shift; --i) {
            data[i] = data[i - word_shift];
        }
        for (int i = 0; i < word_shift; ++i) {
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
BITCAL_FORCEINLINE void shift_right_array(std::uint64_t* data, int count) noexcept {
    static_assert(N > 0, "Array size must be positive");

    if (count <= 0) {
        return;
    }

    if (count >= 64) {
        const int word_shift = count / 64;
        const int bit_shift = count % 64;

        if (word_shift >= static_cast<int>(N)) {
            std::memset(data, 0, N * sizeof(std::uint64_t));
            return;
        }

        for (std::size_t i = 0; i < N - static_cast<std::size_t>(word_shift); ++i) {
            data[i] = data[i + static_cast<std::size_t>(word_shift)];
        }
        for (std::size_t i = N - static_cast<std::size_t>(word_shift); i < N; ++i) {
            data[i] = 0;
        }

        if (bit_shift == 0) {
            return;
        }

        count = bit_shift;
    }

    std::uint64_t carry = 0;
    for (int i = static_cast<int>(N) - 1; i >= 0; --i) {
        const std::uint64_t next_carry = data[i] << (64 - count);
        data[i] = (data[i] >> count) | carry;
        carry = next_carry;
    }
}

}  // namespace bitcal::detail
