#pragma once

#include <cstdint>

#include <array>
#include <random>
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

        tc.lhs = bitcal::bit_block<Bits>::from_words(tc.lhs_words);
        tc.rhs = bitcal::bit_block<Bits>::from_words(tc.rhs_words);
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

        tc.block = bitcal::bit_block<Bits>::from_words(tc.words);
        cases.push_back(tc);
    }

    return cases;
}

template <std::size_t Bits>
struct random_shift_case {
    bitcal::bit_block<Bits> block;
    std::array<std::uint64_t, Bits / 64> words;
    std::size_t count;
};

template <std::size_t Bits>
std::vector<random_shift_case<Bits>> make_random_shift_cases(const std::uint64_t seed, const std::size_t count) {
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<std::size_t> count_dist(0, Bits);
    std::vector<random_shift_case<Bits>> cases;
    cases.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        random_shift_case<Bits> tc{};
        for (std::size_t word = 0; word < Bits / 64; ++word) {
            tc.words[word] = rng();
        }
        tc.block = bitcal::bit_block<Bits>::from_words(tc.words);
        tc.count = count_dist(rng);
        cases.push_back(tc);
    }

    return cases;
}

}  // namespace bitcal::test
