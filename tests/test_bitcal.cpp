#include <cstddef>
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

int main() {
    std::cout << "=== BitCal vNext test suite ===" << std::endl;

    RUN_TEST(test_vnext_block_view_smoke);

    std::cout << std::endl;
    std::cout << "Passed: " << g_pass << std::endl;
    std::cout << "Failed: " << g_fail << std::endl;

    return g_fail == 0 ? 0 : 1;
}
