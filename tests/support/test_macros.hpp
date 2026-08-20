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

// --- 自注册测试注册表 --------------------------------------------------------
// BITCAL_TEST_CASE(name) 展开为「前置声明 + 静态注册器 + 函数定义」，测试
// 函数一经定义即被登记，main() 无需（也无法）漏掉任何用例。注册表用静态
// 链表、无动态分配；头指针为函数局部静态，跨 TU 复用也规避静态初始化顺序
// 问题。注册顺序为各 TU 内源码顺序（跨 TU 的顺序未定义，当前仅单 TU）。

using test_fn = bool (*)();

struct test_entry {
    const char* name;
    test_fn func;
    test_entry* next = nullptr;
};

inline test_entry*& registered_tests() {
    static test_entry* head = nullptr;
    return head;
}

inline void append_test(test_entry& entry) noexcept {
    test_entry** tail = &registered_tests();
    while (*tail != nullptr) {
        tail = &(*tail)->next;
    }
    *tail = &entry;
}

class test_registrar {
public:
    test_registrar(const char* name, test_fn func) noexcept : entry_{name, func, nullptr} { append_test(entry_); }

private:
    test_entry entry_;
};

inline int run_all_registered(suite_counters& counters) {
    int registered = 0;
    for (const test_entry* e = registered_tests(); e != nullptr; e = e->next) {
        run_case(counters, e->name, e->func);
        ++registered;
    }
    return registered;
}

}  // namespace bitcal::test

#define BITCAL_TEST_CASE(name)                                                             \
    static bool name();                                                                    \
    [[maybe_unused]] static ::bitcal::test::test_registrar name##_registrar{#name, &name}; \
    static bool name()

#define BITCAL_ASSERT_EQ(a, b)                                                                                 \
    do {                                                                                                       \
        if ((a) != (b)) {                                                                                      \
            std::cerr << "  FAIL: " << #a << " == " << #b << " (got " << (a) << " vs " << (b) << ")" << " at " \
                      << __FILE__ << ":" << __LINE__ << std::endl;                                             \
            return false;                                                                                      \
        }                                                                                                      \
    } while (0)

#define BITCAL_ASSERT_TRUE(expr)                                                                    \
    do {                                                                                            \
        if (!(expr)) {                                                                              \
            std::cerr << "  FAIL: " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false;                                                                           \
        }                                                                                           \
    } while (0)
