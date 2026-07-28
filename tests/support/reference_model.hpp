#pragma once

#include <cstdint>

#include <array>
#include <bit>

namespace bitcal::test {

template <std::size_t Bits>
using word_array = std::array<std::uint64_t, Bits / 64>;

template <std::size_t Bits>
word_array<Bits> reference_bit_and(const word_array<Bits>& lhs, const word_array<Bits>& rhs) {
    word_array<Bits> out{};

    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = lhs[i] & rhs[i];
    }

    return out;
}

template <std::size_t Bits>
word_array<Bits> reference_bit_or(const word_array<Bits>& lhs, const word_array<Bits>& rhs) {
    word_array<Bits> out{};

    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = lhs[i] | rhs[i];
    }

    return out;
}

template <std::size_t Bits>
word_array<Bits> reference_bit_xor(const word_array<Bits>& lhs, const word_array<Bits>& rhs) {
    word_array<Bits> out{};

    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = lhs[i] ^ rhs[i];
    }

    return out;
}

template <std::size_t Bits>
word_array<Bits> reference_bit_andnot(const word_array<Bits>& lhs, const word_array<Bits>& rhs) {
    word_array<Bits> out{};

    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = lhs[i] & ~rhs[i];
    }

    return out;
}

template <std::size_t WordCount>
std::uint64_t reference_popcount(const std::array<std::uint64_t, WordCount>& words) {
    std::uint64_t total = 0;

    for (const auto word : words) {
        total += std::popcount(word);
    }

    return total;
}

template <std::size_t WordCount>
bool reference_is_zero(const std::array<std::uint64_t, WordCount>& words) {
    for (const auto word : words) {
        if (word != 0) {
            return false;
        }
    }

    return true;
}

template <std::size_t Bits>
bool reference_equals(const word_array<Bits>& lhs, const word_array<Bits>& rhs) {
    for (std::size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

// Bit-by-bit reference shifts: obviously correct, O(Bits) per call.
template <std::size_t Bits>
word_array<Bits> reference_shift_left(const word_array<Bits>& words, std::size_t count) {
    word_array<Bits> out{};
    if (count >= Bits)
        return out;
    for (std::size_t i = 0; i + count < Bits; ++i) {
        if ((words[i / 64] >> (i % 64)) & 1ULL) {
            out[(i + count) / 64] |= 1ULL << ((i + count) % 64);
        }
    }
    return out;
}

template <std::size_t Bits>
word_array<Bits> reference_shift_right(const word_array<Bits>& words, std::size_t count) {
    word_array<Bits> out{};
    if (count >= Bits)
        return out;
    for (std::size_t i = count; i < Bits; ++i) {
        if ((words[i / 64] >> (i % 64)) & 1ULL) {
            out[(i - count) / 64] |= 1ULL << ((i - count) % 64);
        }
    }
    return out;
}

}  // namespace bitcal::test
