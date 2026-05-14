#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>

#include <bitcal/bitcal.hpp>

static int g_pass = 0;
static int g_fail = 0;

#define ASSERT_EQ(a, b)                                                                                        \
    do {                                                                                                       \
        if ((a) != (b)) {                                                                                      \
            std::cerr << "  FAIL: " << #a << " == " << #b << " (got " << (a) << " vs " << (b) << ")" << " at " \
                      << __FILE__ << ":" << __LINE__ << std::endl;                                             \
            return false;                                                                                      \
        }                                                                                                      \
    } while (0)

#define ASSERT_TRUE(expr)                                                                           \
    do {                                                                                            \
        if (!(expr)) {                                                                              \
            std::cerr << "  FAIL: " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false;                                                                           \
        }                                                                                           \
    } while (0)

#define RUN_TEST(func)                         \
    do {                                       \
        std::cout << "  " << #func << " ... "; \
        if (func()) {                          \
            std::cout << "PASS" << std::endl;  \
            ++g_pass;                          \
        } else {                               \
            std::cout << "FAIL" << std::endl;  \
            ++g_fail;                          \
        }                                      \
    } while (0)

static_assert(std::is_default_constructible_v<bitcal::bit_block<256>>);

bool test_vnext_block_view_smoke() {
    bitcal::bit_block<256> block;
    auto view = block.view();

    ASSERT_EQ(view.word_count(), std::size_t{4});
    ASSERT_TRUE(view.data() != nullptr);
    return true;
}

bool test_vnext_algorithm_smoke() {
    bitcal::bit_block<256> lhs;
    bitcal::bit_block<256> rhs;

    auto out = bitcal::bit_and<256>(lhs.view(), rhs.view());

    ASSERT_EQ(decltype(out)::word_count, std::size_t{4});
    ASSERT_EQ(out.word(0), std::uint64_t{0});
    return true;
}

bool test_vnext_is_zero_query() {
    bitcal::bit_block<256> block;

    ASSERT_TRUE(bitcal::is_zero(block.view()));

    auto view = block.view();
    view.data()[0] = 0x1ULL;

    ASSERT_TRUE(!bitcal::is_zero(block.view()));
    return true;
}

bool test_vnext_popcount_counts_bits_across_words() {
    bitcal::bit_block<256> block;
    auto view = block.view();

    view.data()[0] = 0b1011ULL;
    view.data()[3] = 0b1000ULL;

    ASSERT_EQ(bitcal::popcount(block.view()), std::uint64_t{4});
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

    ASSERT_EQ(out.word(0), std::uint64_t{0b0101ULL});
    ASSERT_EQ(out.word(3), std::uint64_t{0xC0ULL});
    return true;
}

bool test_vnext_block_storage_alignment() {
    bitcal::bit_block<256> block;
    const auto address = reinterpret_cast<std::uintptr_t>(block.view().data());

#if BITCAL_ARCH_X86
    ASSERT_EQ(address % std::uintptr_t{32}, std::uintptr_t{0});
#else
    ASSERT_TRUE(address != 0);
#endif

    return true;
}

int main() {
    std::cout << "=== BitCal vNext test suite ===" << std::endl;

    RUN_TEST(test_vnext_block_view_smoke);
    RUN_TEST(test_vnext_algorithm_smoke);
    RUN_TEST(test_vnext_is_zero_query);
    RUN_TEST(test_vnext_popcount_counts_bits_across_words);
    RUN_TEST(test_vnext_and_into_writes_preallocated_output);
    RUN_TEST(test_vnext_block_storage_alignment);

    std::cout << std::endl;
    std::cout << "Passed: " << g_pass << std::endl;
    std::cout << "Failed: " << g_fail << std::endl;

    return g_fail == 0 ? 0 : 1;
}
