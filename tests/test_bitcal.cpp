#include <bitcal/bitcal.hpp>
#include <iostream>
#include <cstring>

static int g_pass = 0;
static int g_fail = 0;

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        std::cerr << "  FAIL: " << #a << " == " << #b \
                  << " (got " << (a) << " vs " << (b) << ")" \
                  << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    } \
} while(0)

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        std::cerr << "  FAIL: " << #expr \
                  << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
        return false; \
    } \
} while(0)

#define RUN_TEST(func) do { \
    std::cout << "  " << #func << " ... "; \
    if (func()) { std::cout << "PASS" << std::endl; ++g_pass; } \
    else { std::cout << "FAIL" << std::endl; ++g_fail; } \
} while(0)

// ========== 64-bit tests ==========

bool test_bit64_basic() {
    bitcal::bit64 a(0xDEADBEEFCAFEBABE);
    ASSERT_EQ(a[0], 0xDEADBEEFCAFEBABEULL);
    ASSERT_TRUE(!a.is_zero());
    a.clear();
    ASSERT_TRUE(a.is_zero());
    ASSERT_EQ(a[0], 0ULL);
    return true;
}

bool test_bit64_shift_left() {
    bitcal::bit64 a(1);
    a.shift_left(10);
    ASSERT_EQ(a[0], 1024ULL);

    bitcal::bit64 b(0x8000000000000000ULL);
    b.shift_left(1);
    ASSERT_EQ(b[0], 0ULL);
    return true;
}

bool test_bit64_shift_right() {
    bitcal::bit64 a(1024);
    a.shift_right(10);
    ASSERT_EQ(a[0], 1ULL);

    bitcal::bit64 b(1);
    b.shift_right(1);
    ASSERT_EQ(b[0], 0ULL);
    return true;
}

bool test_bit64_bitwise_ops() {
    bitcal::bit64 a(0xFF00FF00FF00FF00ULL);
    bitcal::bit64 b(0xF0F0F0F0F0F0F0F0ULL);
    ASSERT_EQ((a & b)[0], 0xF000F000F000F000ULL);
    ASSERT_EQ((a | b)[0], 0xFFF0FFF0FFF0FFF0ULL);
    ASSERT_EQ((a ^ b)[0], 0x0FF00FF00FF00FF0ULL);
    ASSERT_EQ((~a)[0],    0x00FF00FF00FF00FFULL);
    return true;
}

bool test_bit64_popcount() {
    ASSERT_EQ(bitcal::bit64(0).popcount(), 0ULL);
    ASSERT_EQ(bitcal::bit64(0xFFFFFFFFFFFFFFFFULL).popcount(), 64ULL);
    ASSERT_EQ(bitcal::bit64(0xAAAAAAAAAAAAAAAAULL).popcount(), 32ULL);
    return true;
}

bool test_bit64_clz_ctz() {
    bitcal::bit64 a(0);
    ASSERT_EQ(a.count_leading_zeros(), 64);
    ASSERT_EQ(a.count_trailing_zeros(), 64);
    bitcal::bit64 b(1);
    ASSERT_EQ(b.count_leading_zeros(), 63);
    ASSERT_EQ(b.count_trailing_zeros(), 0);
    bitcal::bit64 c(0x8000000000000000ULL);
    ASSERT_EQ(c.count_leading_zeros(), 0);
    ASSERT_EQ(c.count_trailing_zeros(), 63);
    return true;
}

bool test_bit64_bit_ops() {
    bitcal::bit64 a;
    a.set_bit(0); ASSERT_TRUE(a.get_bit(0)); ASSERT_EQ(a[0], 1ULL);
    a.set_bit(63); ASSERT_TRUE(a.get_bit(63)); ASSERT_EQ(a[0], 0x8000000000000001ULL);
    a.set_bit(0, false); ASSERT_TRUE(!a.get_bit(0)); ASSERT_EQ(a[0], 0x8000000000000000ULL);
    a.flip_bit(63); ASSERT_TRUE(!a.get_bit(63)); ASSERT_EQ(a[0], 0ULL);
    return true;
}

bool test_bit64_reverse() {
    bitcal::bit64 a(0x0000000000000001ULL);
    a.reverse();
    ASSERT_EQ(a[0], 0x8000000000000000ULL);
    bitcal::bit64 b(0x00000000000000FFULL);
    b.reverse();
    ASSERT_EQ(b[0], 0xFF00000000000000ULL);
    return true;
}

bool test_bit64_compound_assignment() {
    bitcal::bit64 a(0xFF00), b(0xF0F0);
    a &= b; ASSERT_EQ(a[0], 0xF000ULL);
    a = bitcal::bit64(0xFF00); a |= b; ASSERT_EQ(a[0], 0xFFF0ULL);
    a = bitcal::bit64(0xFF00); a ^= b; ASSERT_EQ(a[0], 0x0FF0ULL);
    a = bitcal::bit64(1); a <<= 10; ASSERT_EQ(a[0], 1024ULL);
    a >>= 5; ASSERT_EQ(a[0], 32ULL);
    return true;
}

// ========== 128-bit tests ==========

bool test_bit128_basic() {
    bitcal::bit128 a(0xDEADBEEF);
    ASSERT_EQ(a[0], 0xDEADBEEFULL);
    ASSERT_EQ(a[1], 0ULL);
    return true;
}

bool test_bit128_shift_left() {
    bitcal::bit128 a;
    a[0] = 0xFFFFFFFFFFFFFFFFULL; a[1] = 0;
    a.shift_left(64);
    ASSERT_EQ(a[0], 0ULL);
    ASSERT_EQ(a[1], 0xFFFFFFFFFFFFFFFFULL);

    bitcal::bit128 b; b[0] = 1; b[1] = 0;
    b.shift_left(10);
    ASSERT_EQ(b[0], 1024ULL);
    ASSERT_EQ(b[1], 0ULL);
    return true;
}

bool test_bit128_shift_right() {
    bitcal::bit128 a; a[0] = 0; a[1] = 0xFFFFFFFFFFFFFFFFULL;
    a.shift_right(64);
    ASSERT_EQ(a[0], 0xFFFFFFFFFFFFFFFFULL);
    ASSERT_EQ(a[1], 0ULL);
    return true;
}

bool test_bit128_shift_cross_carry() {
    bitcal::bit128 a;
    a[0] = 0x8000000000000000ULL;
    a[1] = 0;
    a.shift_left(1);
    ASSERT_EQ(a[0], 0ULL);
    ASSERT_EQ(a[1], 1ULL);

    bitcal::bit128 b;
    b[0] = 0; b[1] = 1;
    b.shift_right(1);
    ASSERT_EQ(b[0], 0x8000000000000000ULL);
    ASSERT_EQ(b[1], 0ULL);
    return true;
}

bool test_bit128_bitwise_ops() {
    bitcal::bit128 a; a[0] = 0xFF00FF00FF00FF00ULL; a[1] = 0x00FF00FF00FF00FFULL;
    bitcal::bit128 b; b[0] = 0xF0F0F0F0F0F0F0F0ULL; b[1] = 0x0F0F0F0F0F0F0F0FULL;
    ASSERT_EQ((a & b)[0], 0xF000F000F000F000ULL);
    ASSERT_EQ((a & b)[1], 0x000F000F000F000FULL);
    ASSERT_EQ((a | b)[0], 0xFFF0FFF0FFF0FFF0ULL);
    ASSERT_EQ((a | b)[1], 0x0FFF0FFF0FFF0FFFULL);
    return true;
}

// ========== 256-bit tests ==========

bool test_bit256_basic() {
    bitcal::bit256 a(0x123456789ABCDEF0ULL);
    ASSERT_EQ(a[0], 0x123456789ABCDEF0ULL);
    ASSERT_EQ(a[1], 0ULL); ASSERT_EQ(a[2], 0ULL); ASSERT_EQ(a[3], 0ULL);
    return true;
}

bool test_bit256_shift_128() {
    bitcal::bit256 a; a[0] = 0xFFFFFFFFFFFFFFFFULL;
    a.shift_left(128);
    ASSERT_EQ(a[0], 0ULL); ASSERT_EQ(a[1], 0ULL);
    ASSERT_EQ(a[2], 0xFFFFFFFFFFFFFFFFULL); ASSERT_EQ(a[3], 0ULL);
    return true;
}

bool test_bit256_shift_cross_carry() {
    // shift left by 1: carry from word 0 -> 1
    bitcal::bit256 a;
    a[0] = 0x8000000000000000ULL; a[1] = 0; a[2] = 0; a[3] = 0;
    a.shift_left(1);
    ASSERT_EQ(a[0], 0ULL);
    ASSERT_EQ(a[1], 1ULL);
    ASSERT_EQ(a[2], 0ULL);
    ASSERT_EQ(a[3], 0ULL);

    // shift left by 1: carry from word 1 -> 2 (cross 128-bit boundary)
    bitcal::bit256 b;
    b[0] = 0; b[1] = 0x8000000000000000ULL; b[2] = 0; b[3] = 0;
    b.shift_left(1);
    ASSERT_EQ(b[0], 0ULL);
    ASSERT_EQ(b[1], 0ULL);
    ASSERT_EQ(b[2], 1ULL);
    ASSERT_EQ(b[3], 0ULL);

    // shift left by 1: carry from word 2 -> 3
    bitcal::bit256 c;
    c[0] = 0; c[1] = 0; c[2] = 0x8000000000000000ULL; c[3] = 0;
    c.shift_left(1);
    ASSERT_EQ(c[0], 0ULL);
    ASSERT_EQ(c[1], 0ULL);
    ASSERT_EQ(c[2], 0ULL);
    ASSERT_EQ(c[3], 1ULL);

    // shift right cross-carry
    bitcal::bit256 d;
    d[0] = 0; d[1] = 0; d[2] = 1; d[3] = 0;
    d.shift_right(1);
    ASSERT_EQ(d[0], 0ULL);
    ASSERT_EQ(d[1], 0x8000000000000000ULL);
    ASSERT_EQ(d[2], 0ULL);
    ASSERT_EQ(d[3], 0ULL);
    return true;
}

bool test_bit256_shift_boundary() {
    // shift by exactly 64
    bitcal::bit256 a;
    a[0] = 0xABCDEF0123456789ULL; a[1] = 0; a[2] = 0; a[3] = 0;
    a.shift_left(64);
    ASSERT_EQ(a[0], 0ULL);
    ASSERT_EQ(a[1], 0xABCDEF0123456789ULL);
    ASSERT_EQ(a[2], 0ULL);

    // shift by 65 = 64 word shift + 1 bit shift
    bitcal::bit256 b;
    b[0] = 0x4000000000000000ULL; b[1] = 0; b[2] = 0; b[3] = 0;
    b.shift_left(65);
    ASSERT_EQ(b[0], 0ULL);
    ASSERT_EQ(b[1], 0ULL);
    ASSERT_EQ(b[2], 0x8000000000000000ULL);

    // shift by 255
    bitcal::bit256 e;
    e[0] = 1; e[1] = 0; e[2] = 0; e[3] = 0;
    e.shift_left(255);
    ASSERT_EQ(e[0], 0ULL);
    ASSERT_EQ(e[1], 0ULL);
    ASSERT_EQ(e[2], 0ULL);
    ASSERT_EQ(e[3], 0x8000000000000000ULL);

    // shift by 256 = all zero
    bitcal::bit256 f;
    f[0] = 0xFFFFFFFFFFFFFFFFULL;
    f.shift_left(256);
    ASSERT_TRUE(f.is_zero());
    return true;
}

bool test_bit256_popcount() {
    bitcal::bit256 a;
    for (size_t i = 0; i < 4; ++i) a[i] = 0xFFFFFFFFFFFFFFFFULL;
    ASSERT_EQ(a.popcount(), 256ULL);
    return true;
}

bool test_bit256_equality() {
    bitcal::bit256 a(123), b(123), c(456);
    ASSERT_TRUE(a == b); ASSERT_TRUE(a != c);
    return true;
}

// ========== 512-bit tests ==========

bool test_bit512_basic() {
    bitcal::bit512 a(0xDEADBEEF);
    ASSERT_EQ(a[0], 0xDEADBEEFULL);
    for (size_t i = 1; i < 8; ++i) ASSERT_EQ(a[i], 0ULL);
    return true;
}

bool test_bit512_shift_cross_carry() {
    // word 3 -> word 4 (cross 256-bit boundary)
    bitcal::bit512 a;
    a[3] = 0x8000000000000000ULL;
    a.shift_left(1);
    ASSERT_EQ(a[3], 0ULL);
    ASSERT_EQ(a[4], 1ULL);

    // shift right cross
    bitcal::bit512 b;
    b[4] = 1;
    b.shift_right(1);
    ASSERT_EQ(b[3], 0x8000000000000000ULL);
    ASSERT_EQ(b[4], 0ULL);

    // shift by 256
    bitcal::bit512 c;
    c[0] = 0xAAAAAAAAAAAAAAAAULL;
    c[1] = 0xBBBBBBBBBBBBBBBBULL;
    c.shift_left(256);
    ASSERT_EQ(c[0], 0ULL);
    ASSERT_EQ(c[4], 0xAAAAAAAAAAAAAAAAULL);
    ASSERT_EQ(c[5], 0xBBBBBBBBBBBBBBBBULL);
    return true;
}

bool test_bit512_shift_boundary() {
    bitcal::bit512 a;
    a[0] = 1;
    a.shift_left(511);
    ASSERT_EQ(a[7], 0x8000000000000000ULL);
    for (int i = 0; i < 7; ++i) ASSERT_EQ(a[i], 0ULL);

    bitcal::bit512 b;
    b[0] = 0xFFFFFFFFFFFFFFFFULL;
    b.shift_left(512);
    ASSERT_TRUE(b.is_zero());
    return true;
}

// ========== ANDNOT tests ==========

bool test_andnot_64() {
    bitcal::bit64 a(0xFF00FF00FF00FF00ULL);
    bitcal::bit64 mask(0xFFFF0000FFFF0000ULL);
    auto r = a.andnot(mask);
    // a & ~mask = 0xFF00FF00FF00FF00 & 0x0000FFFF0000FFFF = 0x000000FF000000FF...
    ASSERT_EQ(r[0], (0xFF00FF00FF00FF00ULL & ~0xFFFF0000FFFF0000ULL));
    return true;
}

bool test_andnot_256() {
    bitcal::bit256 a, mask;
    a[0] = 0xFFFFFFFFFFFFFFFFULL; a[1] = 0xAAAAAAAAAAAAAAAAULL;
    mask[0] = 0xFF00FF00FF00FF00ULL; mask[1] = 0xFFFFFFFFFFFFFFFFULL;
    auto r = a.andnot(mask);
    ASSERT_EQ(r[0], (0xFFFFFFFFFFFFFFFFULL & ~0xFF00FF00FF00FF00ULL));
    ASSERT_EQ(r[1], 0ULL);
    return true;
}

// ========== NOT tests ==========

bool test_not_128() {
    bitcal::bit128 a;
    a[0] = 0xFF00FF00FF00FF00ULL; a[1] = 0x0000FFFF0000FFFFULL;
    auto r = ~a;
    ASSERT_EQ(r[0], 0x00FF00FF00FF00FFULL);
    ASSERT_EQ(r[1], 0xFFFF0000FFFF0000ULL);
    return true;
}

bool test_not_256() {
    bitcal::bit256 a;
    for (size_t i = 0; i < 4; ++i) a[i] = 0;
    auto r = ~a;
    for (size_t i = 0; i < 4; ++i) ASSERT_EQ(r[i], 0xFFFFFFFFFFFFFFFFULL);
    return true;
}

// ========== is_zero tests ==========

bool test_is_zero_various() {
    bitcal::bit64 a; ASSERT_TRUE(a.is_zero());
    bitcal::bit128 b; ASSERT_TRUE(b.is_zero());
    bitcal::bit256 c; ASSERT_TRUE(c.is_zero());
    bitcal::bit512 d; ASSERT_TRUE(d.is_zero());

    bitcal::bit256 e; e[3] = 1; ASSERT_TRUE(!e.is_zero());
    bitcal::bit512 f; f[7] = 1; ASSERT_TRUE(!f.is_zero());
    return true;
}

// ========== 256-bit reverse ==========

bool test_reverse_256() {
    bitcal::bit256 a;
    a[0] = 1;
    a.reverse();
    ASSERT_EQ(a[3], 0x8000000000000000ULL);
    ASSERT_EQ(a[0], 0ULL);
    return true;
}

// ========== main ==========

int main() {
    std::cout << "=== BitCal Unit Tests ===" << std::endl;

    std::cout << std::endl << "[64-bit]" << std::endl;
    RUN_TEST(test_bit64_basic);
    RUN_TEST(test_bit64_shift_left);
    RUN_TEST(test_bit64_shift_right);
    RUN_TEST(test_bit64_bitwise_ops);
    RUN_TEST(test_bit64_popcount);
    RUN_TEST(test_bit64_clz_ctz);
    RUN_TEST(test_bit64_bit_ops);
    RUN_TEST(test_bit64_reverse);
    RUN_TEST(test_bit64_compound_assignment);

    std::cout << std::endl << "[128-bit]" << std::endl;
    RUN_TEST(test_bit128_basic);
    RUN_TEST(test_bit128_shift_left);
    RUN_TEST(test_bit128_shift_right);
    RUN_TEST(test_bit128_shift_cross_carry);
    RUN_TEST(test_bit128_bitwise_ops);

    std::cout << std::endl << "[256-bit]" << std::endl;
    RUN_TEST(test_bit256_basic);
    RUN_TEST(test_bit256_shift_128);
    RUN_TEST(test_bit256_shift_cross_carry);
    RUN_TEST(test_bit256_shift_boundary);
    RUN_TEST(test_bit256_popcount);
    RUN_TEST(test_bit256_equality);
    RUN_TEST(test_reverse_256);

    std::cout << std::endl << "[512-bit]" << std::endl;
    RUN_TEST(test_bit512_basic);
    RUN_TEST(test_bit512_shift_cross_carry);
    RUN_TEST(test_bit512_shift_boundary);

    std::cout << std::endl << "[ANDNOT]" << std::endl;
    RUN_TEST(test_andnot_64);
    RUN_TEST(test_andnot_256);

    std::cout << std::endl << "[NOT]" << std::endl;
    RUN_TEST(test_not_128);
    RUN_TEST(test_not_256);

    std::cout << std::endl << "[is_zero]" << std::endl;
    RUN_TEST(test_is_zero_various);

    std::cout << std::endl << "==============================" << std::endl;
    std::cout << "Total: " << (g_pass + g_fail)
              << "  Pass: " << g_pass
              << "  Fail: " << g_fail << std::endl;

    if (g_fail > 0) {
        std::cout << "SOME TESTS FAILED!" << std::endl;
        return 1;
    }
    std::cout << "ALL TESTS PASSED!" << std::endl;
    return 0;
}
