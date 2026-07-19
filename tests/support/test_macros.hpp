#pragma once

#include <iostream>
#include <utility>

namespace bitcal::test {

struct suite_counters {
    int pass = 0;
    int fail = 0;
};

template <typename Func>
void run_case(suite_counters& counters, const char* name, Func&& func) {
    std::cout << "  " << name << " ... ";
    if (func()) {
        std::cout << "PASS" << std::endl;
        ++counters.pass;
    } else {
        std::cout << "FAIL" << std::endl;
        ++counters.fail;
    }
}

}  // namespace bitcal::test

#define BITCAL_ASSERT_EQ(a, b)                                                                       \
    do {                                                                                             \
        if ((a) != (b)) {                                                                            \
            std::cerr << "  FAIL: " << #a << " == " << #b << " (got " << (a) << " vs " << (b) << ")" \
                      << " at " << __FILE__ << ":" << __LINE__ << std::endl;                         \
            return false;                                                                            \
        }                                                                                            \
    } while (0)

#define BITCAL_ASSERT_TRUE(expr)                                                                    \
    do {                                                                                            \
        if (!(expr)) {                                                                              \
            std::cerr << "  FAIL: " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false;                                                                           \
        }                                                                                           \
    } while (0)
