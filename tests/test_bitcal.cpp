#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <type_traits>

#include "support/deterministic_cases.hpp"
#include "support/test_macros.hpp"
#include <bitcal/bitcal.hpp>

static_assert(std::is_default_constructible_v<bitcal::bit_block<256>>);

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

int main() {
    std::cout << "=== BitCal vNext test suite ===" << std::endl;

    bitcal::test::run_case(g_counters, "test_vnext_block_view_smoke", test_vnext_block_view_smoke);
    bitcal::test::run_case(g_counters, "test_vnext_algorithm_smoke", test_vnext_algorithm_smoke);
    bitcal::test::run_case(g_counters, "test_vnext_is_zero_query", test_vnext_is_zero_query);
    bitcal::test::run_case(g_counters, "test_vnext_popcount_counts_bits_across_words",
                           test_vnext_popcount_counts_bits_across_words);
    bitcal::test::run_case(g_counters, "test_vnext_and_into_writes_preallocated_output",
                           test_vnext_and_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_vnext_block_storage_alignment", test_vnext_block_storage_alignment);
    bitcal::test::run_case(g_counters, "test_vnext_block_word_span_interop", test_vnext_block_word_span_interop);
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

    std::cout << std::endl;
    std::cout << "Passed: " << g_counters.pass << std::endl;
    std::cout << "Failed: " << g_counters.fail << std::endl;

    return g_counters.fail == 0 ? 0 : 1;
}
