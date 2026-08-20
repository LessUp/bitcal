#include <cstdint>

#include <array>
#include <iostream>

#include <bitcal/bitcal.hpp>

int main() {
    // 两个 256 位值：各由四个 64 位字构造。
    const std::array<std::uint64_t, 4> lhs_words{0xF0F0F0F0F0F0F0F0ULL, 0x0ULL, 0x0ULL, 0x0ULL};
    const std::array<std::uint64_t, 4> rhs_words{0xFFFFFFFFFFFFFFFFULL, 0x0ULL, 0x0ULL, 0x0ULL};

    const auto lhs = bitcal::bit_block<256>::from_words(lhs_words);
    const auto rhs = bitcal::bit_block<256>::from_words(rhs_words);

    // AND：两侧都置位的位保留。
    const auto and_result = bitcal::bit_and(lhs, rhs);
    // OR：任一侧置位的位保留。
    const auto or_result = bitcal::bit_or(lhs, rhs);
    // ANDNOT：lhs & ~rhs（清除 lhs 中在 rhs 置位的位）。
    const auto andnot_result = bitcal::bit_andnot(lhs, rhs);

    std::cout << "BitCal basic usage\n";
    std::cout << "word_count: " << decltype(and_result)::word_count << "\n";
    std::cout << "lhs[0]      = 0x" << std::hex << lhs.word(0) << "\n";
    std::cout << "rhs[0]      = 0x" << rhs.word(0) << "\n";
    std::cout << "and[0]      = 0x" << and_result.word(0) << "\n";
    std::cout << "or[0]       = 0x" << or_result.word(0) << "\n";
    std::cout << "andnot[0]   = 0x" << andnot_result.word(0) << "\n";
    std::cout << "popcount(lhs) = " << std::dec << bitcal::popcount(lhs.view()) << "\n";
    std::cout << "is_zero(andnot)? " << (bitcal::is_zero(andnot_result.view()) ? "yes" : "no") << "\n";

    // 将 lhs 左移 65 位（跨 64 位字边界）。
    const auto shifted = bitcal::shift_left(lhs, 65);
    std::cout << "shift_left(lhs, 65)[1] = 0x" << std::hex << shifted.word(1) << "\n";

    return 0;
}
