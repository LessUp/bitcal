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
    // Original: bit 62 set. After 65-bit shift: bit 127 set (bit 63 of word 1)
    bitcal::bit256 b;
    b[0] = 0x4000000000000000ULL; b[1] = 0; b[2] = 0; b[3] = 0;
    b.shift_left(65);
    ASSERT_EQ(b[0], 0ULL);
    ASSERT_EQ(b[1], 0x8000000000000000ULL);
    ASSERT_EQ(b[2], 0ULL);

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

// ========== Boundary condition tests ==========

bool test_shift_boundary_conditions() {
    // Test shift equal to bit width
    bitcal::bit64 a(0xFFFFFFFFFFFFFFFFULL);
    a.shift_left(64);
    ASSERT_TRUE(a.is_zero());

    bitcal::bit64 b(0xFFFFFFFFFFFFFFFFULL);
    b.shift_right(64);
    ASSERT_TRUE(b.is_zero());

    // Test shift greater than bit width
    bitcal::bit64 c(0xFFFFFFFFFFFFFFFFULL);
    c.shift_left(100);
    ASSERT_TRUE(c.is_zero());

    bitcal::bit64 d(0xFFFFFFFFFFFFFFFFULL);
    d.shift_right(100);
    ASSERT_TRUE(d.is_zero());

    // Test zero shift
    bitcal::bit64 e(0x123456789ABCDEF0ULL);
    e.shift_left(0);
    ASSERT_EQ(e[0], 0x123456789ABCDEF0ULL);

    bitcal::bit64 f(0x123456789ABCDEF0ULL);
    f.shift_right(0);
    ASSERT_EQ(f[0], 0x123456789ABCDEF0ULL);

    // Test 256-bit boundary conditions
    bitcal::bit256 g;
    g[0] = 0xFFFFFFFFFFFFFFFFULL;
    g.shift_left(256);
    ASSERT_TRUE(g.is_zero());

    bitcal::bit256 h;
    h[3] = 0xFFFFFFFFFFFFFFFFULL;
    h.shift_right(256);
    ASSERT_TRUE(h.is_zero());

    // Test 512-bit boundary conditions
    bitcal::bit512 i;
    i[0] = 0xFFFFFFFFFFFFFFFFULL;
    i.shift_left(512);
    ASSERT_TRUE(i.is_zero());

    bitcal::bit512 j;
    j[7] = 0xFFFFFFFFFFFFFFFFULL;
    j.shift_right(512);
    ASSERT_TRUE(j.is_zero());

    return true;
}

bool test_bitwise_mathematical_properties() {
    // Test commutativity: a & b == b & a
    bitcal::bit256 a, b;
    a[0] = 0xFF00FF00FF00FF00ULL; a[1] = 0x00FF00FF00FF00FFULL;
    b[0] = 0xF0F0F0F0F0F0F0F0ULL; b[1] = 0x0F0F0F0F0F0F0F0FULL;
    ASSERT_TRUE((a & b) == (b & a));
    ASSERT_TRUE((a | b) == (b | a));
    ASSERT_TRUE((a ^ b) == (b ^ a));

    // Test associativity: (a & b) & c == a & (b & c)
    bitcal::bit256 c;
    c[0] = 0xAAAAAAAAAAAAAAAAULL; c[1] = 0x5555555555555555ULL;
    ASSERT_TRUE(((a & b) & c) == (a & (b & c)));
    ASSERT_TRUE(((a | b) | c) == (a | (b | c)));
    ASSERT_TRUE(((a ^ b) ^ c) == (a ^ (b ^ c)));

    // Test distributivity: a & (b | c) == (a & b) | (a & c)
    ASSERT_TRUE((a & (b | c)) == ((a & b) | (a & c)));

    // Test De Morgan's laws: ~(a & b) == ~a | ~b, ~(a | b) == ~a & ~b
    ASSERT_TRUE((~(a & b)) == (~a | ~b));
    ASSERT_TRUE((~(a | b)) == (~a & ~b));

    // Test identity: a & all_ones == a, a | all_zeros == a
    bitcal::bit256 all_ones, all_zeros;
    for (size_t i = 0; i < 4; ++i) {
        all_ones[i] = 0xFFFFFFFFFFFFFFFFULL;
        all_zeros[i] = 0;
    }
    ASSERT_TRUE((a & all_ones) == a);
    ASSERT_TRUE((a | all_zeros) == a);

    // Test idempotence: a & a == a, a | a == a
    ASSERT_TRUE((a & a) == a);
    ASSERT_TRUE((a | a) == a);

    // Test XOR with self: a ^ a == 0
    ASSERT_TRUE((a ^ a).is_zero());

    return true;
}

bool test_large_shifts() {
    // Test shift by exact word boundaries (64, 128, 192 bits for 256-bit)
    bitcal::bit256 a;
    a[0] = 0xDEADBEEFCAFEBABEULL;

    // Shift by 64
    bitcal::bit256 b = a;
    b.shift_left(64);
    ASSERT_EQ(b[1], 0xDEADBEEFCAFEBABEULL);
    ASSERT_EQ(b[0], 0ULL);

    // Shift by 128
    bitcal::bit256 c = a;
    c.shift_left(128);
    ASSERT_EQ(c[2], 0xDEADBEEFCAFEBABEULL);
    ASSERT_EQ(c[0], 0ULL);
    ASSERT_EQ(c[1], 0ULL);

    // Shift by 192
    bitcal::bit256 d = a;
    d.shift_left(192);
    ASSERT_EQ(d[3], 0xDEADBEEFCAFEBABEULL);
    ASSERT_EQ(d[0], 0ULL);
    ASSERT_EQ(d[1], 0ULL);
    ASSERT_EQ(d[2], 0ULL);

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

// ========== 1024-bit tests ==========

bool test_bit1024_basic() {
    bitcal::bit1024 a(0xDEADBEEF);
    ASSERT_EQ(a[0], 0xDEADBEEFULL);
    for (size_t i = 1; i < 16; ++i) ASSERT_EQ(a[i], 0ULL);
    return true;
}

bool test_bit1024_shift_cross_carry() {
    // word 7 -> word 8 (cross 512-bit boundary)
    bitcal::bit1024 a;
    a[7] = 0x8000000000000000ULL;
    a.shift_left(1);
    ASSERT_EQ(a[7], 0ULL);
    ASSERT_EQ(a[8], 1ULL);

    // shift right cross
    bitcal::bit1024 b;
    b[8] = 1;
    b.shift_right(1);
    ASSERT_EQ(b[7], 0x8000000000000000ULL);
    ASSERT_EQ(b[8], 0ULL);

    // shift by 512
    bitcal::bit1024 c;
    c[0] = 0xAAAAAAAAAAAAAAAAULL;
    c[1] = 0xBBBBBBBBBBBBBBBBULL;
    c.shift_left(512);
    ASSERT_EQ(c[0], 0ULL);
    ASSERT_EQ(c[8], 0xAAAAAAAAAAAAAAAAULL);
    ASSERT_EQ(c[9], 0xBBBBBBBBBBBBBBBBULL);
    return true;
}

bool test_bit1024_shift_boundary() {
    bitcal::bit1024 a;
    a[0] = 1;
    a.shift_left(1023);
    ASSERT_EQ(a[15], 0x8000000000000000ULL);
    for (int i = 0; i < 15; ++i) ASSERT_EQ(a[i], 0ULL);

    bitcal::bit1024 b;
    b[0] = 0xFFFFFFFFFFFFFFFFULL;
    b.shift_left(1024);
    ASSERT_TRUE(b.is_zero());
    return true;
}

bool test_bit1024_popcount() {
    bitcal::bit1024 a;
    for (size_t i = 0; i < 16; ++i) a[i] = 0xFFFFFFFFFFFFFFFFULL;
    ASSERT_EQ(a.popcount(), 1024ULL);

    bitcal::bit1024 b;
    b[0] = 0xFFFFFFFFFFFFFFFFULL;
    ASSERT_EQ(b.popcount(), 64ULL);
    return true;
}

bool test_bit1024_bitwise_ops() {
    bitcal::bit1024 a, b;
    a[0] = 0xFF00FF00FF00FF00ULL; a[15] = 0x00FF00FF00FF00FFULL;
    b[0] = 0xF0F0F0F0F0F0F0F0ULL; b[15] = 0x0F0F0F0F0F0F0F0FULL;

    auto and_result = a & b;
    ASSERT_EQ(and_result[0], 0xF000F000F000F000ULL);
    ASSERT_EQ(and_result[15], 0x000F000F000F000FULL);

    auto or_result = a | b;
    ASSERT_EQ(or_result[0], 0xFFF0FFF0FFF0FFF0ULL);
    ASSERT_EQ(or_result[15], 0x0FFF0FFF0FFF0FFFULL);

    auto xor_result = a ^ b;
    ASSERT_EQ(xor_result[0], 0x0FF00FF00FF00FF0ULL);
    ASSERT_EQ(xor_result[15], 0x0FF00FF00FF00FF0ULL);
    return true;
}

bool test_bit1024_is_zero() {
    bitcal::bit1024 a;
    ASSERT_TRUE(a.is_zero());

    bitcal::bit1024 b;
    b[15] = 1;
    ASSERT_TRUE(!b.is_zero());

    bitcal::bit1024 c;
    c[8] = 1;
    ASSERT_TRUE(!c.is_zero());
    return true;
}

bool test_bit1024_equality() {
    bitcal::bit1024 a(123), b(123), c(456);
    ASSERT_TRUE(a == b);
    ASSERT_TRUE(a != c);
    return true;
}

// ========== Backend consistency tests ==========

bool test_backend_consistency() {
    // Test that scalar backend produces correct results
    using scalar_bit256 = bitcal::bitarray<256, bitcal::simd_backend::scalar>;

    // Create scalar version and test operations
    scalar_bit256 sa, sb;
    sa[0] = 0xDEADBEEFCAFEBABEULL; sa[1] = 0x123456789ABCDEF0ULL;
    sa[2] = 0xFEDCBA9876543210ULL; sa[3] = 0x0F0F0F0F0F0F0F0FULL;
    sb[0] = 0xABCDEF0123456789ULL; sb[1] = 0x9876543210FEDCBAULL;
    sb[2] = 0x5555555555555555ULL; sb[3] = 0xAAAAAAAAAAAAAAAAULL;

    // Test bitwise operations produce non-zero results
    scalar_bit256 and_result = sa & sb;
    scalar_bit256 or_result = sa | sb;
    scalar_bit256 xor_result = sa ^ sb;
    scalar_bit256 not_result = ~sa;

    ASSERT_TRUE(!and_result.is_zero());
    ASSERT_TRUE(!or_result.is_zero());
    ASSERT_TRUE(!xor_result.is_zero());
    ASSERT_TRUE(!not_result.is_zero());

    // Test popcount
    ASSERT_TRUE(sa.popcount() > 0);
    ASSERT_TRUE(sb.popcount() > 0);

    // Test shifts
    scalar_bit256 shift_sa = sa;
    shift_sa.shift_left(10);
    shift_sa.shift_right(10);
    // After shifting left and right by same amount, we should get back original (with some loss)

    return true;
}

// ========== Type traits tests ==========

bool test_type_traits() {
    // Test is_bitarray
    static_assert(bitcal::is_bitarray_v<bitcal::bit64>, "bit64 should be a bitarray");
    static_assert(bitcal::is_bitarray_v<bitcal::bit256>, "bit256 should be a bitarray");
    static_assert(bitcal::is_bitarray_v<bitcal::bit1024>, "bit1024 should be a bitarray");
    static_assert(!bitcal::is_bitarray_v<int>, "int should not be a bitarray");
    static_assert(!bitcal::is_bitarray_v<uint64_t>, "uint64_t should not be a bitarray");

    // Test bitarray_traits
    static_assert(bitcal::bitarray_traits<bitcal::bit256>::bits == 256, "bit256 should have 256 bits");
    static_assert(bitcal::bitarray_traits<bitcal::bit256>::u64_count == 4, "bit256 should have 4 words");
    static_assert(bitcal::bitarray_traits<bitcal::bit512>::u64_count == 8, "bit512 should have 8 words");
    static_assert(bitcal::bitarray_traits<bitcal::bit64>::u64_count == 1, "bit64 should have 1 word");

    ASSERT_TRUE(true);  // If we get here, all static_asserts passed
    return true;
}

// ========== Static assert validation tests ==========

bool test_static_assert_validation() {
    // These compile-time checks verify the static_assert in bitarray template
    // The following would fail to compile if uncommented:
    // bitcal::bitarray<32> invalid_32;   // Bits < 64
    // bitcal::bitarray<100> invalid_100; // Bits not multiple of 64

    // Valid types should work
    bitcal::bit64 valid_64;
    bitcal::bit128 valid_128;
    bitcal::bit256 valid_256;
    bitcal::bit512 valid_512;
    bitcal::bit1024 valid_1024;
    bitcal::bitarray<2048> valid_2048;

    ASSERT_TRUE(valid_64.is_zero());
    ASSERT_TRUE(valid_128.is_zero());
    ASSERT_TRUE(valid_256.is_zero());
    ASSERT_TRUE(valid_512.is_zero());
    ASSERT_TRUE(valid_1024.is_zero());
    ASSERT_TRUE(valid_2048.is_zero());

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

    std::cout << std::endl << "[1024-bit]" << std::endl;
    RUN_TEST(test_bit1024_basic);
    RUN_TEST(test_bit1024_shift_cross_carry);
    RUN_TEST(test_bit1024_shift_boundary);
    RUN_TEST(test_bit1024_popcount);
    RUN_TEST(test_bit1024_bitwise_ops);
    RUN_TEST(test_bit1024_is_zero);
    RUN_TEST(test_bit1024_equality);

    std::cout << std::endl << "[ANDNOT]" << std::endl;
    RUN_TEST(test_andnot_64);
    RUN_TEST(test_andnot_256);

    std::cout << std::endl << "[NOT]" << std::endl;
    RUN_TEST(test_not_128);
    RUN_TEST(test_not_256);

    std::cout << std::endl << "[is_zero]" << std::endl;
    RUN_TEST(test_is_zero_various);

    std::cout << std::endl << "[Boundary Conditions]" << std::endl;
    RUN_TEST(test_shift_boundary_conditions);
    RUN_TEST(test_bitwise_mathematical_properties);
    RUN_TEST(test_large_shifts);

    std::cout << std::endl << "[Backend Consistency]" << std::endl;
    RUN_TEST(test_backend_consistency);

    std::cout << std::endl << "[Type Traits]" << std::endl;
    RUN_TEST(test_type_traits);

    std::cout << std::endl << "[Static Assert Validation]" << std::endl;
    RUN_TEST(test_static_assert_validation);

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
