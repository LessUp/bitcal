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
static_assert(BITCAL_VERSION_MINOR == 0);
static_assert(BITCAL_VERSION_PATCH == 0);
static_assert(BITCAL_VERSION == ((4 << 16) | (0 << 8) | 0));

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

// Constexpr query verification: popcount, is_zero, and equals must be usable
// in constant expressions. This guards the `if consteval` scalar path in
// word_ops.hpp and ensures the AVX2 runtime path does not break constexpr.
constexpr bool test_vnext_public_queries_remain_constexpr() {
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

static_assert(test_vnext_public_queries_remain_constexpr());

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

bool test_vnext_or_into_writes_preallocated_output() {
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

bool test_vnext_xor_into_writes_preallocated_output() {
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

bool test_vnext_andnot_into_writes_preallocated_output() {
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

#if BITCAL_HAS_AVX2
    // 256-bit+ block on AVX2 build: 32-byte alignment for __m256i load/store.
    BITCAL_ASSERT_EQ(address % std::uintptr_t{32}, std::uintptr_t{0});
#else
    // Scalar build: natural alignment only.
    BITCAL_ASSERT_TRUE(address % std::uintptr_t{alignof(std::uint64_t)} == 0);
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
    auto block =
        bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));
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

bool test_vnext_bit_or_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitOrCases128) {
        const auto lhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.lhs.data(), tc.lhs.size()));
        const auto rhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.rhs.data(), tc.rhs.size()));
        const auto out = bitcal::bit_or<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_vnext_bit_xor_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitXorCases128) {
        const auto lhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.lhs.data(), tc.lhs.size()));
        const auto rhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.rhs.data(), tc.rhs.size()));
        const auto out = bitcal::bit_xor<128>(lhs.view(), rhs.view());

        for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
            BITCAL_ASSERT_EQ(out.word(i), tc.expected[i]);
        }
    }

    return true;
}

bool test_vnext_bit_andnot_matches_deterministic_matrix_128() {
    for (const auto& tc : bitcal::test::kBitAndnotCases128) {
        const auto lhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.lhs.data(), tc.lhs.size()));
        const auto rhs =
            bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(tc.rhs.data(), tc.rhs.size()));
        const auto out = bitcal::bit_andnot<128>(lhs.view(), rhs.view());

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
    const auto zero =
        bitcal::bit_block<192>::from_words(std::span<const std::uint64_t>(zero_words.data(), zero_words.size()));
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
    BITCAL_ASSERT_EQ(alignof(bitcal::bit_block<128>), bitcal::get_optimal_alignment<128>());
    BITCAL_ASSERT_EQ(alignof(bitcal::bit_block<256>), bitcal::get_optimal_alignment<256>());
    BITCAL_ASSERT_EQ(alignof(bitcal::bit_block<512>), bitcal::get_optimal_alignment<512>());
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

bool test_vnext_shift_preserves_single_bit_at_width_minus_one_128() {
    // Bit at position 0 shifted left by (Bits-1) should land at the top bit.
    const std::array<std::uint64_t, 2> input_words{1ULL, 0ULL};
    const auto block =
        bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    const auto out_l = bitcal::shift_left<128>(block.view(), 127);
    BITCAL_ASSERT_EQ(out_l.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out_l.word(1), std::uint64_t{0x8000000000000000ULL});

    // Reverse: top bit shifted right by (Bits-1) should land at position 0.
    const std::array<std::uint64_t, 2> top_words{0ULL, 0x8000000000000000ULL};
    const auto top_block =
        bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(top_words.data(), top_words.size()));
    const auto out_r = bitcal::shift_right<128>(top_block.view(), 127);
    BITCAL_ASSERT_EQ(out_r.word(0), std::uint64_t{1});
    BITCAL_ASSERT_EQ(out_r.word(1), std::uint64_t{0});
    return true;
}

bool test_vnext_shift_clears_when_count_exceeds_width_128() {
    // count > Bits should short-circuit to zero, same as count == Bits.
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL};
    const auto block =
        bitcal::bit_block<128>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    const auto out_l = bitcal::shift_left<128>(block.view(), 129);
    const auto out_r = bitcal::shift_right<128>(block.view(), 129);

    for (std::size_t i = 0; i < bitcal::bit_block<128>::word_count; ++i) {
        BITCAL_ASSERT_EQ(out_l.word(i), std::uint64_t{0});
        BITCAL_ASSERT_EQ(out_r.word(i), std::uint64_t{0});
    }
    return true;
}

bool test_vnext_shift_handles_size_max_count_256() {
    // SIZE_MAX is the largest count; must short-circuit, not overflow loops.
    const std::array<std::uint64_t, 4> input_words{0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL,
                                                   0xFFFFFFFFFFFFFFFFULL};
    const auto block =
        bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    const auto out_l = bitcal::shift_left<256>(block.view(), SIZE_MAX);
    const auto out_r = bitcal::shift_right<256>(block.view(), SIZE_MAX);

    for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
        BITCAL_ASSERT_EQ(out_l.word(i), std::uint64_t{0});
        BITCAL_ASSERT_EQ(out_r.word(i), std::uint64_t{0});
    }
    return true;
}

bool test_vnext_shift_zero_is_noop_256() {
    const std::array<std::uint64_t, 4> input_words{0xDEADBEEFULL, 0xCAFEBABEULL, 0x0123456789ABCDEFULL,
                                                   0xFEDCBA9876543210ULL};
    const auto block =
        bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(input_words.data(), input_words.size()));

    const auto out_l = bitcal::shift_left<256>(block.view(), 0);
    const auto out_r = bitcal::shift_right<256>(block.view(), 0);

    for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
        BITCAL_ASSERT_EQ(out_l.word(i), input_words[i]);
        BITCAL_ASSERT_EQ(out_r.word(i), input_words[i]);
    }
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

bool test_public_contract_core_types_accessible_through_umbrella() {
    // Verify that all core public types are accessible through <bitcal/bitcal.hpp>
    // This test is primarily a compile-time check that the umbrella header works

    bitcal::bit_block<128> block;
    bitcal::bit_view mutable_view = block.view();
    bitcal::const_bit_view const_view = block.view();

    BITCAL_ASSERT_EQ(mutable_view.word_count(), std::size_t{2});
    BITCAL_ASSERT_EQ(const_view.word_count(), std::size_t{2});
    BITCAL_ASSERT_TRUE(mutable_view.data() != nullptr);
    BITCAL_ASSERT_TRUE(const_view.data() != nullptr);

    return true;
}

bool test_public_contract_all_retained_algorithms_are_accessible() {
    // Runtime smoke + in-place alias correctness: every retained algorithm is
    // callable with the documented argument shape, and the in-place variants
    // produce correct results when out aliases lhs. Compile-time shape is
    // verified by the static_asserts below; this function exercises the
    // runtime path so the test runner has something to invoke.
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

// Compile-time contract: every retained algorithm is invocable with the
// documented argument shape and returns the documented type. This is the
// real "API surface" check; the runtime function above only exercises it.
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

int main() {
    std::cout << "=== BitCal vNext test suite ===" << std::endl;

    bitcal::test::run_case(g_counters, "test_vnext_block_view_smoke", test_vnext_block_view_smoke);
    bitcal::test::run_case(g_counters, "test_vnext_algorithm_smoke", test_vnext_algorithm_smoke);
    bitcal::test::run_case(g_counters, "test_vnext_is_zero_query", test_vnext_is_zero_query);
    bitcal::test::run_case(g_counters, "test_vnext_popcount_counts_bits_across_words",
                           test_vnext_popcount_counts_bits_across_words);
    bitcal::test::run_case(g_counters, "test_vnext_and_into_writes_preallocated_output",
                           test_vnext_and_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_vnext_or_into_writes_preallocated_output",
                           test_vnext_or_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_vnext_xor_into_writes_preallocated_output",
                           test_vnext_xor_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_vnext_andnot_into_writes_preallocated_output",
                           test_vnext_andnot_into_writes_preallocated_output);
    bitcal::test::run_case(g_counters, "test_vnext_bit_or_combines_words_256", test_vnext_bit_or_combines_words_256);
    bitcal::test::run_case(g_counters, "test_vnext_bit_xor_combines_words_256", test_vnext_bit_xor_combines_words_256);
    bitcal::test::run_case(g_counters, "test_vnext_bit_andnot_masks_words_256", test_vnext_bit_andnot_masks_words_256);
    bitcal::test::run_case(g_counters, "test_vnext_block_storage_alignment", test_vnext_block_storage_alignment);
    bitcal::test::run_case(g_counters, "test_vnext_block_word_span_interop", test_vnext_block_word_span_interop);
    bitcal::test::run_case(g_counters, "test_vnext_block_copy_words_to_supports_self_copy",
                           test_vnext_block_copy_words_to_supports_self_copy);
    bitcal::test::run_case(g_counters, "test_vnext_bit_and_matches_deterministic_matrix_128",
                           test_vnext_bit_and_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_vnext_bit_or_matches_deterministic_matrix_128",
                           test_vnext_bit_or_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_vnext_bit_xor_matches_deterministic_matrix_128",
                           test_vnext_bit_xor_matches_deterministic_matrix_128);
    bitcal::test::run_case(g_counters, "test_vnext_bit_andnot_matches_deterministic_matrix_128",
                           test_vnext_bit_andnot_matches_deterministic_matrix_128);
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
    bitcal::test::run_case(g_counters, "test_vnext_shift_preserves_single_bit_at_width_minus_one_128",
                           test_vnext_shift_preserves_single_bit_at_width_minus_one_128);
    bitcal::test::run_case(g_counters, "test_vnext_shift_clears_when_count_exceeds_width_128",
                           test_vnext_shift_clears_when_count_exceeds_width_128);
    bitcal::test::run_case(g_counters, "test_vnext_shift_handles_size_max_count_256",
                           test_vnext_shift_handles_size_max_count_256);
    bitcal::test::run_case(g_counters, "test_vnext_shift_zero_is_noop_256", test_vnext_shift_zero_is_noop_256);
    bitcal::test::run_case(g_counters, "test_vnext_random_bit_and_matches_reference_model_256",
                           test_vnext_random_bit_and_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_vnext_random_queries_match_reference_model_512",
                           test_vnext_random_queries_match_reference_model_512);

    // Public contract verification tests
    bitcal::test::run_case(g_counters, "test_public_contract_core_types_accessible_through_umbrella",
                           test_public_contract_core_types_accessible_through_umbrella);
    bitcal::test::run_case(g_counters, "test_public_contract_all_retained_algorithms_are_accessible",
                           test_public_contract_all_retained_algorithms_are_accessible);

    std::cout << std::endl;
    std::cout << "Passed: " << g_counters.pass << std::endl;
    std::cout << "Failed: " << g_counters.fail << std::endl;

    return g_counters.fail == 0 ? 0 : 1;
}
