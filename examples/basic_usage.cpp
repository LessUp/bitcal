#include <iostream>

#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto out = bitcal::bit_and<256>(lhs.view(), rhs.view());

    std::cout << "BitCal vNext basic usage" << std::endl;
    std::cout << "word_count: " << decltype(out)::word_count << std::endl;
    std::cout << "word[0]: " << out.word(0) << std::endl;

    return 0;
}
