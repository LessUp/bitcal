#include <cstddef>
#include <cstdint>
#include <iostream>
#include <array>
#include <span>
#include <type_traits>

#include "support/deterministic_cases.hpp"
#include "support/random_cases.hpp"
#include "support/reference_model.hpp"
#include "support/test_macros.hpp"
#include <bitcal/bitcal.hpp>

static_assert(std::is_default_constructible_v<bitcal::bit_block<256>>);

constexpr bool test_vnext_block_word_roundtrip_is_constexpr() {
    constexpr std::array<std::uint64_t, 4> input_words{0x1ULL, 0x2ULL, 0x3ULL, 0x4ULL};
    const auto block =
        bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    std::array<std::uint64_t, 4> output_words{};
    block.copy_words_to(std::span<std::uint64_t>(output_words.data(), output_words.size()));

    return output_words[0] == input_words[0] && output_words[1] == input_words[1] &&
           output_words[2] == input_words[2] && output_words[3] == input_words[3];
}

static_assert(test_vnext_block_word_roundtrip_is_constexpr());

static bitcal::test::suite_counters g_counters;

bool test_vnext_block_view_smoke() {
    bitcal::bit_block<256> block;
    auto view = block.view();

    BITCAL_ASSERT_EQ(view.word_count(), std::size_t{4});
    BITCAL_ASSERT_TRUE(view.data() != nullptr);
    return true;
}

bool test_vnext_algorithm_smoke() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto out = bitcal::bit_and<256>(lhs.view(), rhs.view());

    BITCAL_ASSERT_EQ(decltype(out)::word_count, std::size_t{4});
    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    return true;
}

bool test_vnext_is_zero_query() {
    bitcal::bit_block<256> block;

    BITCAL_ASSERT_TRUE(bitcal::is_zero(block.view()));

    auto view = block.view();
    view.data()[0] = 0x1ULL;

    BITCAL_ASSERT_TRUE(!bitcal::is_zero(block.view()));
    return true;
}

bool test_vnext_popcount_counts_bits_across_words() {
    bitcal::bit_block<256> block;
    auto view = block.view();

    view.data()[0] = 0b1011ULL;
    view.data()[3] = 0b1000ULL;

    BITCAL_ASSERT_EQ(bitcal::popcount(block.view()), std::uint64_t{4});
    return true;
}

bool test_vnext_and_into_writes_preallocated_output() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;
    bitcal::bit_block<256> out;

    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    auto out_view = out.view();

    lhs_view.data()[0] = 0b1111ULL;
    lhs_view.data()[3] = 0xF0ULL;
    rhs_view.data()[0] = 0b0101ULL;
    rhs_view.data()[3] = 0xCCULL;
    out_view.data()[0] = 0xFFFFFFFFFFFFFFFFULL;
    out_view.data()[3] = 0xFFFFFFFFFFFFFFFFULL;

    bitcal::and_into(lhs.view(), rhs.view(), out.view());

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0b0101ULL});
    BITCAL_ASSERT_EQ(out.word(3), std::uint64_t{0xC0ULL});
    return true;
}

bool test_vnext_bit_or_combines_words_256() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    lhs_view.data()[0] = 0x0F0FULL;
    lhs_view.data()[3] = 0xFF00ULL;
    rhs_view.data()[0] = 0xF0F0ULL;
    rhs_view.data()[3] = 0x00FFULL;

    const auto out = bitcal::bit_or<256>(lhs.view(), rhs.view());

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xFFFFULL});
    BITCAL_ASSERT_EQ(out.word(3), std::uint64_t{0xFFFFULL});
    return true;
}

bool test_vnext_bit_xor_combines_words_256() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    lhs_view.data()[0] = 0xFFFF0000FFFF0000ULL;
    rhs_view.data()[0] = 0x00FF00FF00FF00FFULL;

    const auto out = bitcal::bit_xor<256>(lhs.view(), rhs.view());

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xFF0000FFFF0000FFULL});
    return true;
}

bool test_vnext_bit_andnot_masks_words_256() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    lhs_view.data()[0] = 0xFFFFULL;
    rhs_view.data()[0] = 0x0FF0ULL;

    const auto out = bitcal::bit_andnot<256>(lhs.view(), rhs.view());

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xF00FULL});
    return true;
}

bool test_vnext_block_storage_alignment() {
    bitcal::bit_block<256> block;
    const auto address = reinterpret_cast<std::uintptr_t>(block.view().data());

#if BITCAL_ARCH_X86
    BITCAL_ASSERT_EQ(address % std::uintptr_t{32}, std::uintptr_t{0});
#else
    BITCAL_ASSERT_TRUE(address != 0);
#endif

    return true;
}

bool test_vnext_block_word_span_interop() {
    const std::uint64_t input_words[] = {0x1ULL, 0x2ULL, 0x3ULL, 0x4ULL};
    auto block = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(input_words, 4));

    BITCAL_ASSERT_EQ(block.word(0), std::uint64_t{0x1ULL});
    BITCAL_ASSERT_EQ(block.word(3), std::uint64_t{0x4ULL});

    std::uint64_t output_words[4] = {};
    block.copy_words_to(std::span<std::uint64_t>(output_words, 4));

    BITCAL_ASSERT_EQ(output_words[0], std::uint64_t{0x1ULL});
    BITCAL_ASSERT_EQ(output_words[3], std::uint64_t{0x4ULL});
    return true;
}

bool test_vnext_block_copy_words_to_supports_self_copy() {
    const std::array<std::uint64_t, 4> input_words{0x10ULL, 0x20ULL, 0x30ULL, 0x40ULL};
    auto block = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));
    auto view = block.view();

    block.copy_words_to(std::span<std::uint64_t>(view.data(), view.word_count()));

    BITCAL_ASSERT_EQ(block.word(0), std::uint64_t{0x10ULL});
    BITCAL_ASSERT_EQ(block.word(1), std::uint64_t{0x20ULL});
    BITCAL_ASSERT_EQ(block.word(2), std::uint64_t{0x30ULL});
    BITCAL_ASSERT_EQ(block.word(3), std::uint64_t{0x40ULL});
    return true;
}

bool test_vnext_bit_and_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitAndCases128) {
        const auto lhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.lhs.data(), tc.lhs.size()));
        const auto rhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.rhs.data(), tc.rhs.size()));
        const auto out = bitcal::bit_and<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_vnext_popcount_matches_deterministic_matrix_256() {
    for (const auto& tc : bitcal::test::kPopcountCases256) {
        const auto block =
            bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(tc.words.data(), tc.words.size()));
        BITCAL_ASSERT_EQ(bitcal::popcount(block.view()), tc.expected);
    }

    return true;
}

bool test_vnext_bit_and_matches_deterministic_matrix_512() {
    for (const auto& tc : bitcal::test::kBitAndCases512) {
        const auto lhs =
            bitcal::bit_block<512>::from_words(std::span<const std::uint64_t>(tc.lhs.data(), tc.lhs.size()));
        const auto rhs =
            bitcal::bit_block<512>::from_words(std::span<const std::uint64_t>(tc.rhs.data(), tc.rhs.size()));
        const auto out = bitcal::bit_and<512>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<512>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_vnext_is_zero_detects_sparse_and_dense_patterns_192() {
    const std::array<std::uint64_t, 3> zero_words{0ULL, 0ULL, 0ULL};
    const std::array<std::uint64_t, 3> sparse_words{0ULL, 1ULL << 17, 0ULL};
    const auto zero = bitcal::bit_block<192>::from_words(std::span<const std::uint64_t>(zero_words.data(), zero_words.size()));
    const auto sparse =
        bitcal::bit_block<192>::from_words(std::span<const std::uint64_t>(sparse_words.data(), sparse_words.size()));

    BITCAL_ASSERT_TRUE(bitcal::is_zero(zero.view()));
    BITCAL_ASSERT_TRUE(!bitcal::is_zero(sparse.view()));
    return true;
}

bool test_vnext_view_word_count_matches_custom_width_192() {
    bitcal::bit_block<192> block;
    const auto view = block.view();
    BITCAL_ASSERT_EQ(bitcal::bit_block<192>::word_count, std::size_t{3});
    BITCAL_ASSERT_EQ(view.word_count(), std::size_t{3});
    return true;
}

bool test_vnext_equals_reports_equal_and_not_equal_192() {
    const std::array<std::uint64_t, 3> lhs_words{1ULL, 2ULL, 3ULL};
    const std::array<std::uint64_t, 3> same_words{1ULL, 2ULL, 3ULL};
    const std::array<std::uint64_t, 3> other_words{1ULL, 2ULL, 4ULL};

    BITCAL_ASSERT_TRUE(bitcal::equals(bitcal::const_bit_view(lhs_words.data(), lhs_words.size()),
                                      bitcal::const_bit_view(same_words.data(), same_words.size())));
    BITCAL_ASSERT_TRUE(!bitcal::equals(bitcal::const_bit_view(lhs_words.data(), lhs_words.size()),
                                       bitcal::const_bit_view(other_words.data(), other_words.size())));
    return true;
}

bool test_vnext_equals_returns_false_for_mismatched_view_lengths() {
    const std::array<std::uint64_t, 1> short_words{0xA5A5A5A5A5A5A5A5ULL};
    const std::array<std::uint64_t, 2> long_words{0xA5A5A5A5A5A5A5A5ULL, 0xFFFFFFFFFFFFFFFFULL};

    BITCAL_ASSERT_TRUE(!bitcal::equals(bitcal::const_bit_view(short_words.data(), short_words.size()),
                                       bitcal::const_bit_view(long_words.data(), long_words.size())));
    BITCAL_ASSERT_TRUE(!bitcal::equals(bitcal::const_bit_view(long_words.data(), long_words.size()),
                                       bitcal::const_bit_view(short_words.data(), short_words.size())));
    return true;
}

bool test_vnext_block_uses_optimal_alignment_per_width() {
    BITCAL_ASSERT_EQ(bitcal::bit_block<128>::storage_alignment, bitcal::get_optimal_alignment<128>());
    BITCAL_ASSERT_EQ(bitcal::bit_block<256>::storage_alignment, bitcal::get_optimal_alignment<256>());
    BITCAL_ASSERT_EQ(bitcal::bit_block<512>::storage_alignment, bitcal::get_optimal_alignment<512>());
    return true;
}

bool test_vnext_shift_left_moves_bits_across_words_128() {
    const std::array<std::uint64_t, 2> input_words{1ULL, 0ULL};
    const auto block =
        bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    const auto out = bitcal::shift_left<128>(block.view(), 65);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{2});
    return true;
}

bool test_vnext_shift_left_clears_when_count_reaches_width_128() {
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0x1ULL};
    const auto block =
        bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    const auto out = bitcal::shift_left<128>(block.view(), 128);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{0});
    return true;
}

bool test_vnext_shift_right_moves_bits_across_words_128() {
    const std::array<std::uint64_t, 2> input_words{0ULL, 2ULL};
    const auto block =
        bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    const auto out = bitcal::shift_right<128>(block.view(), 65);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{1});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{0});
    return true;
}

bool test_vnext_random_bit_and_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0xB17CA1ULL, 64)) {
        const auto actual = bitcal::bit_and<256>(tc.lhs.view(), tc.rhs.view());
        const auto expected = bitcal::test::reference_bit_and<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_vnext_random_queries_match_reference_model_512() {
    for (const auto& tc : bitcal::test::make_random_unary_cases<512>(0xC0FFEEULL, 64)) {
        BITCAL_ASSERT_EQ(bitcal::popcount(tc.block.view()), bitcal::test::reference_popcount(tc.words));
        BITCAL_ASSERT_EQ(bitcal::is_zero(tc.block.view()), bitcal::test::reference_is_zero(tc.words));
    }

    return true;
}

int main() {
    std::cout << "=== BitCal vNext test suite ===" << std::endl;

    bitcal::test::run_case(g_counters, "test_vnext_block_view_smoke", test_vnext_block_view_smoke);
    bitcal::test::run_case(g_counters, "test_vnext_algorithm_smoke", test_vnext_algorithm_smoke);
    bitcal::test::run_case(g_counters, "test_vnext_is_zero_query", test_vnext_is_zero_query);
    bitcal::test::run_case(g_counters, "test_vnext_popcount_counts_bits_across_words",
                           test_vnext_popcount_counts_bits_across_words);
    bitcal::test::run_case(g_counters, "test_vnext_and_into_writes_preallocated_output",
                           test_vnext_and_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_vnext_bit_or_combines_words_256", test_vnext_bit_or_combines_words_256);
    bitcal::test::run_case(g_counters, "test_vnext_bit_xor_combines_words_256", test_vnext_bit_xor_combines_words_256);
    bitcal::test::run_case(g_counters, "test_vnext_bit_andnot_masks_words_256",
                           test_vnext_bit_andnot_masks_words_256);
    bitcal::test::run_case(g_counters, "test_vnext_block_storage_alignment", test_vnext_block_storage_alignment);
    bitcal::test::run_case(g_counters, "test_vnext_block_word_span_interop", test_vnext_block_word_span_interop);
    bitcal::test::run_case(g_counters, "test_vnext_block_copy_words_to_supports_self_copy",
                           test_vnext_block_copy_words_to_supports_self_copy);
    bitcal::test::run_case(g_counters, "test_vnext_bit_and_matches_deterministic_matrix_128",
                           test_vnext_bit_and_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_vnext_popcount_matches_deterministic_matrix_256",
                           test_vnext_popcount_matches_deterministic_matrix_256);
    bitcal::test::run_case(g_counters, "test_vnext_bit_and_matches_deterministic_matrix_512",
                           test_vnext_bit_and_matches_deterministic_matrix_512);
    bitcal::test::run_case(g_counters, "test_vnext_is_zero_detects_sparse_and_dense_patterns_192",
                           test_vnext_is_zero_detects_sparse_and_dense_patterns_192);
    bitcal::test::run_case(g_counters, "test_vnext_view_word_count_matches_custom_width_192",
                           test_vnext_view_word_count_matches_custom_width_192);
    bitcal::test::run_case(g_counters, "test_vnext_equals_reports_equal_and_not_equal_192",
                           test_vnext_equals_reports_equal_and_not_equal_192);
    bitcal::test::run_case(g_counters, "test_vnext_equals_returns_false_for_mismatched_view_lengths",
                           test_vnext_equals_returns_false_for_mismatched_view_lengths);
    bitcal::test::run_case(g_counters, "test_vnext_block_uses_optimal_alignment_per_width",
                           test_vnext_block_uses_optimal_alignment_per_width);
    bitcal::test::run_case(g_counters, "test_vnext_shift_left_moves_bits_across_words_128",
                           test_vnext_shift_left_moves_bits_across_words_128);
    bitcal::test::run_case(g_counters, "test_vnext_shift_left_clears_when_count_reaches_width_128",
                           test_vnext_shift_left_clears_when_count_reaches_width_128);
    bitcal::test::run_case(g_counters, "test_vnext_shift_right_moves_bits_across_words_128",
                           test_vnext_shift_right_moves_bits_across_words_128);
    bitcal::test::run_case(g_counters, "test_vnext_random_bit_and_matches_reference_model_256",
                           test_vnext_random_bit_and_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_vnext_random_queries_match_reference_model_512",
                           test_vnext_random_queries_match_reference_model_512);

    std::cout << std::endl;
    std::cout << "Passed: " << g_counters.pass << std::endl;
    std::cout << "Failed: " << g_counters.fail << std::endl;

    return g_counters.fail == 0 ? 0 : 1;
}
