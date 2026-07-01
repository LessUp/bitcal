#pragma once

#include <array>
#include <cstdint>

namespace bitcal::test {

template <std::size_t WordCount>
struct binary_case {
    std::array<std::uint64_t, WordCount> lhs;
    std::array<std::uint64_t, WordCount> rhs;
    std::array<std::uint64_t, WordCount> expected;
};

template <std::size_t WordCount>
struct unary_count_case {
    std::array<std::uint64_t, WordCount> words;
    std::uint64_t expected;
};

inline constexpr std::array<binary_case<2>, 3> kBitAndCases128{{
    {{{0ULL, 0ULL}}, {{0ULL, 0ULL}}, {{0ULL, 0ULL}}},
    {{{0xFFFF0000FFFF0000ULL, 0xAAAAAAAAAAAAAAAAULL}},
     {{0x00FF00FF00FF00FFULL, 0xF0F0F0F0F0F0F0F0ULL}},
     {{0x00FF000000FF0000ULL, 0xA0A0A0A0A0A0A0A0ULL}}},
    {{{1ULL << 63, 1ULL}}, {{1ULL << 63, 0ULL}}, {{1ULL << 63, 0ULL}}},
}};

inline constexpr std::array<binary_case<2>, 3> kBitOrCases128{{
    {{{0ULL, 0ULL}}, {{0ULL, 0ULL}}, {{0ULL, 0ULL}}},
    {{{0xFFFF0000FFFF0000ULL, 0xAAAAAAAAAAAAAAAAULL}},
     {{0x00FF00FF00FF00FFULL, 0xF0F0F0F0F0F0F0F0ULL}},
     {{0xFFFF00FFFFFF00FFULL, 0xFAFAFAFAFAFAFAFAULL}}},
    {{{1ULL << 63, 1ULL}}, {{1ULL << 63, 0ULL}}, {{1ULL << 63, 1ULL}}},
}};

inline constexpr std::array<binary_case<2>, 3> kBitXorCases128{{
    {{{0ULL, 0ULL}}, {{0ULL, 0ULL}}, {{0ULL, 0ULL}}},
    {{{0xFFFF0000FFFF0000ULL, 0xAAAAAAAAAAAAAAAAULL}},
     {{0x00FF00FF00FF00FFULL, 0xF0F0F0F0F0F0F0F0ULL}},
     {{0xFF0000FFFF0000FFULL, 0x5A5A5A5A5A5A5A5AULL}}},
    {{{1ULL << 63, 1ULL}}, {{1ULL << 63, 0ULL}}, {{0ULL, 1ULL}}},
}};

inline constexpr std::array<binary_case<2>, 3> kBitAndnotCases128{{
    {{{0ULL, 0ULL}}, {{0ULL, 0ULL}}, {{0ULL, 0ULL}}},
    {{{0xFFFF0000FFFF0000ULL, 0xAAAAAAAAAAAAAAAAULL}},
     {{0x00FF00FF00FF00FFULL, 0xF0F0F0F0F0F0F0F0ULL}},
     {{0xFF000000FF000000ULL, 0x0A0A0A0A0A0A0A0AULL}}},
    {{{1ULL << 63, 1ULL}}, {{1ULL << 63, 0ULL}}, {{0ULL, 1ULL}}},
}};

inline constexpr std::array<unary_count_case<4>, 3> kPopcountCases256{{
    {{{0ULL, 0ULL, 0ULL, 0ULL}}, 0ULL},
    {{{0xFFFFFFFFFFFFFFFFULL, 0ULL, 0ULL, 0ULL}}, 64ULL},
    {{{0b1011ULL, 0ULL, 0ULL, 0b1000ULL}}, 4ULL},
}};

inline constexpr std::array<binary_case<8>, 2> kBitAndCases512{{
    {{{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL}},
     {{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL}},
     {{0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL}}},
    {{{0xFFFFFFFFFFFFFFFFULL, 0xAAAAAAAAAAAAAAAAULL, 0x1234567890ABCDEFULL, 0x0000FFFF0000FFFFULL,
       0x0F0F0F0F0F0F0F0FULL, 0x8000000000000000ULL, 0x0000000000000001ULL, 0x5555555555555555ULL}},
     {{0x0000000000000000ULL, 0xF0F0F0F0F0F0F0F0ULL, 0xFEDCBA0987654321ULL, 0xFFFF0000FFFF0000ULL,
       0x3333333333333333ULL, 0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, 0xAAAAAAAAAAAAAAAAULL}},
     {{0x0000000000000000ULL, 0xA0A0A0A0A0A0A0A0ULL, 0x1214120880214121ULL, 0x0000000000000000ULL,
       0x0303030303030303ULL, 0x8000000000000000ULL, 0x0000000000000001ULL, 0x0000000000000000ULL}}},
}};

}  // namespace bitcal::test
