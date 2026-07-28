#include "support/deterministic_cases.hpp"
#include "support/random_cases.hpp"
#include "support/reference_model.hpp"
#include "support/test_macros.hpp"

#include <cstddef>
#include <cstdint>

#include <array>
#include <iostream>
#include <span>
#include <type_traits>

#include <bitcal/bitcal.hpp>

static_assert(std::is_default_constructible_v<bitcal::bit_block<256>>);

// Public contract verification: core types are accessible through the umbrella header
static_assert(std::is_same_v<decltype(std::declval<bitcal::bit_block<256>>().view()), bitcal::bit_view>);
static_assert(std::is_same_v<decltype(std::declval<const bitcal::bit_block<256>>().view()), bitcal::const_bit_view>);

// Public contract verification: version macros are defined
static_assert(BITCAL_VERSION_MAJOR == 4);
static_assert(BITCAL_VERSION_MINOR == 1);
static_assert(BITCAL_VERSION_PATCH == 0);
static_assert(BITCAL_VERSION == ((4 << 16) | (1 << 8) | 0));

constexpr bool test_block_word_roundtrip_is_constexpr() {
    constexpr std::array<std::uint64_t, 4> input_words{0x1ULL, 0x2ULL, 0x3ULL, 0x4ULL};
    const auto block = bitcal::bit_block<256>::from_words(input_words);

    std::array<std::uint64_t, 4> output_words{};
    block.copy_words_to(output_words);

    return output_words[0] == input_words[0] && output_words[1] == input_words[1] &&
           output_words[2] == input_words[2] && output_words[3] == input_words[3];
}

static_assert(test_block_word_roundtrip_is_constexpr());

// Constexpr query verification: popcount, is_zero, and equals must be usable
// in constant expressions. This guards the `if consteval` scalar path in
// word_ops.hpp and ensures the AVX2 runtime path does not break constexpr.
constexpr bool test_public_queries_remain_constexpr() {
    constexpr std::array<std::uint64_t, 4> lhs_words{0x1ULL, 0x0ULL, 0xFULL, 0x8000000000000000ULL};
    constexpr std::array<std::uint64_t, 4> same_words{0x1ULL, 0x0ULL, 0xFULL, 0x8000000000000000ULL};
    constexpr std::array<std::uint64_t, 4> other_words{0x1ULL, 0x0ULL, 0xEULL, 0x8000000000000000ULL};
    constexpr std::array<std::uint64_t, 4> zero_words{};

    const auto lhs = bitcal::const_bit_view(lhs_words.data(), lhs_words.size());
    const auto same = bitcal::const_bit_view(same_words.data(), same_words.size());
    const auto other = bitcal::const_bit_view(other_words.data(), other_words.size());
    const auto zero = bitcal::const_bit_view(zero_words.data(), zero_words.size());

    return bitcal::popcount(lhs) == 6 && bitcal::is_zero(zero) && !bitcal::is_zero(lhs) && bitcal::equals(lhs, same) &&
           !bitcal::equals(lhs, other);
}

static_assert(test_public_queries_remain_constexpr());

static bitcal::test::suite_counters g_counters;

bool test_block_view_smoke() {
    bitcal::bit_block<256> block;
    auto view = block.view();

    BITCAL_ASSERT_EQ(view.word_count(), std::size_t{4});
    BITCAL_ASSERT_TRUE(view.data() != nullptr);

    // const 重载是另一个 view()，单独走其运行时路径
    const bitcal::bit_block<256>& cblock = block;
    bitcal::const_bit_view cview = cblock.view();
    BITCAL_ASSERT_EQ(cview.word_count(), std::size_t{4});
    BITCAL_ASSERT_TRUE(cview.data() != nullptr);
    return true;
}

bool test_algorithm_smoke() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto out = bitcal::bit_and<256>(lhs.view(), rhs.view());

    BITCAL_ASSERT_EQ(decltype(out)::word_count, std::size_t{4});
    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    return true;
}

bool test_is_zero_query() {
    bitcal::bit_block<256> block;

    BITCAL_ASSERT_TRUE(bitcal::is_zero(block.view()));

    auto view = block.view();
    view.data()[0] = 0x1ULL;

    BITCAL_ASSERT_TRUE(!bitcal::is_zero(block.view()));
    return true;
}

bool test_popcount_counts_bits_across_words() {
    bitcal::bit_block<256> block;
    auto view = block.view();

    view.data()[0] = 0b1011ULL;
    view.data()[3] = 0b1000ULL;

    BITCAL_ASSERT_EQ(bitcal::popcount(block.view()), std::uint64_t{4});
    return true;
}

bool test_and_into_writes_preallocated_output() {
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

bool test_or_into_writes_preallocated_output() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;
    bitcal::bit_block<256> out;

    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    auto out_view = out.view();

    lhs_view.data()[0] = 0x0F0FULL;
    lhs_view.data()[3] = 0xF0ULL;
    rhs_view.data()[0] = 0xF0F0ULL;
    rhs_view.data()[3] = 0x0FULL;
    out_view.data()[0] = 0ULL;
    out_view.data()[3] = 0ULL;

    bitcal::or_into(lhs.view(), rhs.view(), out.view());

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xFFFFULL});
    BITCAL_ASSERT_EQ(out.word(3), std::uint64_t{0xFFULL});
    return true;
}

bool test_xor_into_writes_preallocated_output() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;
    bitcal::bit_block<256> out;

    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    auto out_view = out.view();

    lhs_view.data()[0] = 0xFFFF0000FFFF0000ULL;
    rhs_view.data()[0] = 0x00FF00FF00FF00FFULL;
    out_view.data()[0] = 0ULL;

    bitcal::xor_into(lhs.view(), rhs.view(), out.view());

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xFF0000FFFF0000FFULL});
    return true;
}

bool test_andnot_into_writes_preallocated_output() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;
    bitcal::bit_block<256> out;

    auto lhs_view = lhs.view();
    auto rhs_view = rhs.view();
    auto out_view = out.view();

    lhs_view.data()[0] = 0xFFFFULL;
    rhs_view.data()[0] = 0x0FF0ULL;
    out_view.data()[0] = 0ULL;

    bitcal::andnot_into(lhs.view(), rhs.view(), out.view());

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xF00FULL});
    return true;
}

bool test_bit_or_combines_words_256() {
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

bool test_bit_xor_combines_words_256() {
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

bool test_bit_andnot_masks_words_256() {
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

bool test_block_storage_alignment() {
    bitcal::bit_block<256> block;
    const auto address = reinterpret_cast<std::uintptr_t>(block.view().data());

#if BITCAL_HAS_AVX2
    // 256-bit+ block on AVX2 build: 32-byte alignment for __m256i load/store.
    BITCAL_ASSERT_EQ(address % std::uintptr_t{32}, std::uintptr_t{0});
#else
    // Scalar build: natural alignment only.
    BITCAL_ASSERT_TRUE(address % std::uintptr_t{alignof(std::uint64_t)} == 0);
#endif

    return true;
}

bool test_block_word_span_interop() {
    const std::uint64_t input_words[] = {0x1ULL, 0x2ULL, 0x3ULL, 0x4ULL};
    auto block = bitcal::bit_block<256>::from_words(input_words);

    BITCAL_ASSERT_EQ(block.word(0), std::uint64_t{0x1ULL});
    BITCAL_ASSERT_EQ(block.word(3), std::uint64_t{0x4ULL});

    std::uint64_t output_words[4] = {};
    block.copy_words_to(output_words);

    BITCAL_ASSERT_EQ(output_words[0], std::uint64_t{0x1ULL});
    BITCAL_ASSERT_EQ(output_words[3], std::uint64_t{0x4ULL});
    return true;
}

bool test_block_copy_words_to_supports_self_copy() {
    const std::array<std::uint64_t, 4> input_words{0x10ULL, 0x20ULL, 0x30ULL, 0x40ULL};
    auto block = bitcal::bit_block<256>::from_words(input_words);
    auto view = block.view();

    block.copy_words_to(std::span<std::uint64_t, bitcal::bit_block<256>::word_count>(view.data(), view.word_count()));

    BITCAL_ASSERT_EQ(block.word(0), std::uint64_t{0x10ULL});
    BITCAL_ASSERT_EQ(block.word(1), std::uint64_t{0x20ULL});
    BITCAL_ASSERT_EQ(block.word(2), std::uint64_t{0x30ULL});
    BITCAL_ASSERT_EQ(block.word(3), std::uint64_t{0x40ULL});
    return true;
}

bool test_bit_and_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitAndCases128) {
        const auto lhs = bitcal::bit_block<128>::from_words(tc.lhs);
        const auto rhs = bitcal::bit_block<128>::from_words(tc.rhs);
        const auto out = bitcal::bit_and<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_bit_or_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitOrCases128) {
        const auto lhs = bitcal::bit_block<128>::from_words(tc.lhs);
        const auto rhs = bitcal::bit_block<128>::from_words(tc.rhs);
        const auto out = bitcal::bit_or<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_bit_xor_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitXorCases128) {
        const auto lhs = bitcal::bit_block<128>::from_words(tc.lhs);
        const auto rhs = bitcal::bit_block<128>::from_words(tc.rhs);
        const auto out = bitcal::bit_xor<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_bit_andnot_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitAndnotCases128) {
        const auto lhs = bitcal::bit_block<128>::from_words(tc.lhs);
        const auto rhs = bitcal::bit_block<128>::from_words(tc.rhs);
        const auto out = bitcal::bit_andnot<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_popcount_matches_deterministic_matrix_256() {
    for (const auto& tc : bitcal::test::kPopcountCases256) {
        const auto block = bitcal::bit_block<256>::from_words(tc.words);
        BITCAL_ASSERT_EQ(bitcal::popcount(block.view()), tc.expected);
    }

    return true;
}

bool test_bit_and_matches_deterministic_matrix_512() {
    for (const auto& tc : bitcal::test::kBitAndCases512) {
        const auto lhs = bitcal::bit_block<512>::from_words(tc.lhs);
        const auto rhs = bitcal::bit_block<512>::from_words(tc.rhs);
        const auto out = bitcal::bit_and<512>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<512>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_is_zero_detects_sparse_and_dense_patterns_192() {
    const std::array<std::uint64_t, 3> zero_words{0ULL, 0ULL, 0ULL};
    const std::array<std::uint64_t, 3> sparse_words{0ULL, 1ULL << 17, 0ULL};
    const auto zero = bitcal::bit_block<192>::from_words(zero_words);
    const auto sparse = bitcal::bit_block<192>::from_words(sparse_words);

    BITCAL_ASSERT_TRUE(bitcal::is_zero(zero.view()));
    BITCAL_ASSERT_TRUE(!bitcal::is_zero(sparse.view()));
    return true;
}

bool test_view_word_count_matches_custom_width_192() {
    bitcal::bit_block<192> block;
    const auto view = block.view();
    BITCAL_ASSERT_EQ(bitcal::bit_block<192>::word_count, std::size_t{3});
    BITCAL_ASSERT_EQ(view.word_count(), std::size_t{3});
    return true;
}

bool test_equals_reports_equal_and_not_equal_192() {
    const std::array<std::uint64_t, 3> lhs_words{1ULL, 2ULL, 3ULL};
    const std::array<std::uint64_t, 3> same_words{1ULL, 2ULL, 3ULL};
    const std::array<std::uint64_t, 3> other_words{1ULL, 2ULL, 4ULL};

    BITCAL_ASSERT_TRUE(bitcal::equals(bitcal::const_bit_view(lhs_words.data(), lhs_words.size()),
                                      bitcal::const_bit_view(same_words.data(), same_words.size())));
    BITCAL_ASSERT_TRUE(!bitcal::equals(bitcal::const_bit_view(lhs_words.data(), lhs_words.size()),
                                       bitcal::const_bit_view(other_words.data(), other_words.size())));
    return true;
}

bool test_equals_returns_false_for_mismatched_view_lengths() {
    const std::array<std::uint64_t, 1> short_words{0xA5A5A5A5A5A5A5A5ULL};
    const std::array<std::uint64_t, 2> long_words{0xA5A5A5A5A5A5A5A5ULL, 0xFFFFFFFFFFFFFFFFULL};

    BITCAL_ASSERT_TRUE(!bitcal::equals(bitcal::const_bit_view(short_words.data(), short_words.size()),
                                       bitcal::const_bit_view(long_words.data(), long_words.size())));
    BITCAL_ASSERT_TRUE(!bitcal::equals(bitcal::const_bit_view(long_words.data(), long_words.size()),
                                       bitcal::const_bit_view(short_words.data(), short_words.size())));
    return true;
}

bool test_block_uses_optimal_alignment_per_width() {
    BITCAL_ASSERT_EQ(alignof(bitcal::bit_block<128>), bitcal::get_optimal_alignment<128>());
    BITCAL_ASSERT_EQ(alignof(bitcal::bit_block<256>), bitcal::get_optimal_alignment<256>());
    BITCAL_ASSERT_EQ(alignof(bitcal::bit_block<512>), bitcal::get_optimal_alignment<512>());
    return true;
}

bool test_shift_left_moves_bits_across_words_128() {
    const std::array<std::uint64_t, 2> input_words{1ULL, 0ULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out = bitcal::shift_left<128>(block.view(), 65);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{2});
    return true;
}

bool test_shift_left_clears_when_count_reaches_width_128() {
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0x1ULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out = bitcal::shift_left<128>(block.view(), 128);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{0});
    return true;
}

bool test_shift_right_moves_bits_across_words_128() {
    const std::array<std::uint64_t, 2> input_words{0ULL, 2ULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out = bitcal::shift_right<128>(block.view(), 65);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{1});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{0});
    return true;
}

bool test_shift_preserves_single_bit_at_width_minus_one_128() {
    const std::array<std::uint64_t, 2> input_words{1ULL, 0ULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out_l = bitcal::shift_left<128>(block.view(), 127);
    BITCAL_ASSERT_EQ(out_l.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out_l.word(1), std::uint64_t{0x8000000000000000ULL});

    const std::array<std::uint64_t, 2> top_words{0ULL, 0x8000000000000000ULL};
    const auto top_block = bitcal::bit_block<128>::from_words(top_words);
    const auto out_r = bitcal::shift_right<128>(top_block.view(), 127);
    BITCAL_ASSERT_EQ(out_r.word(0), std::uint64_t{1});
    BITCAL_ASSERT_EQ(out_r.word(1), std::uint64_t{0});
    return true;
}

bool test_shift_clears_when_count_exceeds_width_128() {
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out_l = bitcal::shift_left<128>(block.view(), 129);
    const auto out_r = bitcal::shift_right<128>(block.view(), 129);

    for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
        BITCAL_ASSERT_EQ(out_l.word(i), std::uint64_t{0});
        BITCAL_ASSERT_EQ(out_r.word(i), std::uint64_t{0});
    }
    return true;
}

bool test_shift_handles_size_max_count_256() {
    // SIZE_MAX 不得导致循环溢出，必须走短路分支
    const std::array<std::uint64_t, 4> input_words{0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL,
                                                   0xFFFFFFFFFFFFFFFFULL};
    const auto block = bitcal::bit_block<256>::from_words(input_words);

    const auto out_l = bitcal::shift_left<256>(block.view(), SIZE_MAX);
    const auto out_r = bitcal::shift_right<256>(block.view(), SIZE_MAX);

    for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
        BITCAL_ASSERT_EQ(out_l.word(i), std::uint64_t{0});
        BITCAL_ASSERT_EQ(out_r.word(i), std::uint64_t{0});
    }
    return true;
}

bool test_shift_zero_is_noop_256() {
    const std::array<std::uint64_t, 4> input_words{0xDEADBEEFULL, 0xCAFEBABEULL, 0x0123456789ABCDEFULL,
                                                   0xFEDCBA9876543210ULL};
    const auto block = bitcal::bit_block<256>::from_words(input_words);

    const auto out_l = bitcal::shift_left<256>(block.view(), 0);
    const auto out_r = bitcal::shift_right<256>(block.view(), 0);

    for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
        BITCAL_ASSERT_EQ(out_l.word(i), input_words[i]);
        BITCAL_ASSERT_EQ(out_r.word(i), input_words[i]);
    }
    return true;
}

bool test_random_bit_and_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0xB17CA1ULL, 64)) {
        const auto actual = bitcal::bit_and<256>(tc.lhs.view(), tc.rhs.view());
        const auto expected = bitcal::test::reference_bit_and<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_queries_match_reference_model_512() {
    for (const auto& tc : bitcal::test::make_random_unary_cases<512>(0xC0FFEEULL, 64)) {
        BITCAL_ASSERT_EQ(bitcal::popcount(tc.block.view()), bitcal::test::reference_popcount(tc.words));
        BITCAL_ASSERT_EQ(bitcal::is_zero(tc.block.view()), bitcal::test::reference_is_zero(tc.words));
    }

    return true;
}

bool test_equals_avx2_path_256_and_512() {
    // 256 位（4 word，单个 AVX2 vec 块）：word(0..2) 相同、word(3) 不同，
    // 使块内同时含相等与不等 lane，验证 _mm256_testc_si256 不被部分相等误导。
    {
        const std::array<std::uint64_t, 4> a{0xAAULL, 0xBBULL, 0xCCULL, 0xDDULL};
        const std::array<std::uint64_t, 4> b{0xAAULL, 0xBBULL, 0xCCULL, 0xEEULL};
        const auto ba = bitcal::bit_block<256>::from_words(a);
        const auto bb = bitcal::bit_block<256>::from_words(b);

        BITCAL_ASSERT_TRUE(bitcal::equals(ba.view(), ba.view()));
        BITCAL_ASSERT_TRUE(!bitcal::equals(ba.view(), bb.view()));
    }

    // 512 位（8 word，两个 AVX2 vec 块）：前 4 word 全相同、第二块仅 word(7) 不同，
    // 验证跨 vec 块的不等也能被捕获（第一块全等不会掩盖第二块的不等）。
    {
        const std::array<std::uint64_t, 8> c{1ULL, 2ULL, 3ULL, 4ULL, 5ULL, 6ULL, 7ULL, 8ULL};
        const std::array<std::uint64_t, 8> d{1ULL, 2ULL, 3ULL, 4ULL, 5ULL, 6ULL, 7ULL, 9ULL};
        const auto bc = bitcal::bit_block<512>::from_words(c);
        const auto bd = bitcal::bit_block<512>::from_words(d);

        BITCAL_ASSERT_TRUE(bitcal::equals(bc.view(), bc.view()));
        BITCAL_ASSERT_TRUE(!bitcal::equals(bc.view(), bd.view()));
    }

    return true;
}

bool test_random_equals_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0xE0A1ULL, 64)) {
        // 自比恒真：覆盖 AVX2 全等路径
        BITCAL_ASSERT_TRUE(bitcal::equals(tc.lhs.view(), tc.lhs.view()));
        // 随机 lhs/rhs 几乎必不等：覆盖 AVX2 不等路径，与参考模型一致
        BITCAL_ASSERT_EQ(bitcal::equals(tc.lhs.view(), tc.rhs.view()),
                         bitcal::test::reference_equals<256>(tc.lhs_words, tc.rhs_words));
    }

    return true;
}

bool test_random_bit_or_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0x0B17ULL, 64)) {
        const auto actual = bitcal::bit_or<256>(tc.lhs.view(), tc.rhs.view());
        const auto expected = bitcal::test::reference_bit_or<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_bit_xor_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0x5EEDULL, 64)) {
        const auto actual = bitcal::bit_xor<256>(tc.lhs.view(), tc.rhs.view());
        const auto expected = bitcal::test::reference_bit_xor<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_bit_andnot_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0xA11EULL, 64)) {
        const auto actual = bitcal::bit_andnot<256>(tc.lhs.view(), tc.rhs.view());
        const auto expected = bitcal::test::reference_bit_andnot<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_bit_block_64_basic_ops() {
    const std::array<std::uint64_t, 1> a_words{0xDEADBEEFCAFEBABEULL};
    const std::array<std::uint64_t, 1> b_words{0xFF00FF00FF00FF00ULL};

    const auto a = bitcal::bit_block<64>::from_words(a_words);
    const auto b = bitcal::bit_block<64>::from_words(b_words);

    const auto and_result = bitcal::bit_and(a, b);
    BITCAL_ASSERT_EQ(and_result.word(0), 0xDEADBEEFCAFEBABEULL & 0xFF00FF00FF00FF00ULL);

    const auto or_result = bitcal::bit_or(a, b);
    BITCAL_ASSERT_EQ(or_result.word(0), 0xDEADBEEFCAFEBABEULL | 0xFF00FF00FF00FF00ULL);

    BITCAL_ASSERT_EQ(bitcal::popcount(a.view()), std::uint64_t{46});
    BITCAL_ASSERT_TRUE(!bitcal::is_zero(a.view()));

    const auto shifted = bitcal::shift_left(a, 4);
    BITCAL_ASSERT_EQ(shifted.word(0), 0xDEADBEEFCAFEBABEULL << 4);

    return true;
}

bool test_public_contract_all_retained_algorithms_are_accessible() {
    // 运行时烟雾 + 原地别名正确性：所有保留算法以文档化参数形态可调用，
    // 且 *_into 在 out 别名 lhs 时结果与返回形态一致。
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;
    auto lv = lhs.view();
    lv.data()[0] = 0xFF00FF00FF00FF00ULL;
    auto rv = rhs.view();
    rv.data()[0] = 0x0F0F0F0F0F0F0F0FULL;

    [[maybe_unused]] auto and_result = bitcal::bit_and<256>(lhs.view(), rhs.view());
    [[maybe_unused]] auto or_result = bitcal::bit_or<256>(lhs.view(), rhs.view());
    [[maybe_unused]] auto xor_result = bitcal::bit_xor<256>(lhs.view(), rhs.view());
    [[maybe_unused]] auto andnot_result = bitcal::bit_andnot<256>(lhs.view(), rhs.view());

    // In-place alias: out == lhs. Verify the aliased result matches the
    // non-aliased returning form, so alias correctness has regression cover.
    bitcal::bit_block<256> and_alias = lhs;
    bitcal::and_into(and_alias.view(), rhs.view(), and_alias.view());
    BITCAL_ASSERT_EQ(and_alias.word(0), and_result.word(0));

    bitcal::bit_block<256> or_alias = lhs;
    bitcal::or_into(or_alias.view(), rhs.view(), or_alias.view());
    BITCAL_ASSERT_EQ(or_alias.word(0), or_result.word(0));

    bitcal::bit_block<256> xor_alias = lhs;
    bitcal::xor_into(xor_alias.view(), rhs.view(), xor_alias.view());
    BITCAL_ASSERT_EQ(xor_alias.word(0), xor_result.word(0));

    bitcal::bit_block<256> andnot_alias = lhs;
    bitcal::andnot_into(andnot_alias.view(), rhs.view(), andnot_alias.view());
    BITCAL_ASSERT_EQ(andnot_alias.word(0), andnot_result.word(0));

    [[maybe_unused]] bool zero = bitcal::is_zero(lhs.view());
    [[maybe_unused]] std::uint64_t count = bitcal::popcount(lhs.view());
    [[maybe_unused]] bool equal = bitcal::equals(lhs.view(), rhs.view());

    [[maybe_unused]] auto shift_l = bitcal::shift_left<256>(lhs.view(), 10);
    [[maybe_unused]] auto shift_r = bitcal::shift_right<256>(lhs.view(), 10);

    return true;
}

// 编译期 API 契约：返回类型与文档一致
static_assert(std::is_same_v<decltype(bitcal::bit_and<256>(std::declval<bitcal::const_bit_view>(),
                                                           std::declval<bitcal::const_bit_view>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::bit_or<256>(std::declval<bitcal::const_bit_view>(),
                                                          std::declval<bitcal::const_bit_view>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::bit_xor<256>(std::declval<bitcal::const_bit_view>(),
                                                           std::declval<bitcal::const_bit_view>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::bit_andnot<256>(std::declval<bitcal::const_bit_view>(),
                                                              std::declval<bitcal::const_bit_view>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::shift_left<256>(std::declval<bitcal::const_bit_view>(), 0)),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::shift_right<256>(std::declval<bitcal::const_bit_view>(), 0)),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::is_zero(std::declval<bitcal::const_bit_view>())), bool>);
static_assert(std::is_same_v<decltype(bitcal::popcount(std::declval<bitcal::const_bit_view>())), std::uint64_t>);
static_assert(std::is_same_v<decltype(bitcal::equals(std::declval<bitcal::const_bit_view>(),
                                                     std::declval<bitcal::const_bit_view>())),
                             bool>);

// CTAD 重载：Bits 从 bit_block<Bits> 推导
static_assert(std::is_same_v<decltype(bitcal::bit_and(std::declval<bitcal::bit_block<256>>(),
                                                      std::declval<bitcal::bit_block<256>>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::bit_or(std::declval<bitcal::bit_block<128>>(),
                                                     std::declval<bitcal::bit_block<128>>())),
                             bitcal::bit_block<128>>);
static_assert(std::is_same_v<decltype(bitcal::bit_xor(std::declval<bitcal::bit_block<512>>(),
                                                      std::declval<bitcal::bit_block<512>>())),
                             bitcal::bit_block<512>>);
static_assert(std::is_same_v<decltype(bitcal::bit_andnot(std::declval<bitcal::bit_block<192>>(),
                                                         std::declval<bitcal::bit_block<192>>())),
                             bitcal::bit_block<192>>);
static_assert(
    std::is_same_v<decltype(bitcal::shift_left(std::declval<bitcal::bit_block<256>>(), 0)), bitcal::bit_block<256>>);
static_assert(
    std::is_same_v<decltype(bitcal::shift_right(std::declval<bitcal::bit_block<256>>(), 0)), bitcal::bit_block<256>>);

// CTAD 重载与视图形态值等价（运行时：AVX2 路径非 constexpr）
bool test_ctad_bit_block_overload_matches_view_form() {
    const std::array<std::uint64_t, 4> a_words{0xF0F0F0F0F0F0F0F0ULL, 0x0F0F0F0F0F0F0F0FULL, 0, 0};
    const std::array<std::uint64_t, 4> b_words{0xFFFFFFFFFFFFFFFFULL, 0xAAAAAAAAAAAAAAAAULL, 0, 0};

    const auto a = bitcal::bit_block<256>::from_words(a_words);
    const auto b = bitcal::bit_block<256>::from_words(b_words);

    const auto via_view_and = bitcal::bit_and<256>(a.view(), b.view());
    const auto via_ctad_and = bitcal::bit_and(a, b);
    const auto via_view_or = bitcal::bit_or<256>(a.view(), b.view());
    const auto via_ctad_or = bitcal::bit_or(a, b);
    const auto via_view_xor = bitcal::bit_xor<256>(a.view(), b.view());
    const auto via_ctad_xor = bitcal::bit_xor(a, b);
    const auto via_view_andnot = bitcal::bit_andnot<256>(a.view(), b.view());
    const auto via_ctad_andnot = bitcal::bit_andnot(a, b);

    const auto via_view_shl = bitcal::shift_left<256>(a.view(), 65);
    const auto via_ctad_shl = bitcal::shift_left(a, 65);
    const auto via_view_shr = bitcal::shift_right<256>(a.view(), 65);
    const auto via_ctad_shr = bitcal::shift_right(a, 65);

    return bitcal::equals(via_view_and.view(), via_ctad_and.view()) &&
           bitcal::equals(via_view_or.view(), via_ctad_or.view()) &&
           bitcal::equals(via_view_xor.view(), via_ctad_xor.view()) &&
           bitcal::equals(via_view_andnot.view(), via_ctad_andnot.view()) &&
           bitcal::equals(via_view_shl.view(), via_ctad_shl.view()) &&
           bitcal::equals(via_view_shr.view(), via_ctad_shr.view());
}

bool test_ctad_bit_block_overload_runtime() {
    std::array<std::uint64_t, 4> a_words{0xDEADBEEFCAFEBABEULL, 0x0123456789ABCDEFULL, 0, 0};
    std::array<std::uint64_t, 4> b_words{0xFFFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0, 0};

    bitcal::bit_block<256> a = bitcal::bit_block<256>::from_words(a_words);
    bitcal::bit_block<256> b = bitcal::bit_block<256>::from_words(b_words);

    auto and_result = bitcal::bit_and(a, b);
    auto or_result = bitcal::bit_or(a, b);
    auto xor_result = bitcal::bit_xor(a, b);
    auto andnot_result = bitcal::bit_andnot(a, b);
    auto shl_result = bitcal::shift_left(a, 64);
    auto shr_result = bitcal::shift_right(a, 64);

    // AND with all-ones in word 0 keeps word 0; word 1 AND 0 -> 0
    BITCAL_ASSERT_EQ(and_result.word(0), 0xDEADBEEFCAFEBABEULL);
    BITCAL_ASSERT_EQ(and_result.word(1), std::uint64_t{0});
    // OR with all-ones in word 0 -> all-ones; word 1 OR 0 keeps a.word(1)
    BITCAL_ASSERT_EQ(or_result.word(0), 0xFFFFFFFFFFFFFFFFULL);
    BITCAL_ASSERT_EQ(or_result.word(1), 0x0123456789ABCDEFULL);
    // XOR with all-ones in word 0 -> bitwise NOT of a.word(0)
    BITCAL_ASSERT_EQ(xor_result.word(0), ~0xDEADBEEFCAFEBABEULL);
    // ANDNOT (a & ~b): word 0 -> a & ~all_ones = 0; word 1 -> a & ~0 = a
    BITCAL_ASSERT_EQ(andnot_result.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(andnot_result.word(1), 0x0123456789ABCDEFULL);
    // shift_left by 64: word 0 -> 0, word 1 -> old word 0
    BITCAL_ASSERT_EQ(shl_result.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(shl_result.word(1), 0xDEADBEEFCAFEBABEULL);
    // shift_right by 64: word 0 -> old word 1, word 1 -> 0
    BITCAL_ASSERT_EQ(shr_result.word(0), 0x0123456789ABCDEFULL);
    BITCAL_ASSERT_EQ(shr_result.word(1), std::uint64_t{0});
    return true;
}

bool test_random_shift_left_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_shift_cases<256>(0x51F7ULL, 64)) {
        const auto actual = bitcal::shift_left<256>(tc.block.view(), tc.count);
        const auto expected = bitcal::test::reference_shift_left<256>(tc.words, tc.count);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_shift_right_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_shift_cases<256>(0x5B17ULL, 64)) {
        const auto actual = bitcal::shift_right<256>(tc.block.view(), tc.count);
        const auto expected = bitcal::test::reference_shift_right<256>(tc.words, tc.count);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

int main() {
    std::cout << "=== BitCal test suite ===" << std::endl;

    bitcal::test::run_case(g_counters, "test_block_view_smoke", test_block_view_smoke);
    bitcal::test::run_case(g_counters, "test_algorithm_smoke", test_algorithm_smoke);
    bitcal::test::run_case(g_counters, "test_is_zero_query", test_is_zero_query);
    bitcal::test::run_case(g_counters, "test_popcount_counts_bits_across_words",
                           test_popcount_counts_bits_across_words);
    bitcal::test::run_case(g_counters, "test_and_into_writes_preallocated_output",
                           test_and_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_or_into_writes_preallocated_output",
                           test_or_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_xor_into_writes_preallocated_output",
                           test_xor_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_andnot_into_writes_preallocated_output",
                           test_andnot_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_bit_or_combines_words_256", test_bit_or_combines_words_256);
    bitcal::test::run_case(g_counters, "test_bit_xor_combines_words_256", test_bit_xor_combines_words_256);
    bitcal::test::run_case(g_counters, "test_bit_andnot_masks_words_256", test_bit_andnot_masks_words_256);
    bitcal::test::run_case(g_counters, "test_block_storage_alignment", test_block_storage_alignment);
    bitcal::test::run_case(g_counters, "test_block_word_span_interop", test_block_word_span_interop);
    bitcal::test::run_case(g_counters, "test_block_copy_words_to_supports_self_copy",
                           test_block_copy_words_to_supports_self_copy);
    bitcal::test::run_case(g_counters, "test_bit_and_matches_deterministic_matrix_128",
                           test_bit_and_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_bit_or_matches_deterministic_matrix_128",
                           test_bit_or_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_bit_xor_matches_deterministic_matrix_128",
                           test_bit_xor_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_bit_andnot_matches_deterministic_matrix_128",
                           test_bit_andnot_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_popcount_matches_deterministic_matrix_256",
                           test_popcount_matches_deterministic_matrix_256);
    bitcal::test::run_case(g_counters, "test_bit_and_matches_deterministic_matrix_512",
                           test_bit_and_matches_deterministic_matrix_512);
    bitcal::test::run_case(g_counters, "test_is_zero_detects_sparse_and_dense_patterns_192",
                           test_is_zero_detects_sparse_and_dense_patterns_192);
    bitcal::test::run_case(g_counters, "test_view_word_count_matches_custom_width_192",
                           test_view_word_count_matches_custom_width_192);
    bitcal::test::run_case(g_counters, "test_equals_reports_equal_and_not_equal_192",
                           test_equals_reports_equal_and_not_equal_192);
    bitcal::test::run_case(g_counters, "test_equals_returns_false_for_mismatched_view_lengths",
                           test_equals_returns_false_for_mismatched_view_lengths);
    bitcal::test::run_case(g_counters, "test_block_uses_optimal_alignment_per_width",
                           test_block_uses_optimal_alignment_per_width);
    bitcal::test::run_case(g_counters, "test_shift_left_moves_bits_across_words_128",
                           test_shift_left_moves_bits_across_words_128);
    bitcal::test::run_case(g_counters, "test_shift_left_clears_when_count_reaches_width_128",
                           test_shift_left_clears_when_count_reaches_width_128);
    bitcal::test::run_case(g_counters, "test_shift_right_moves_bits_across_words_128",
                           test_shift_right_moves_bits_across_words_128);
    bitcal::test::run_case(g_counters, "test_shift_preserves_single_bit_at_width_minus_one_128",
                           test_shift_preserves_single_bit_at_width_minus_one_128);
    bitcal::test::run_case(g_counters, "test_shift_clears_when_count_exceeds_width_128",
                           test_shift_clears_when_count_exceeds_width_128);
    bitcal::test::run_case(g_counters, "test_shift_handles_size_max_count_256", test_shift_handles_size_max_count_256);
    bitcal::test::run_case(g_counters, "test_shift_zero_is_noop_256", test_shift_zero_is_noop_256);
    bitcal::test::run_case(g_counters, "test_random_bit_and_matches_reference_model_256",
                           test_random_bit_and_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_random_bit_or_matches_reference_model_256",
                           test_random_bit_or_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_random_bit_xor_matches_reference_model_256",
                           test_random_bit_xor_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_random_bit_andnot_matches_reference_model_256",
                           test_random_bit_andnot_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_random_queries_match_reference_model_512",
                           test_random_queries_match_reference_model_512);
    bitcal::test::run_case(g_counters, "test_equals_avx2_path_256_and_512", test_equals_avx2_path_256_and_512);
    bitcal::test::run_case(g_counters, "test_random_equals_matches_reference_model_256",
                           test_random_equals_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_bit_block_64_basic_ops", test_bit_block_64_basic_ops);

    // 公开契约验证
    bitcal::test::run_case(g_counters, "test_public_contract_all_retained_algorithms_are_accessible",
                           test_public_contract_all_retained_algorithms_are_accessible);
    bitcal::test::run_case(g_counters, "test_ctad_bit_block_overload_runtime", test_ctad_bit_block_overload_runtime);
    bitcal::test::run_case(g_counters, "test_ctad_bit_block_overload_matches_view_form",
                           test_ctad_bit_block_overload_matches_view_form);
    bitcal::test::run_case(g_counters, "test_random_shift_left_matches_reference_model_256",
                           test_random_shift_left_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_random_shift_right_matches_reference_model_256",
                           test_random_shift_right_matches_reference_model_256);

    std::cout << std::endl;
    std::cout << "Passed: " << g_counters.pass << std::endl;
    std::cout << "Failed: " << g_counters.fail << std::endl;

    return g_counters.fail == 0 ? 0 : 1;
}
