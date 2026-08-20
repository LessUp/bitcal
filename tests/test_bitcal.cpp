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

// 公开契约验证：核心类型经 umbrella 头文件可访问
static_assert(std::is_same_v<decltype(std::declval<bitcal::bit_block<256>>().view()), bitcal::bit_view>);
static_assert(std::is_same_v<decltype(std::declval<const bitcal::bit_block<256>>().view()), bitcal::const_bit_view>);

// 公开契约验证：版本宏已定义
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

// constexpr 查询验证：popcount、is_zero、equals 必须可在常量表达式中使用。
// 守护 word_ops.hpp 里的 `if consteval` scalar 路径，并确保 AVX2 运行时路径
// 不破坏 constexpr。
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

// constexpr 移位验证：shift_left / shift_right 必须可在常量表达式中使用。
// 融合单遍内核是纯 scalar 循环（无 memcpy/memset），故无需 `if consteval`
// 分叉——这一点与 is_zero / equals 不同。期望值是硬编码的字常量（word 0 为
// 最低有效字）；覆盖词移+位移混合（65）、bit_shift == 0 的纯词移（64）、
// count >= Bits 的短路（256）。
constexpr bool test_shifts_remain_constexpr() {
    constexpr std::array<std::uint64_t, 4> words{0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL, 0xDEADBEEFCAFEBABEULL,
                                                 0x0F0F0F0F0F0F0F0FULL};
    const auto block = bitcal::bit_block<256>::from_words(words);

    const auto shl65 = bitcal::shift_left(block, 65);
    const auto shr65 = bitcal::shift_right(block, 65);
    const auto shl64 = bitcal::shift_left(block, 64);
    const auto shr64 = bitcal::shift_right(block, 64);
    const auto shl256 = bitcal::shift_left(block, 256);

    return shl65.word(0) == 0 && shl65.word(1) == 0x02468ACF13579BDEULL && shl65.word(2) == 0xFDB97530ECA86420ULL &&
           shl65.word(3) == 0xBD5B7DDF95FD757DULL && shr65.word(0) == 0x7F6E5D4C3B2A1908ULL &&
           shr65.word(1) == 0xEF56DF77E57F5D5FULL && shr65.word(2) == 0x0787878787878787ULL && shr65.word(3) == 0 &&
           shl64.word(0) == 0 && shl64.word(1) == 0x0123456789ABCDEFULL && shl64.word(2) == 0xFEDCBA9876543210ULL &&
           shl64.word(3) == 0xDEADBEEFCAFEBABEULL && shr64.word(0) == 0xFEDCBA9876543210ULL &&
           shr64.word(1) == 0xDEADBEEFCAFEBABEULL && shr64.word(2) == 0x0F0F0F0F0F0F0F0FULL && shr64.word(3) == 0 &&
           shl256.word(0) == 0 && shl256.word(1) == 0 && shl256.word(2) == 0 && shl256.word(3) == 0;
}

static_assert(test_shifts_remain_constexpr());

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

    auto out = bitcal::bit_and(lhs, rhs);

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

    const auto out = bitcal::bit_or(lhs, rhs);

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

    const auto out = bitcal::bit_xor(lhs, rhs);

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

    const auto out = bitcal::bit_andnot(lhs, rhs);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0xF00FULL});
    return true;
}

bool test_block_storage_alignment() {
    bitcal::bit_block<256> block;
    const auto address = reinterpret_cast<std::uintptr_t>(block.view().data());

#if BITCAL_HAS_AVX2
    // AVX2 构建下 256 位及以上块：32 字节对齐是存储保证
    // （内核用 unaligned load，故非正确性前提）。
    BITCAL_ASSERT_EQ(address % std::uintptr_t{32}, std::uintptr_t{0});
#else
    // Scalar 构建：仅自然对齐。
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
        const auto out = bitcal::bit_and(lhs, rhs);

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
        const auto out = bitcal::bit_or(lhs, rhs);

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
        const auto out = bitcal::bit_xor(lhs, rhs);

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
        const auto out = bitcal::bit_andnot(lhs, rhs);

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
        const auto out = bitcal::bit_and(lhs, rhs);

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

    const auto out = bitcal::shift_left(block, 65);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{2});
    return true;
}

bool test_shift_left_clears_when_count_reaches_width_128() {
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0x1ULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out = bitcal::shift_left(block, 128);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{0});
    return true;
}

bool test_shift_right_moves_bits_across_words_128() {
    const std::array<std::uint64_t, 2> input_words{0ULL, 2ULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out = bitcal::shift_right(block, 65);

    BITCAL_ASSERT_EQ(out.word(0), std::uint64_t{1});
    BITCAL_ASSERT_EQ(out.word(1), std::uint64_t{0});
    return true;
}

bool test_shift_preserves_single_bit_at_width_minus_one_128() {
    const std::array<std::uint64_t, 2> input_words{1ULL, 0ULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out_l = bitcal::shift_left(block, 127);
    BITCAL_ASSERT_EQ(out_l.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out_l.word(1), std::uint64_t{0x8000000000000000ULL});

    const std::array<std::uint64_t, 2> top_words{0ULL, 0x8000000000000000ULL};
    const auto top_block = bitcal::bit_block<128>::from_words(top_words);
    const auto out_r = bitcal::shift_right(top_block, 127);
    BITCAL_ASSERT_EQ(out_r.word(0), std::uint64_t{1});
    BITCAL_ASSERT_EQ(out_r.word(1), std::uint64_t{0});
    return true;
}

bool test_shift_clears_when_count_exceeds_width_128() {
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out_l = bitcal::shift_left(block, 129);
    const auto out_r = bitcal::shift_right(block, 129);

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

    const auto out_l = bitcal::shift_left(block, SIZE_MAX);
    const auto out_r = bitcal::shift_right(block, SIZE_MAX);

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

    const auto out_l = bitcal::shift_left(block, 0);
    const auto out_r = bitcal::shift_right(block, 0);

    for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
        BITCAL_ASSERT_EQ(out_l.word(i), input_words[i]);
        BITCAL_ASSERT_EQ(out_r.word(i), input_words[i]);
    }
    return true;
}

bool test_shift_bit_shift_extremes_128() {
    // 词内 bit_shift 的两种极端：count==1（最小）与 count==63（最大，carry 走 >>1 / <<1）。
    // 非对称字 {全 1, 0xA...A} 让 carry 项与主源词的贡献可区分。
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0xAAAAAAAAAAAAAAAAULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto l1 = bitcal::shift_left(block, 1);
    BITCAL_ASSERT_EQ(l1.word(0), 0xFFFFFFFFFFFFFFFEULL);
    BITCAL_ASSERT_EQ(l1.word(1), 0x5555555555555555ULL);

    const auto r1 = bitcal::shift_right(block, 1);
    BITCAL_ASSERT_EQ(r1.word(0), 0x7FFFFFFFFFFFFFFFULL);
    BITCAL_ASSERT_EQ(r1.word(1), 0x5555555555555555ULL);

    const auto l63 = bitcal::shift_left(block, 63);
    BITCAL_ASSERT_EQ(l63.word(0), 0x8000000000000000ULL);
    BITCAL_ASSERT_EQ(l63.word(1), 0x7FFFFFFFFFFFFFFFULL);

    const auto r63 = bitcal::shift_right(block, 63);
    BITCAL_ASSERT_EQ(r63.word(0), 0x5555555555555555ULL);
    BITCAL_ASSERT_EQ(r63.word(1), std::uint64_t{1});
    return true;
}

bool test_shift_word_boundary_no_bit_shift_128() {
    // count == 64：纯词移、bit_shift == 0，锁定位移进位项的短路路径（避免 >>64 / <<64 UB）。
    const std::array<std::uint64_t, 2> input_words{0xFFFFFFFFFFFFFFFFULL, 0xAAAAAAAAAAAAAAAAULL};
    const auto block = bitcal::bit_block<128>::from_words(input_words);

    const auto out_l = bitcal::shift_left(block, 64);
    BITCAL_ASSERT_EQ(out_l.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(out_l.word(1), 0xFFFFFFFFFFFFFFFFULL);

    const auto out_r = bitcal::shift_right(block, 64);
    BITCAL_ASSERT_EQ(out_r.word(0), 0xAAAAAAAAAAAAAAAAULL);
    BITCAL_ASSERT_EQ(out_r.word(1), std::uint64_t{0});
    return true;
}

bool test_random_bit_and_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0xB17CA1ULL, 64)) {
        const auto actual = bitcal::bit_and(tc.lhs, tc.rhs);
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
        const auto actual = bitcal::bit_or(tc.lhs, tc.rhs);
        const auto expected = bitcal::test::reference_bit_or<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_bit_xor_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0x5EEDULL, 64)) {
        const auto actual = bitcal::bit_xor(tc.lhs, tc.rhs);
        const auto expected = bitcal::test::reference_bit_xor<256>(tc.lhs_words, tc.rhs_words);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_bit_andnot_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_binary_cases<256>(0xA11EULL, 64)) {
        const auto actual = bitcal::bit_andnot(tc.lhs, tc.rhs);
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

    [[maybe_unused]] auto and_result = bitcal::bit_and(lhs, rhs);
    [[maybe_unused]] auto or_result = bitcal::bit_or(lhs, rhs);
    [[maybe_unused]] auto xor_result = bitcal::bit_xor(lhs, rhs);
    [[maybe_unused]] auto andnot_result = bitcal::bit_andnot(lhs, rhs);

    // 原地别名：out == lhs。验证别名后的结果与非别名返回形态一致，
    // 使别名正确性有回归覆盖。
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

    [[maybe_unused]] auto shift_l = bitcal::shift_left(lhs, 10);
    [[maybe_unused]] auto shift_r = bitcal::shift_right(lhs, 10);

    return true;
}

// 编译期 API 契约：返回类型与文档一致（span 形态宽度从 extent 推导，无需显式模板参数）
static_assert(std::is_same_v<decltype(bitcal::bit_and(std::declval<std::span<const std::uint64_t, 4>>(),
                                                      std::declval<std::span<const std::uint64_t, 4>>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::bit_or(std::declval<std::span<const std::uint64_t, 4>>(),
                                                     std::declval<std::span<const std::uint64_t, 4>>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::bit_xor(std::declval<std::span<const std::uint64_t, 4>>(),
                                                      std::declval<std::span<const std::uint64_t, 4>>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::bit_andnot(std::declval<std::span<const std::uint64_t, 4>>(),
                                                         std::declval<std::span<const std::uint64_t, 4>>())),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::shift_left(std::declval<std::span<const std::uint64_t, 4>>(), 0)),
                             bitcal::bit_block<256>>);
static_assert(std::is_same_v<decltype(bitcal::shift_right(std::declval<std::span<const std::uint64_t, 4>>(), 0)),
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

// bit_block 重载与 span 形态值等价（运行时：AVX2 路径非 constexpr）。
// span 形态直接传 std::array，同时覆盖隐式 array -> span 转换。
bool test_block_overload_matches_span_form() {
    const std::array<std::uint64_t, 4> a_words{0xF0F0F0F0F0F0F0F0ULL, 0x0F0F0F0F0F0F0F0FULL, 0, 0};
    const std::array<std::uint64_t, 4> b_words{0xFFFFFFFFFFFFFFFFULL, 0xAAAAAAAAAAAAAAAAULL, 0, 0};

    const auto a = bitcal::bit_block<256>::from_words(a_words);
    const auto b = bitcal::bit_block<256>::from_words(b_words);

    const auto via_span_and = bitcal::bit_and(a_words, b_words);
    const auto via_block_and = bitcal::bit_and(a, b);
    const auto via_span_or = bitcal::bit_or(a_words, b_words);
    const auto via_block_or = bitcal::bit_or(a, b);
    const auto via_span_xor = bitcal::bit_xor(a_words, b_words);
    const auto via_block_xor = bitcal::bit_xor(a, b);
    const auto via_span_andnot = bitcal::bit_andnot(a_words, b_words);
    const auto via_block_andnot = bitcal::bit_andnot(a, b);

    const auto via_span_shl = bitcal::shift_left(a_words, 65);
    const auto via_block_shl = bitcal::shift_left(a, 65);
    const auto via_span_shr = bitcal::shift_right(a_words, 65);
    const auto via_block_shr = bitcal::shift_right(a, 65);

    return bitcal::equals(via_span_and.view(), via_block_and.view()) &&
           bitcal::equals(via_span_or.view(), via_block_or.view()) &&
           bitcal::equals(via_span_xor.view(), via_block_xor.view()) &&
           bitcal::equals(via_span_andnot.view(), via_block_andnot.view()) &&
           bitcal::equals(via_span_shl.view(), via_block_shl.view()) &&
           bitcal::equals(via_span_shr.view(), via_block_shr.view());
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

    // AND：word 0 与全 1 相与保持不变；word 1 与 0 相与 -> 0
    BITCAL_ASSERT_EQ(and_result.word(0), 0xDEADBEEFCAFEBABEULL);
    BITCAL_ASSERT_EQ(and_result.word(1), std::uint64_t{0});
    // OR：word 0 与全 1 相或 -> 全 1；word 1 与 0 相或保留 a.word(1)
    BITCAL_ASSERT_EQ(or_result.word(0), 0xFFFFFFFFFFFFFFFFULL);
    BITCAL_ASSERT_EQ(or_result.word(1), 0x0123456789ABCDEFULL);
    // XOR：word 0 与全 1 异或 -> a.word(0) 按位取反
    BITCAL_ASSERT_EQ(xor_result.word(0), ~0xDEADBEEFCAFEBABEULL);
    // ANDNOT（a & ~b）：word 0 -> a & ~全1 = 0；word 1 -> a & ~0 = a
    BITCAL_ASSERT_EQ(andnot_result.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(andnot_result.word(1), 0x0123456789ABCDEFULL);
    // shift_left 64：word 0 -> 0，word 1 -> 旧 word 0
    BITCAL_ASSERT_EQ(shl_result.word(0), std::uint64_t{0});
    BITCAL_ASSERT_EQ(shl_result.word(1), 0xDEADBEEFCAFEBABEULL);
    // shift_right 64：word 0 -> 旧 word 1，word 1 -> 0
    BITCAL_ASSERT_EQ(shr_result.word(0), 0x0123456789ABCDEFULL);
    BITCAL_ASSERT_EQ(shr_result.word(1), std::uint64_t{0});
    return true;
}

bool test_random_shift_left_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_shift_cases<256>(0x51F7ULL, 64)) {
        const auto actual = bitcal::shift_left(tc.block, tc.count);
        const auto expected = bitcal::test::reference_shift_left<256>(tc.words, tc.count);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_shift_right_matches_reference_model_256() {
    for (const auto& tc : bitcal::test::make_random_shift_cases<256>(0x5B17ULL, 64)) {
        const auto actual = bitcal::shift_right(tc.block, tc.count);
        const auto expected = bitcal::test::reference_shift_right<256>(tc.words, tc.count);

        for (std::size_t i = 0; i < bitcal::bit_block<256>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_shift_left_matches_reference_model_512() {
    // 512 位 = 8 词块，覆盖 word_shift 达 7 的多词链（256 位随机对照最大只到 word_shift <= 3）。
    for (const auto& tc : bitcal::test::make_random_shift_cases<512>(0x5121ULL, 64)) {
        const auto actual = bitcal::shift_left(tc.block, tc.count);
        const auto expected = bitcal::test::reference_shift_left<512>(tc.words, tc.count);

        for (std::size_t i = 0; i < bitcal::bit_block<512>::word_count; ++i) {
            BITCAL_ASSERT_EQ(actual.word(i), expected[i]);
        }
    }

    return true;
}

bool test_random_shift_right_matches_reference_model_512() {
    for (const auto& tc : bitcal::test::make_random_shift_cases<512>(0x5122ULL, 64)) {
        const auto actual = bitcal::shift_right(tc.block, tc.count);
        const auto expected = bitcal::test::reference_shift_right<512>(tc.words, tc.count);

        for (std::size_t i = 0; i < bitcal::bit_block<512>::word_count; ++i) {
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
    bitcal::test::run_case(g_counters, "test_shift_bit_shift_extremes_128", test_shift_bit_shift_extremes_128);
    bitcal::test::run_case(g_counters, "test_shift_word_boundary_no_bit_shift_128",
                           test_shift_word_boundary_no_bit_shift_128);
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
    bitcal::test::run_case(g_counters, "test_block_overload_matches_span_form", test_block_overload_matches_span_form);
    bitcal::test::run_case(g_counters, "test_random_shift_left_matches_reference_model_256",
                           test_random_shift_left_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_random_shift_right_matches_reference_model_256",
                           test_random_shift_right_matches_reference_model_256);
    bitcal::test::run_case(g_counters, "test_random_shift_left_matches_reference_model_512",
                           test_random_shift_left_matches_reference_model_512);
    bitcal::test::run_case(g_counters, "test_random_shift_right_matches_reference_model_512",
                           test_random_shift_right_matches_reference_model_512);

    std::cout << std::endl;
    std::cout << "Passed: " << g_counters.pass << std::endl;
    std::cout << "Failed: " << g_counters.fail << std::endl;

    return g_counters.fail == 0 ? 0 : 1;
}
