#include <cstdint>
#include <iostream>

#include <bitcal/bit_view.hpp>
#include <bitcal/detail/word_ops.hpp>

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

bool test_detail_word_ops_queries_share_one_view_interface() {
    const std::uint64_t words[] = {0b1011ULL, 0ULL, 0ULL, 0b1000ULL};
    const bitcal::const_bit_view view(words, 4);

    ASSERT_TRUE(!bitcal::detail::is_zero_words(view));
    ASSERT_EQ(bitcal::detail::popcount_words(view), std::uint64_t{4});
    return true;
}

bool test_detail_word_ops_and_words_writes_every_word() {
    std::uint64_t lhs_words[] = {0b1111ULL, 0xFFFF0000FFFF0000ULL, 0xAAAAAAAAAAAAAAAAULL, 0x0F0F0F0F0F0F0F0FULL};
    std::uint64_t rhs_words[] = {0b0101ULL, 0x00FF00FF00FF00FFULL, 0xFF00FF00FF00FF00ULL, 0x3333333333333333ULL};
    std::uint64_t out_words[] = {~0ULL, ~0ULL, ~0ULL, ~0ULL};

    bitcal::detail::and_words(bitcal::const_bit_view(lhs_words, 4), bitcal::const_bit_view(rhs_words, 4),
                              bitcal::bit_view(out_words, 4));

    ASSERT_EQ(out_words[0], std::uint64_t{0b0101ULL});
    ASSERT_EQ(out_words[1], std::uint64_t{0x00FF000000FF0000ULL});
    ASSERT_EQ(out_words[2], std::uint64_t{0xAA00AA00AA00AA00ULL});
    ASSERT_EQ(out_words[3], std::uint64_t{0x0303030303030303ULL});
    return true;
}

int main() {
    std::cout << "=== BitCal detail word ops test suite ===" << std::endl;

    RUN_TEST(test_detail_word_ops_queries_share_one_view_interface);
    RUN_TEST(test_detail_word_ops_and_words_writes_every_word);

    std::cout << std::endl;
    std::cout << "Passed: " << g_pass << std::endl;
    std::cout << "Failed: " << g_fail << std::endl;

    return g_fail == 0 ? 0 : 1;
}
