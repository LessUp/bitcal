#pragma once

#include <array>
#include <cstdint>
#include <random>
#include <span>
#include <vector>

#include <bitcal/bit_block.hpp>

namespace bitcal::test {

template <std::size_t Bits>
struct random_binary_case {
    bitcal::bit_block<Bits> lhs;
    bitcal::bit_block<Bits> rhs;
    std::array<std::uint64_t, Bits / 64> lhs_words;
    std::array<std::uint64_t, Bits / 64> rhs_words;
};

template <std::size_t Bits>
struct random_unary_case {
    bitcal::bit_block<Bits> block;
    std::array<std::uint64_t, Bits / 64> words;
};

template <std::size_t Bits>
std::vector<random_binary_case<Bits>> make_random_binary_cases(const std::uint64_t seed, const std::size_t count) {
    std::mt19937_64 rng(seed);
    std::vector<random_binary_case<Bits>> cases;
    cases.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        random_binary_case<Bits> tc{};

        for (std::size_t word = 0; word < Bits / 64; ++word) {
            tc.lhs_words[word] = rng();
            tc.rhs_words[word] = rng();
        }

        tc.lhs = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(tc.lhs_words.data(), tc.lhs_words.size()));
        tc.rhs = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(tc.rhs_words.data(), tc.rhs_words.size()));
        cases.push_back(tc);
    }

    return cases;
}

template <std::size_t Bits>
std::vector<random_unary_case<Bits>> make_random_unary_cases(const std::uint64_t seed, const std::size_t count) {
    std::mt19937_64 rng(seed);
    std::vector<random_unary_case<Bits>> cases;
    cases.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        random_unary_case<Bits> tc{};

        for (std::size_t word = 0; word < Bits / 64; ++word) {
            tc.words[word] = rng();
        }

        tc.block = bitcal::bit_block<Bits>::from_words(std::span<const std::uint64_t>(tc.words.data(), tc.words.size()));
        cases.push_back(tc);
    }

    return cases;
}

}  // namespace bitcal::test
