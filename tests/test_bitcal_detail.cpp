// Implementation regression test suite for BitCal vNext internal word operations.
//
// This test file directly includes implementation headers (<bitcal/detail/word_ops.hpp>)
// and tests internal implementation details, NOT the public contract.
//
// Purpose:
// - Regression coverage for internal word-level operations
// - Validation that SIMD backend dispatch works correctly across widths
// - Internal invariant checking for detail::*_words functions
//
// NOT in scope:
// - Public API contract validation (see test_bitcal.cpp)
// - Public include seam verification (test_bitcal.cpp uses <bitcal/bitcal.hpp> only)
//
// Justification for retention:
// These tests verify implementation-level correctness of the internal word operations
// that underpin the public algorithms. They provide valuable regression coverage for
// backend dispatch and SIMD correctness without being part of the public contract.

#include <cstdint>
#include <iostream>

#include "support/test_macros.hpp"
#include <bitcal/bit_view.hpp>
#include <bitcal/detail/word_ops.hpp>

static bitcal::test::suite_counters g_counters;

bool test_detail_word_ops_queries_share_one_view_interface() {
    const std::uint64_t words[] = {0b1011ULL, 0ULL, 0ULL, 0b1000ULL};
    const bitcal::const_bit_view view(words, 4);

    BITCAL_ASSERT_TRUE(!bitcal::detail::is_zero_words(view));
    BITCAL_ASSERT_EQ(bitcal::detail::popcount_words(view), std::uint64_t{4});
    return true;
}

bool test_detail_word_ops_and_words_writes_every_word() {
    std::uint64_t lhs_words[] = {0b1111ULL, 0xFFFF0000FFFF0000ULL, 0xAAAAAAAAAAAAAAAAULL, 0x0F0F0F0F0F0F0F0FULL};
    std::uint64_t rhs_words[] = {0b0101ULL, 0x00FF00FF00FF00FFULL, 0xFF00FF00FF00FF00ULL, 0x3333333333333333ULL};
    std::uint64_t out_words[] = {~0ULL, ~0ULL, ~0ULL, ~0ULL};

    bitcal::detail::and_words(bitcal::const_bit_view(lhs_words, 4), bitcal::const_bit_view(rhs_words, 4),
                              bitcal::bit_view(out_words, 4));

    BITCAL_ASSERT_EQ(out_words[0], std::uint64_t{0b0101ULL});
    BITCAL_ASSERT_EQ(out_words[1], std::uint64_t{0x00FF000000FF0000ULL});
    BITCAL_ASSERT_EQ(out_words[2], std::uint64_t{0xAA00AA00AA00AA00ULL});
    BITCAL_ASSERT_EQ(out_words[3], std::uint64_t{0x0303030303030303ULL});
    return true;
}

bool test_detail_word_ops_or_words_writes_every_word() {
    std::uint64_t lhs_words[] = {0x0FULL, 0xF000ULL, 0xFF00FF00ULL, 0x0ULL};
    std::uint64_t rhs_words[] = {0xF0ULL, 0x0FF0ULL, 0x00FF00FFULL, 0x1234ULL};
    std::uint64_t out_words[] = {0ULL, 0ULL, 0ULL, 0ULL};

    bitcal::detail::or_words(bitcal::const_bit_view(lhs_words, 4), bitcal::const_bit_view(rhs_words, 4),
                             bitcal::bit_view(out_words, 4));

    BITCAL_ASSERT_EQ(out_words[0], std::uint64_t{0xFFULL});
    BITCAL_ASSERT_EQ(out_words[1], std::uint64_t{0xFFF0ULL});
    BITCAL_ASSERT_EQ(out_words[2], std::uint64_t{0xFFFFFFFFULL});
    BITCAL_ASSERT_EQ(out_words[3], std::uint64_t{0x1234ULL});
    return true;
}

bool test_detail_word_ops_equals_words_detects_match_and_mismatch() {
    const std::uint64_t lhs_words[] = {1ULL, 2ULL, 3ULL, 4ULL};
    const std::uint64_t same_words[] = {1ULL, 2ULL, 3ULL, 4ULL};
    const std::uint64_t other_words[] = {1ULL, 2ULL, 3ULL, 5ULL};

    BITCAL_ASSERT_TRUE(bitcal::detail::equals_words(bitcal::const_bit_view(lhs_words, 4),
                                                    bitcal::const_bit_view(same_words, 4)));
    BITCAL_ASSERT_TRUE(!bitcal::detail::equals_words(bitcal::const_bit_view(lhs_words, 4),
                                                     bitcal::const_bit_view(other_words, 4)));
    return true;
}

int main() {
    std::cout << "=== BitCal detail word ops test suite ===" << std::endl;

    bitcal::test::run_case(g_counters, "test_detail_word_ops_queries_share_one_view_interface",
                           test_detail_word_ops_queries_share_one_view_interface);
    bitcal::test::run_case(g_counters, "test_detail_word_ops_and_words_writes_every_word",
                           test_detail_word_ops_and_words_writes_every_word);
    bitcal::test::run_case(g_counters, "test_detail_word_ops_or_words_writes_every_word",
                           test_detail_word_ops_or_words_writes_every_word);
    bitcal::test::run_case(g_counters, "test_detail_word_ops_equals_words_detects_match_and_mismatch",
                           test_detail_word_ops_equals_words_detects_match_and_mismatch);

    std::cout << std::endl;
    std::cout << "Passed: " << g_counters.pass << std::endl;
    std::cout << "Failed: " << g_counters.fail << std::endl;

    return g_counters.fail == 0 ? 0 : 1;
}
