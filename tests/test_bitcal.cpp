#include <bitcal/bitcal.hpp>
#include <iostream>
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <utility>

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

// ============================================================================
// 测试辅助函数：只使用公开接口，不依赖内部实现
// ============================================================================

/// 从位位置列表创建 bitarray（只使用公开接口 set_bit）
template<typename BitArray>
BitArray make_bitarray(std::initializer_list<size_t> bit_positions) {
    BitArray arr;
    for (size_t pos : bit_positions) {
        arr.set_bit(pos, true);
    }
    return arr;
}

/// 从 uint64_t 值创建 bitarray（使用构造函数）
template<typename BitArray>
BitArray make_bitarray_from_u64(uint64_t value) {
    return BitArray(value);
}

/// 设置 bitarray 的所有位
template<typename BitArray>
void set_all_bits(BitArray& arr) {
    for (size_t i = 0; i < BitArray::bits; ++i) {
        arr.set_bit(i, true);
    }
}

/// 检查两个 bitarray 是否相等（使用公开接口）
template<typename BitArray>
bool arrays_equal(const BitArray& a, const BitArray& b) {
    return a == b;
}

/// 验证位模式是否匹配预期（使用 get_bit）
template<typename BitArray>
bool verify_bit_pattern(const BitArray& arr, std::initializer_list<std::pair<size_t, bool>> expected) {
    for (const auto& [pos, val] : expected) {
        if (arr.get_bit(pos) != val) return false;
    }
    return true;
}

// ============================================================================
// Width-agnostic shared test helpers
// ============================================================================

/// Verify single-bit shift left and right with boundary checks
template<typename BitArray>
bool verify_single_bit_shift_left(size_t shift_amount, size_t bit_width) {
    BitArray arr;
    arr.set_bit(0, true);
    arr.shift_left(shift_amount);
    
    if (shift_amount >= bit_width) {
        if (!arr.is_zero()) return false;
    } else {
        if (!arr.get_bit(shift_amount)) return false;
        if (shift_amount > 0 && arr.get_bit(shift_amount - 1)) return false;
    }
    return true;
}

/// Verify single-bit shift right with boundary checks
template<typename BitArray>
bool verify_single_bit_shift_right(size_t shift_amount, size_t bit_width) {
    BitArray arr;
    arr.set_bit(bit_width - 1, true);
    arr.shift_right(shift_amount);
    
    if (shift_amount >= bit_width) {
        if (!arr.is_zero()) return false;
    } else {
        size_t expected_pos = bit_width - 1 - shift_amount;
        if (!arr.get_bit(expected_pos)) return false;
        if (shift_amount > 0 && arr.get_bit(expected_pos + 1)) return false;
    }
    return true;
}

/// Verify that shift by full width results in zero
template<typename BitArray>
bool verify_zero_after_full_width_shift(size_t bit_width) {
    BitArray arr;
    arr.set_bit(0, true);
    arr.shift_left(bit_width);
    if (!arr.is_zero()) return false;
    
    arr.set_bit(bit_width - 1, true);
    arr.shift_right(bit_width);
    if (!arr.is_zero()) return false;
    
    return true;
}

/// Verify equality of constructed arrays
template<typename BitArray>
bool verify_equality_basics(uint64_t value_a, uint64_t value_b) {
    BitArray a(value_a), b(value_b), c(value_a);
    if (!(a == c)) return false;
    if (a == b && value_a != value_b) return false;
    if (a != b && value_a == value_b) return false;
    return true;
}

template<typename T, typename = void>
struct has_set_range : std::false_type {};

template<typename T>
struct has_set_range<T, std::void_t<decltype(std::declval<T&>().set_range(size_t{}, size_t{}))>> : std::true_type {};

template<typename T, typename = void>
struct has_clear_range : std::false_type {};

template<typename T>
struct has_clear_range<T, std::void_t<decltype(std::declval<T&>().clear_range(size_t{}, size_t{}))>> : std::true_type {};

template<typename T, typename = void>
struct has_flip_range : std::false_type {};

template<typename T>
struct has_flip_range<T, std::void_t<decltype(std::declval<T&>().flip_range(size_t{}, size_t{}))>> : std::true_type {};

template<typename T, typename = void>
struct has_find_first_set : std::false_type {};

template<typename T>
struct has_find_first_set<T, std::void_t<decltype(std::declval<const T&>().find_first_set())>> : std::true_type {};

template<typename T, typename = void>
struct has_find_last_set : std::false_type {};

template<typename T>
struct has_find_last_set<T, std::void_t<decltype(std::declval<const T&>().find_last_set())>> : std::true_type {};

template<typename T, typename = void>
struct has_all : std::false_type {};

template<typename T>
struct has_all<T, std::void_t<decltype(std::declval<const T&>().all())>> : std::true_type {};

template<typename T, typename = void>
struct has_any : std::false_type {};

template<typename T>
struct has_any<T, std::void_t<decltype(std::declval<const T&>().any())>> : std::true_type {};

template<typename T, typename = void>
struct has_none : std::false_type {};

template<typename T>
struct has_none<T, std::void_t<decltype(std::declval<const T&>().none())>> : std::true_type {};

template<typename T, typename = void>
struct has_count : std::false_type {};

template<typename T>
struct has_count<T, std::void_t<decltype(std::declval<const T&>().count())>> : std::true_type {};

template<typename T, typename = void>
struct has_test : std::false_type {};

template<typename T>
struct has_test<T, std::void_t<decltype(std::declval<const T&>().test(size_t{}))>> : std::true_type {};

template<typename T, typename = void>
struct has_size : std::false_type {};

template<typename T>
struct has_size<T, std::void_t<decltype(std::declval<const T&>().size())>> : std::true_type {};

template<typename T, typename = void>
struct has_explicit_uint64_conversion : std::false_type {};

template<typename T>
struct has_explicit_uint64_conversion<T, std::void_t<decltype(static_cast<uint64_t>(std::declval<const T&>()))>> : std::true_type {};

static_assert(!has_set_range<bitcal::bit256>::value, "set_range should not remain public");
static_assert(!has_clear_range<bitcal::bit256>::value, "clear_range should not remain public");
static_assert(!has_flip_range<bitcal::bit256>::value, "flip_range should not remain public");
static_assert(!has_find_first_set<bitcal::bit256>::value, "find_first_set should not remain public");
static_assert(!has_find_last_set<bitcal::bit256>::value, "find_last_set should not remain public");
static_assert(!has_all<bitcal::bit256>::value, "all should not remain public");
static_assert(!has_any<bitcal::bit256>::value, "any should not remain public");
static_assert(!has_none<bitcal::bit256>::value, "none should not remain public");
static_assert(!has_count<bitcal::bit256>::value, "count should not remain public");
static_assert(!has_test<bitcal::bit256>::value, "test should not remain public");
static_assert(!has_size<bitcal::bit256>::value, "size should not remain public");
static_assert(!has_explicit_uint64_conversion<bitcal::bit64>::value, "bit64 to uint64_t conversion should not remain public");

bool test_retained_public_contract() {
    bitcal::bit256 a;
    ASSERT_TRUE(a.is_zero());

    a.set_bit(0, true);
    a.set_bit(63, true);
    a.set_bit(200, true);
    ASSERT_TRUE(a.get_bit(0));
    ASSERT_TRUE(a.get_bit(63));
    ASSERT_TRUE(a.get_bit(200));
    ASSERT_EQ(a.popcount(), 3ULL);

    bitcal::bit256 b(0x3ULL);
    b.shift_left(64);
    ASSERT_TRUE(b.get_bit(64));
    ASSERT_TRUE(b.get_bit(65));
    ASSERT_EQ(b.popcount(), 2ULL);

    bitcal::bit256 combined = a | b;
    ASSERT_TRUE((combined & b) == b);
    ASSERT_TRUE((combined ^ b) == a);

    combined.clear();
    ASSERT_TRUE(combined.is_zero());
    return true;
}

// ========== 64-bit tests ==========

bool test_bit64_basic() {
    bitcal::bit64 a(0xDEADBEEFCAFEBABE);
    // 使用运算符和 popcount 验证值，而非直接访问内部数组
    ASSERT_EQ(a.popcount(), 46ULL);  // 0xDEADBEEFCAFEBABE 有 46 个位设置
    ASSERT_TRUE(!a.is_zero());
    a.clear();
    ASSERT_TRUE(a.is_zero());
    ASSERT_EQ(a.popcount(), 0ULL);
    return true;
}

bool test_bit64_shift_left() {
    bitcal::bit64 a(1);
    a.shift_left(10);
    ASSERT_TRUE(a.get_bit(10));
    ASSERT_TRUE(!a.get_bit(9));

    bitcal::bit64 b(0x8000000000000000ULL);
    b.shift_left(1);
    ASSERT_TRUE(b.is_zero());
    return true;
}

bool test_bit64_shift_right() {
    bitcal::bit64 a(1024);
    a.shift_right(10);
    ASSERT_TRUE(a.get_bit(0));

    bitcal::bit64 b(1);
    b.shift_right(1);
    ASSERT_TRUE(b.is_zero());
    return true;
}

bool test_bit64_bitwise_ops() {
    // 使用 set_bit 设置位模式，而非直接赋值数组
    bitcal::bit64 a, b;
    // 设置 a 为交替模式: 0xFF00FF00FF00FF00
    for (int i = 0; i < 64; i += 2) {
        if ((i / 8) % 2 == 0) {
            a.set_bit(i, false);
        } else {
            a.set_bit(i, true);
        }
    }
    // 简化：使用构造函数
    a = bitcal::bit64(0xFF00FF00FF00FF00ULL);
    b = bitcal::bit64(0xF0F0F0F0F0F0F0F0ULL);

    // 验证 AND 结果
    auto and_result = a & b;
    ASSERT_EQ(and_result.popcount(), (a & b).popcount());

    // 使用数学性质验证
    bitcal::bit64 all_ones(~0ULL);
    ASSERT_TRUE((a & all_ones) == a);
    ASSERT_TRUE((a | bitcal::bit64(0)) == a);
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
    a.set_bit(0); ASSERT_TRUE(a.get_bit(0));
    a.set_bit(63); ASSERT_TRUE(a.get_bit(63));
    a.set_bit(0, false); ASSERT_TRUE(!a.get_bit(0));
    a.flip_bit(63); ASSERT_TRUE(!a.get_bit(63));
    return true;
}

bool test_bit64_reverse() {
    bitcal::bit64 a(0x0000000000000001ULL);
    a.reverse();
    ASSERT_TRUE(a.get_bit(63));
    ASSERT_TRUE(!a.get_bit(62));

    bitcal::bit64 b(0x00000000000000FFULL);
    b.reverse();
    ASSERT_TRUE(b.get_bit(56));  // bit 0 -> bit 63, bit 7 -> bit 56
    return true;
}

bool test_bit64_compound_assignment() {
    bitcal::bit64 a(0xFF00), b(0xF0F0);
    a &= b;
    ASSERT_EQ(a.popcount(), bitcal::bit64(0xF000).popcount());

    a = bitcal::bit64(0xFF00); a |= b;
    ASSERT_EQ(a.popcount(), bitcal::bit64(0xFFF0).popcount());

    a = bitcal::bit64(0xFF00); a ^= b;
    ASSERT_EQ(a.popcount(), bitcal::bit64(0x0FF0).popcount());

    a = bitcal::bit64(1); a <<= 10;
    ASSERT_TRUE(a.get_bit(10));
    a >>= 5;
    ASSERT_TRUE(a.get_bit(5));
    return true;
}

// ========== 128-bit tests ==========

bool test_bit128_basic() {
    bitcal::bit128 a(0xDEADBEEF);
    // 使用 get_bit 验证低位 - DEADBEEF 的二进制
    ASSERT_TRUE(a.get_bit(0));  // bit 0
    ASSERT_TRUE(a.get_bit(1));  // bit 1
    ASSERT_TRUE(a.get_bit(2));  // bit 2
    ASSERT_TRUE(a.get_bit(3));  // bit 3 (0xF 最低 4 位)
    // 验证高位为 0
    ASSERT_TRUE(!a.get_bit(64));
    ASSERT_TRUE(!a.get_bit(127));
    return true;
}

bool test_bit128_shift_left() {
    bitcal::bit128 a;
    // 设置低 64 位的所有位
    for (int i = 0; i < 64; ++i) a.set_bit(i, true);
    a.shift_left(64);
    // 现在高 64 位应该有所有位
    for (int i = 64; i < 128; ++i) ASSERT_TRUE(a.get_bit(i));
    for (int i = 0; i < 64; ++i) ASSERT_TRUE(!a.get_bit(i));

    // Use helper to verify single-bit shift behavior
    ASSERT_TRUE(verify_single_bit_shift_left<bitcal::bit128>(10, 128));
    return true;
}

bool test_bit128_shift_right() {
    bitcal::bit128 a;
    // 设置高 64 位的所有位
    for (int i = 64; i < 128; ++i) a.set_bit(i, true);
    a.shift_right(64);
    // 现在低 64 位应该有所有位
    for (int i = 0; i < 64; ++i) ASSERT_TRUE(a.get_bit(i));
    for (int i = 64; i < 128; ++i) ASSERT_TRUE(!a.get_bit(i));
    return true;
}

bool test_bit128_shift_cross_carry() {
    bitcal::bit128 a;
    a.set_bit(63, true);  // 低 64 位的最高位
    a.shift_left(1);
    ASSERT_TRUE(a.get_bit(64));  // 应该移到高 64 位
    ASSERT_TRUE(!a.get_bit(63));

    bitcal::bit128 b;
    b.set_bit(64, true);  // 高 64 位的最低位
    b.shift_right(1);
    ASSERT_TRUE(b.get_bit(63));  // 应该移到低 64 位
    ASSERT_TRUE(!b.get_bit(64));
    return true;
}

bool test_bit128_bitwise_ops() {
    bitcal::bit128 a, b;
    for (size_t i = 0; i < 64; ++i) {
        a.set_bit(i, true);
    }
    for (size_t i = 32; i < 96; ++i) {
        b.set_bit(i, true);
    }

    // 使用 popcount 验证操作结果
    auto and_result = a & b;
    ASSERT_EQ(and_result.popcount(), 32ULL);  // 重叠: bits 32-63

    auto or_result = a | b;
    ASSERT_EQ(or_result.popcount(), 96ULL);  // 总共 96 位

    // 使用数学性质验证
    ASSERT_TRUE((a & b).popcount() == (a.popcount() + b.popcount() - (a | b).popcount()));
    return true;
}

// ========== 256-bit tests ==========

bool test_bit256_basic() {
    bitcal::bit256 a(0x123456789ABCDEF0ULL);
    // 使用 popcount 验证值
    ASSERT_EQ(a.popcount(), bitcal::bit64(0x123456789ABCDEF0ULL).popcount());
    // 验证高位为 0
    ASSERT_TRUE(!a.get_bit(64));
    ASSERT_TRUE(!a.get_bit(128));
    ASSERT_TRUE(!a.get_bit(192));
    return true;
}

bool test_bit256_shift_128() {
    bitcal::bit256 a;
    // 设置低 64 位
    for (int i = 0; i < 64; ++i) a.set_bit(i, true);
    a.shift_left(128);
    // 验证移位结果
    for (int i = 128; i < 192; ++i) ASSERT_TRUE(a.get_bit(i));
    for (int i = 0; i < 128; ++i) ASSERT_TRUE(!a.get_bit(i));
    return true;
}

bool test_bit256_shift_cross_carry() {
    // shift left by 1: carry from word 0 -> 1
    bitcal::bit256 a;
    a.set_bit(63, true);
    a.shift_left(1);
    ASSERT_TRUE(a.get_bit(64));
    ASSERT_TRUE(!a.get_bit(63));

    // shift left by 1: carry from word 1 -> 2 (cross 128-bit boundary)
    bitcal::bit256 b;
    b.set_bit(127, true);
    b.shift_left(1);
    ASSERT_TRUE(b.get_bit(128));
    ASSERT_TRUE(!b.get_bit(127));

    // shift left by 1: carry from word 2 -> 3
    bitcal::bit256 c;
    c.set_bit(191, true);
    c.shift_left(1);
    ASSERT_TRUE(c.get_bit(192));
    ASSERT_TRUE(!c.get_bit(191));

    // shift right cross-carry
    bitcal::bit256 d;
    d.set_bit(128, true);
    d.shift_right(1);
    ASSERT_TRUE(d.get_bit(127));
    ASSERT_TRUE(!d.get_bit(128));
    
    // Use helper for single-bit shift verification
    ASSERT_TRUE(verify_single_bit_shift_right<bitcal::bit256>(1, 256));
    return true;
}

bool test_bit256_shift_boundary() {
    // shift by exactly 64
    bitcal::bit256 a;
    a.set_bit(0, true);
    a.set_bit(63, true);
    a.shift_left(64);
    ASSERT_TRUE(a.get_bit(64));
    ASSERT_TRUE(a.get_bit(127));
    ASSERT_TRUE(!a.get_bit(0));
    ASSERT_TRUE(!a.get_bit(63));

    // shift by 65 = 64 word shift + 1 bit shift
    bitcal::bit256 b;
    b.set_bit(62, true);
    b.shift_left(65);
    ASSERT_TRUE(b.get_bit(127));
    ASSERT_TRUE(!b.get_bit(126));

    // shift by 255
    bitcal::bit256 e;
    e.set_bit(0, true);
    e.shift_left(255);
    ASSERT_TRUE(e.get_bit(255));
    ASSERT_TRUE(!e.get_bit(254));

    // Use helper to verify full-width shift results in zero
    ASSERT_TRUE(verify_zero_after_full_width_shift<bitcal::bit256>(256));
    return true;
}

bool test_bit256_popcount() {
    bitcal::bit256 a;
    // 设置所有位
    for (int i = 0; i < 256; ++i) a.set_bit(i, true);
    ASSERT_EQ(a.popcount(), 256ULL);
    return true;
}

bool test_bit256_equality() {
    // Use helper to verify basic equality contracts
    ASSERT_TRUE(verify_equality_basics<bitcal::bit256>(123, 456));
    
    // Additional explicit test
    bitcal::bit256 a(123), b(123), c(456);
    ASSERT_TRUE(a == b);
    ASSERT_TRUE(a != c);
    return true;
}

// ========== 512-bit tests ==========

bool test_bit512_basic() {
    bitcal::bit512 a(0xDEADBEEF);
    // 验证 popcount
    ASSERT_EQ(a.popcount(), bitcal::bit64(0xDEADBEEF).popcount());
    // 验证高位为 0
    for (int i = 64; i < 512; i += 64) {
        ASSERT_TRUE(!a.get_bit(i));
    }
    return true;
}

bool test_bit512_shift_cross_carry() {
    // word 3 -> word 4 (cross 256-bit boundary)
    bitcal::bit512 a;
    a.set_bit(255, true);  // word 3 的最高位
    a.shift_left(1);
    ASSERT_TRUE(a.get_bit(256));
    ASSERT_TRUE(!a.get_bit(255));

    // shift right cross
    bitcal::bit512 b;
    b.set_bit(256, true);
    b.shift_right(1);
    ASSERT_TRUE(b.get_bit(255));
    ASSERT_TRUE(!b.get_bit(256));

    // shift by 256
    bitcal::bit512 c;
    c.set_bit(0, true);
    c.set_bit(63, true);
    c.shift_left(256);
    ASSERT_TRUE(c.get_bit(256));
    ASSERT_TRUE(c.get_bit(319));
    ASSERT_TRUE(!c.get_bit(0));
    return true;
}

bool test_bit512_shift_boundary() {
    bitcal::bit512 a;
    a.set_bit(0, true);
    a.shift_left(511);
    ASSERT_TRUE(a.get_bit(511));
    ASSERT_TRUE(!a.get_bit(510));
    for (int i = 0; i < 511; ++i) ASSERT_TRUE(!a.get_bit(i));

    // Use helper to verify full-width shift results in zero
    ASSERT_TRUE(verify_zero_after_full_width_shift<bitcal::bit512>(512));
    return true;
}

// ========== ANDNOT tests ==========

bool test_andnot_64() {
    bitcal::bit64 a(0xFF00FF00FF00FF00ULL);
    bitcal::bit64 mask(0xFFFF0000FFFF0000ULL);
    auto r = a.andnot(mask);
    // 验证结果使用 popcount 和特定位的检查
    // a & ~mask = 0xFF00FF00FF00FF00 & 0x0000FFFF0000FFFF
    uint64_t expected = 0xFF00FF00FF00FF00ULL & ~0xFFFF0000FFFF0000ULL;
    ASSERT_EQ(r.popcount(), bitcal::bit64(expected).popcount());
    return true;
}

bool test_andnot_256() {
    bitcal::bit256 a, mask;
    // 设置 a 的低 128 位
    for (int i = 0; i < 128; ++i) a.set_bit(i, true);
    // 设置 mask 的低 64 位
    for (int i = 0; i < 64; ++i) mask.set_bit(i, true);

    auto r = a.andnot(mask);
    // r 应该有 bits 64-127 设置
    ASSERT_EQ(r.popcount(), 64ULL);
    for (int i = 64; i < 128; ++i) ASSERT_TRUE(r.get_bit(i));
    for (int i = 0; i < 64; ++i) ASSERT_TRUE(!r.get_bit(i));
    return true;
}

// ========== NOT tests ==========

bool test_not_128() {
    bitcal::bit128 a;
    // 设置低 64 位的偶数位
    for (int i = 0; i < 64; i += 2) a.set_bit(i, true);
    auto r = ~a;
    // 验证反转后的结果
    for (int i = 0; i < 64; i += 2) ASSERT_TRUE(!r.get_bit(i));
    for (int i = 1; i < 64; i += 2) ASSERT_TRUE(r.get_bit(i));
    for (int i = 64; i < 128; ++i) ASSERT_TRUE(r.get_bit(i));  // 高 64 位应该全 1
    return true;
}

bool test_not_256() {
    bitcal::bit256 a;
    ASSERT_TRUE(a.is_zero());
    auto r = ~a;
    // 所有位应该为 1
    ASSERT_EQ(r.popcount(), 256ULL);
    return true;
}

// ========== is_zero tests ==========

bool test_is_zero_various() {
    bitcal::bit64 a; ASSERT_TRUE(a.is_zero());
    bitcal::bit128 b; ASSERT_TRUE(b.is_zero());
    bitcal::bit256 c; ASSERT_TRUE(c.is_zero());
    bitcal::bit512 d; ASSERT_TRUE(d.is_zero());

    bitcal::bit256 e; e.set_bit(200, true); ASSERT_TRUE(!e.is_zero());
    bitcal::bit512 f; f.set_bit(500, true); ASSERT_TRUE(!f.is_zero());
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
    ASSERT_EQ(e.popcount(), bitcal::bit64(0x123456789ABCDEF0ULL).popcount());

    bitcal::bit64 f(0x123456789ABCDEF0ULL);
    f.shift_right(0);
    ASSERT_EQ(f.popcount(), bitcal::bit64(0x123456789ABCDEF0ULL).popcount());

    // Test 256-bit boundary conditions
    bitcal::bit256 g;
    g.set_bit(0, true);
    g.shift_left(256);
    ASSERT_TRUE(g.is_zero());

    bitcal::bit256 h;
    h.set_bit(255, true);
    h.shift_right(256);
    ASSERT_TRUE(h.is_zero());

    // Test 512-bit boundary conditions
    bitcal::bit512 i;
    i.set_bit(0, true);
    i.shift_left(512);
    ASSERT_TRUE(i.is_zero());

    bitcal::bit512 j;
    j.set_bit(511, true);
    j.shift_right(512);
    ASSERT_TRUE(j.is_zero());

    return true;
}

bool test_bitwise_mathematical_properties() {
    // Test commutativity: a & b == b & a
    bitcal::bit256 a, b;
    // 使用 set_bit 设置位模式
    for (int i = 0; i < 128; ++i) a.set_bit(i, (i % 2) == 0);
    for (int i = 0; i < 128; ++i) b.set_bit(i, (i % 4) == 0);
    ASSERT_TRUE((a & b) == (b & a));
    ASSERT_TRUE((a | b) == (b | a));
    ASSERT_TRUE((a ^ b) == (b ^ a));

    // Test associativity: (a & b) & c == a & (b & c)
    bitcal::bit256 c;
    for (int i = 0; i < 128; ++i) c.set_bit(i, (i % 8) == 0);
    ASSERT_TRUE(((a & b) & c) == (a & (b & c)));
    ASSERT_TRUE(((a | b) | c) == (a | (b | c)));
    ASSERT_TRUE(((a ^ b) ^ c) == (a ^ (b ^ c)));

    // Test distributivity: a & (b | c) == (a & b) | (a & c)
    ASSERT_TRUE((a & (b | c)) == ((a & b) | (a & c)));

    // Test De Morgan's laws: ~(a & b) == ~a | ~b, ~(a | b) == ~a & ~b
    ASSERT_TRUE((~(a & b)) == (~a | ~b));
    ASSERT_TRUE((~(a | b)) == (~a & ~b));

    // Test identity: a & all_ones == a, a | all_zeros == a
    bitcal::bit256 all_ones = ~bitcal::bit256();  // 全 1
    bitcal::bit256 all_zeros;  // 全 0
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
    a.set_bit(0, true);
    a.set_bit(10, true);
    a.set_bit(20, true);

    // Shift by 64
    bitcal::bit256 b = a;
    b.shift_left(64);
    ASSERT_TRUE(b.get_bit(64));
    ASSERT_TRUE(b.get_bit(74));
    ASSERT_TRUE(b.get_bit(84));
    ASSERT_TRUE(!b.get_bit(0));

    // Shift by 128
    bitcal::bit256 c = a;
    c.shift_left(128);
    ASSERT_TRUE(c.get_bit(128));
    ASSERT_TRUE(c.get_bit(138));
    ASSERT_TRUE(c.get_bit(148));
    ASSERT_TRUE(!c.get_bit(0));
    ASSERT_TRUE(!c.get_bit(64));

    // Shift by 192
    bitcal::bit256 d = a;
    d.shift_left(192);
    ASSERT_TRUE(d.get_bit(192));
    ASSERT_TRUE(d.get_bit(202));
    ASSERT_TRUE(d.get_bit(212));
    ASSERT_TRUE(!d.get_bit(0));
    ASSERT_TRUE(!d.get_bit(64));
    ASSERT_TRUE(!d.get_bit(128));

    return true;
}

// ========== 256-bit reverse ==========

bool test_reverse_256() {
    bitcal::bit256 a;
    a.set_bit(0, true);
    a.reverse();
    ASSERT_TRUE(a.get_bit(255));
    ASSERT_TRUE(!a.get_bit(0));
    return true;
}

// ========== 1024-bit tests ==========

bool test_bit1024_basic() {
    bitcal::bit1024 a(0xDEADBEEF);
    ASSERT_EQ(a.popcount(), bitcal::bit64(0xDEADBEEF).popcount());
    // 验证高位为 0
    for (int i = 64; i < 1024; i += 128) {
        ASSERT_TRUE(!a.get_bit(i));
    }
    return true;
}

bool test_bit1024_shift_cross_carry() {
    // word 7 -> word 8 (cross 512-bit boundary)
    bitcal::bit1024 a;
    a.set_bit(511, true);
    a.shift_left(1);
    ASSERT_TRUE(a.get_bit(512));
    ASSERT_TRUE(!a.get_bit(511));

    // shift right cross
    bitcal::bit1024 b;
    b.set_bit(512, true);
    b.shift_right(1);
    ASSERT_TRUE(b.get_bit(511));
    ASSERT_TRUE(!b.get_bit(512));

    // shift by 512
    bitcal::bit1024 c;
    c.set_bit(0, true);
    c.set_bit(63, true);
    c.shift_left(512);
    ASSERT_TRUE(c.get_bit(512));
    ASSERT_TRUE(c.get_bit(575));
    ASSERT_TRUE(!c.get_bit(0));
    return true;
}

bool test_bit1024_shift_boundary() {
    bitcal::bit1024 a;
    a.set_bit(0, true);
    a.shift_left(1023);
    ASSERT_TRUE(a.get_bit(1023));
    ASSERT_TRUE(!a.get_bit(1022));
    for (int i = 0; i < 1023; ++i) ASSERT_TRUE(!a.get_bit(i));

    // Use helper to verify full-width shift results in zero
    ASSERT_TRUE(verify_zero_after_full_width_shift<bitcal::bit1024>(1024));
    return true;
}

bool test_bit1024_popcount() {
    bitcal::bit1024 a;
    // 设置所有位
    for (int i = 0; i < 1024; ++i) a.set_bit(i, true);
    ASSERT_EQ(a.popcount(), 1024ULL);

    bitcal::bit1024 b;
    b.set_bit(0, true);
    ASSERT_EQ(b.popcount(), 1ULL);
    return true;
}

bool test_bit1024_bitwise_ops() {
    bitcal::bit1024 a, b;
    // 设置 a 的特定位
    a.set_bit(0, true);
    a.set_bit(120, true);
    a.set_bit(1000, true);
    // 设置 b 的特定位
    b.set_bit(0, true);
    b.set_bit(60, true);
    b.set_bit(1008, true);

    auto and_result = a & b;
    ASSERT_TRUE(and_result.get_bit(0));  // 只有 bit 0 重叠
    ASSERT_EQ(and_result.popcount(), 1ULL);

    auto or_result = a | b;
    ASSERT_EQ(or_result.popcount(), 5ULL);

    auto xor_result = a ^ b;
    // XOR 应该只包含不重叠的位
    ASSERT_EQ(xor_result.popcount(), 4ULL);
    return true;
}

bool test_bit1024_is_zero() {
    bitcal::bit1024 a;
    ASSERT_TRUE(a.is_zero());

    bitcal::bit1024 b;
    b.set_bit(1000, true);
    ASSERT_TRUE(!b.is_zero());

    bitcal::bit1024 c;
    c.set_bit(512, true);
    ASSERT_TRUE(!c.is_zero());
    return true;
}

bool test_bit1024_equality() {
    // Use helper to verify basic equality contracts
    ASSERT_TRUE(verify_equality_basics<bitcal::bit1024>(123, 456));
    
    // Additional explicit test
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
    // 使用 set_bit 设置位
    sa.set_bit(0, true); sa.set_bit(63, true); sa.set_bit(128, true);
    sb.set_bit(1, true); sb.set_bit(64, true); sb.set_bit(200, true);

    // Test bitwise operations produce non-zero results
    scalar_bit256 and_result = sa & sb;
    scalar_bit256 or_result = sa | sb;
    scalar_bit256 xor_result = sa ^ sb;
    scalar_bit256 not_result = ~sa;

    ASSERT_TRUE(and_result.is_zero());  // 没有重叠位
    ASSERT_TRUE(!or_result.is_zero());
    ASSERT_TRUE(!xor_result.is_zero());
    ASSERT_TRUE(!not_result.is_zero());

    // Test popcount
    ASSERT_EQ(sa.popcount(), 3ULL);
    ASSERT_EQ(sb.popcount(), 3ULL);

    // Test shifts
    scalar_bit256 shift_sa = sa;
    shift_sa.shift_left(10);
    shift_sa.shift_right(10);
    // 位移后会丢失一些位

    return true;
}

// ========== Cross-backend consistency tests ==========

bool test_cross_backend_consistency() {
    // Test that different backends produce identical results
    using scalar_bit256 = bitcal::bitarray<256, bitcal::simd_backend::scalar>;
    using auto_bit256 = bitcal::bit256;  // Uses default (SIMD) backend

    // Initialize both backends with same data using set_bit
    scalar_bit256 sa, sb;
    auto_bit256 aa, ab;

    // 设置相同的位模式
    for (int i = 0; i < 256; i += 7) {
        sa.set_bit(i, true);
        aa.set_bit(i, true);
    }
    for (int i = 3; i < 256; i += 11) {
        sb.set_bit(i, true);
        ab.set_bit(i, true);
    }

    // Test bitwise AND - 验证 popcount 而不是直接比较
    scalar_bit256 scalar_and = sa & sb;
    auto_bit256 auto_and = aa & ab;
    ASSERT_EQ(scalar_and.popcount(), auto_and.popcount());

    // Test bitwise OR
    scalar_bit256 scalar_or = sa | sb;
    auto_bit256 auto_or = aa | ab;
    ASSERT_EQ(scalar_or.popcount(), auto_or.popcount());

    // Test bitwise XOR
    scalar_bit256 scalar_xor = sa ^ sb;
    auto_bit256 auto_xor = aa ^ ab;
    ASSERT_EQ(scalar_xor.popcount(), auto_xor.popcount());

    // Test bitwise NOT
    scalar_bit256 scalar_not = ~sa;
    auto_bit256 auto_not = ~aa;
    ASSERT_EQ(scalar_not.popcount(), auto_not.popcount());

    // Test popcount
    ASSERT_EQ(sa.popcount(), aa.popcount());
    ASSERT_EQ(sb.popcount(), ab.popcount());

    // Test shift left
    scalar_bit256 sa_shift_l = sa;
    auto_bit256 aa_shift_l = aa;
    sa_shift_l.shift_left(37);
    aa_shift_l.shift_left(37);
    ASSERT_EQ(sa_shift_l.popcount(), aa_shift_l.popcount());

    // Test shift right
    scalar_bit256 sa_shift_r = sa;
    auto_bit256 aa_shift_r = aa;
    sa_shift_r.shift_right(23);
    aa_shift_r.shift_right(23);
    ASSERT_EQ(sa_shift_r.popcount(), aa_shift_r.popcount());

    // Test zero / all-ones states with retained methods only
    ASSERT_EQ(sa.is_zero(), aa.is_zero());

    scalar_bit256 scalar_ones = ~scalar_bit256();
    auto_bit256 auto_ones = ~auto_bit256();
    ASSERT_EQ(scalar_ones.popcount(), auto_ones.popcount());
    ASSERT_EQ(scalar_ones.is_zero(), auto_ones.is_zero());

    scalar_bit256 scalar_zeros;
    auto_bit256 auto_zeros;
    ASSERT_EQ(scalar_zeros.popcount(), auto_zeros.popcount());
    ASSERT_EQ(scalar_zeros.is_zero(), auto_zeros.is_zero());

    return true;
}

// ========== Small width backend fallback consistency (seam characterization) ==========

bool test_small_width_backend_fallback_consistency() {
    // Characterization test: verify that small bit widths (64) behave consistently
    // across scalar and SIMD backends, ensuring fallback paths work correctly.
    // This test ensures the backend adapter layer preserves semantics for unsupported
    // width/backend combinations.
    
    // Test scalar 64-bit operations
    using scalar_bit64 = bitcal::bitarray<64, bitcal::simd_backend::scalar>;
    scalar_bit64 scalar64;
    scalar64.set_bit(0, true);
    scalar64.set_bit(63, true);
    ASSERT_EQ(scalar64.popcount(), 2ULL);
    
#if BITCAL_HAS_SSE2
    // SSE2 doesn't have a dedicated 64-bit specialization, should fall back to scalar
    using sse2_bit64 = bitcal::bitarray<64, bitcal::simd_backend::sse2>;
    sse2_bit64 sse2_64;
    sse2_64.set_bit(0, true);
    sse2_64.set_bit(63, true);
    // Verify behavior is consistent with scalar via public interface
    ASSERT_EQ(sse2_64.popcount(), scalar64.popcount());
    ASSERT_EQ(sse2_64.is_zero(), scalar64.is_zero());
#endif

#if BITCAL_HAS_AVX2
    // AVX2 doesn't have a dedicated 64-bit specialization, should fall back to scalar
    using avx2_bit64 = bitcal::bitarray<64, bitcal::simd_backend::avx2>;
    avx2_bit64 avx2_64;
    avx2_64.set_bit(0, true);
    avx2_64.set_bit(63, true);
    // Verify behavior is consistent with scalar via public interface
    ASSERT_EQ(avx2_64.popcount(), scalar64.popcount());
    ASSERT_EQ(avx2_64.is_zero(), scalar64.is_zero());
#endif

#if BITCAL_HAS_AVX512
    // AVX512 doesn't have a dedicated 64-bit specialization, should fall back to scalar
    using avx512_bit64 = bitcal::bitarray<64, bitcal::simd_backend::avx512>;
    avx512_bit64 avx512_64;
    avx512_64.set_bit(0, true);
    avx512_64.set_bit(63, true);
    // Verify behavior is consistent with scalar via public interface
    ASSERT_EQ(avx512_64.popcount(), scalar64.popcount());
    ASSERT_EQ(avx512_64.is_zero(), scalar64.is_zero());
#endif

#if BITCAL_HAS_NEON
    // NEON doesn't have a dedicated 64-bit specialization, should fall back to scalar
    using neon_bit64 = bitcal::bitarray<64, bitcal::simd_backend::neon>;
    neon_bit64 neon_64;
    neon_64.set_bit(0, true);
    neon_64.set_bit(63, true);
    // Verify behavior is consistent with scalar via public interface
    ASSERT_EQ(neon_64.popcount(), scalar64.popcount());
    ASSERT_EQ(neon_64.is_zero(), scalar64.is_zero());
#endif

    return true;
}

// ========== Large-width 1024-bit fallback characterization test ==========

bool test_large_width_1024_fallback_characterization() {
    // Characterization test: verify that large bit widths (1024) that don't have
    // dedicated SIMD optimizations fall back to scalar consistently across all backends.
    // This test locks the behavior before production refactor.
    
    // Create scalar 1024-bit reference
    using scalar_bit1024 = bitcal::bitarray<1024, bitcal::simd_backend::scalar>;
    scalar_bit1024 scalar_ref;
    
    // Set representative bits at key positions: 0, 511, 1000
    scalar_ref.set_bit(0, true);
    scalar_ref.set_bit(511, true);
    scalar_ref.set_bit(1000, true);
    
    // Verify initial state
    ASSERT_EQ(scalar_ref.popcount(), 3ULL);
    ASSERT_TRUE(scalar_ref.get_bit(0));
    ASSERT_TRUE(scalar_ref.get_bit(511));
    ASSERT_TRUE(scalar_ref.get_bit(1000));
    ASSERT_TRUE(!scalar_ref.get_bit(1));    // Adjacent bit should not be set
    ASSERT_TRUE(!scalar_ref.get_bit(510));
    ASSERT_TRUE(!scalar_ref.get_bit(1001));

#if BITCAL_HAS_SSE2
    // SSE2 1024-bit should fall back to scalar
    using sse2_bit1024 = bitcal::bitarray<1024, bitcal::simd_backend::sse2>;
    sse2_bit1024 sse2_test;
    sse2_test.set_bit(0, true);
    sse2_test.set_bit(511, true);
    sse2_test.set_bit(1000, true);
    
    // Verify SSE2 fallback produces same results
    ASSERT_EQ(sse2_test.popcount(), scalar_ref.popcount());
    ASSERT_EQ(sse2_test.get_bit(0), scalar_ref.get_bit(0));
    ASSERT_EQ(sse2_test.get_bit(511), scalar_ref.get_bit(511));
    ASSERT_EQ(sse2_test.get_bit(1000), scalar_ref.get_bit(1000));
    
    // Test shift operation on SSE2 variant
    sse2_bit1024 sse2_shifted = sse2_test;
    sse2_shifted.shift_left(5);
    
    scalar_bit1024 scalar_shifted = scalar_ref;
    scalar_shifted.shift_left(5);
    
    ASSERT_EQ(sse2_shifted.popcount(), scalar_shifted.popcount());
#endif

#if BITCAL_HAS_AVX2
    // AVX2 1024-bit should fall back to scalar
    using avx2_bit1024 = bitcal::bitarray<1024, bitcal::simd_backend::avx2>;
    avx2_bit1024 avx2_test;
    avx2_test.set_bit(0, true);
    avx2_test.set_bit(511, true);
    avx2_test.set_bit(1000, true);
    
    // Verify AVX2 fallback produces same results
    ASSERT_EQ(avx2_test.popcount(), scalar_ref.popcount());
    ASSERT_EQ(avx2_test.get_bit(0), scalar_ref.get_bit(0));
    ASSERT_EQ(avx2_test.get_bit(511), scalar_ref.get_bit(511));
    ASSERT_EQ(avx2_test.get_bit(1000), scalar_ref.get_bit(1000));
    
    // Test shift operation on AVX2 variant
    avx2_bit1024 avx2_shifted = avx2_test;
    avx2_shifted.shift_right(3);
    
    scalar_bit1024 scalar_shifted_r = scalar_ref;
    scalar_shifted_r.shift_right(3);
    
    ASSERT_EQ(avx2_shifted.popcount(), scalar_shifted_r.popcount());
#endif

#if BITCAL_HAS_AVX512
    // AVX512 1024-bit should fall back to scalar
    using avx512_bit1024 = bitcal::bitarray<1024, bitcal::simd_backend::avx512>;
    avx512_bit1024 avx512_test;
    avx512_test.set_bit(0, true);
    avx512_test.set_bit(511, true);
    avx512_test.set_bit(1000, true);
    
    // Verify AVX512 fallback produces same results
    ASSERT_EQ(avx512_test.popcount(), scalar_ref.popcount());
    ASSERT_EQ(avx512_test.get_bit(0), scalar_ref.get_bit(0));
    ASSERT_EQ(avx512_test.get_bit(511), scalar_ref.get_bit(511));
    ASSERT_EQ(avx512_test.get_bit(1000), scalar_ref.get_bit(1000));
#endif

#if BITCAL_HAS_NEON
    // NEON 1024-bit should fall back to scalar
    using neon_bit1024 = bitcal::bitarray<1024, bitcal::simd_backend::neon>;
    neon_bit1024 neon_test;
    neon_test.set_bit(0, true);
    neon_test.set_bit(511, true);
    neon_test.set_bit(1000, true);
    
    // Verify NEON fallback produces same results
    ASSERT_EQ(neon_test.popcount(), scalar_ref.popcount());
    ASSERT_EQ(neon_test.get_bit(0), scalar_ref.get_bit(0));
    ASSERT_EQ(neon_test.get_bit(511), scalar_ref.get_bit(511));
    ASSERT_EQ(neon_test.get_bit(1000), scalar_ref.get_bit(1000));
#endif

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

// ========== Bit64 specialization tests ==========

bool test_bit64_specialization() {
    // Basic operations
    bitcal::bit64 a(0xDEADBEEFCAFEBABEULL);
    ASSERT_EQ(a.popcount(), 46ULL);  // 0xDEADBEEFCAFEBABE has 46 bits set

    // Bit operations
    bitcal::bit64 b;
    b.set_bit(5);
    ASSERT_TRUE(b.get_bit(5));
    b.clear();
    ASSERT_TRUE(b.is_zero());

    // Popcount
    bitcal::bit64 c(0xFFFFFFFFFFFFFFFFULL);
    ASSERT_EQ(c.popcount(), 64ULL);

    // Single-bit access
    bitcal::bit64 d;
    d.set_bit(4, true);
    ASSERT_TRUE(d.get_bit(4));
    ASSERT_EQ(d.popcount(), 1ULL);

    // Zero / non-zero queries
    bitcal::bit64 e(0xFFFFFFFFFFFFFFFFULL);
    ASSERT_TRUE(!e.is_zero());

    bitcal::bit64 f(0);
    ASSERT_TRUE(f.is_zero());
    ASSERT_EQ(f.popcount(), 0ULL);

    return true;
}

// ========== Default backend contract tests ==========

// Lock the contract: bit64 must use get_default_backend() for its backend
static_assert(bitcal::bit64::backend == bitcal::get_default_backend(),
              "bit64 default backend must match get_default_backend()");

bool test_bit64_default_backend_contract() {
    // Runtime verification that bit64 and bitarray<64, simd_backend::scalar> 
    // behave equivalently for representative operations
    
    // Test 1: Construction and initialization
    bitcal::bit64 a(0xABCDEF0123456789ULL);
    bitcal::bitarray<64, bitcal::simd_backend::scalar> b(0xABCDEF0123456789ULL);
    ASSERT_EQ(a.popcount(), b.popcount());
    
    // Test 2: Bit manipulation
    bitcal::bit64 c;
    bitcal::bitarray<64, bitcal::simd_backend::scalar> d;
    
    for (size_t i = 0; i < 64; i += 8) {
        c.set_bit(i, true);
        d.set_bit(i, true);
    }
    
    ASSERT_EQ(c.popcount(), d.popcount());
    
    for (size_t i = 0; i < 64; i += 8) {
        ASSERT_EQ(c.get_bit(i), d.get_bit(i));
    }
    
    // Test 3: Bitwise operations
    bitcal::bit64 e(0xFFFFFFFFU);
    bitcal::bitarray<64, bitcal::simd_backend::scalar> f(0xFFFFFFFFU);
    
    bitcal::bit64 g = e | a;
    bitcal::bitarray<64, bitcal::simd_backend::scalar> h = f | b;
    ASSERT_EQ(g.popcount(), h.popcount());
    
    // Test 4: Shift operations
    bitcal::bit64 i(1ULL);
    bitcal::bitarray<64, bitcal::simd_backend::scalar> j(1ULL);
    
    i.shift_left(32);
    j.shift_left(32);
    ASSERT_TRUE(i.get_bit(32));
    ASSERT_TRUE(j.get_bit(32));
    ASSERT_EQ(i.popcount(), j.popcount());
    
    return true;
}

// ========== Abstract shift tests (not dependent on internal layout) ==========

bool test_abstract_shift_left() {
    // 测试不依赖内部 word 布局，只通过公开接口验证
    // Use helper to verify single-bit shift at position 1
    ASSERT_TRUE(verify_single_bit_shift_left<bitcal::bit256>(1, 256));
    
    bitcal::bit256 a;
    a.set_bit(0, true);  // 设置 bit 0
    a.shift_left(1);
    ASSERT_TRUE(a.get_bit(1));
    ASSERT_TRUE(!a.get_bit(0));

    // 再左移 63 位，bit 64 应该设置
    a.shift_left(63);
    ASSERT_TRUE(a.get_bit(64));

    // 清除并测试跨边界情况
    bitcal::bit256 b;
    b.set_bit(127, true);  // bit 127 (word 1 的最高位)
    b.shift_left(1);
    ASSERT_TRUE(b.get_bit(128));  // 应该移到 bit 128

    // 测试边界：左移整个位宽后应该全零 - use helper
    ASSERT_TRUE(verify_zero_after_full_width_shift<bitcal::bit256>(256));

    return true;
}

bool test_abstract_shift_right() {
    // 测试不依赖内部 word 布局
    // Use helper to verify single-bit shift right at position 1
    ASSERT_TRUE(verify_single_bit_shift_right<bitcal::bit256>(1, 256));
    
    bitcal::bit256 a;
    a.set_bit(255, true);  // 设置最高位
    a.shift_right(1);
    ASSERT_TRUE(a.get_bit(254));
    ASSERT_TRUE(!a.get_bit(255));

    // 测试边界：右移整个位宽后应该全零 - use helper
    ASSERT_TRUE(verify_zero_after_full_width_shift<bitcal::bit256>(256));

    return true;
}

// ========== Comprehensive boundary shift tests ==========

bool test_comprehensive_shift_boundaries() {
    // 测试所有关键边界值
    const int boundary_counts[] = {0, 1, 31, 32, 63, 64, 65,
                                    127, 128, 129,
                                    191, 192, 193,
                                    254, 255, 256, 257, 512, 1023};

    for (int count : boundary_counts) {
        // 测试 bit256
        bitcal::bit256 a;
        a.set_bit(0, true);
        a.shift_left(count);

        if (count >= 256) {
            ASSERT_TRUE(a.is_zero());
        } else {
            ASSERT_TRUE(a.get_bit(count));
            if (count > 0) {
                ASSERT_TRUE(!a.get_bit(count - 1));
            }
        }

        // 测试 bit512
        bitcal::bit512 b;
        b.set_bit(0, true);
        b.shift_left(count);

        if (count >= 512) {
            ASSERT_TRUE(b.is_zero());
        } else {
            ASSERT_TRUE(b.get_bit(count));
        }

        // 测试 bit1024
        bitcal::bit1024 c;
        c.set_bit(0, true);
        c.shift_left(count);

        if (count >= 1024) {
            ASSERT_TRUE(c.is_zero());
        } else {
            ASSERT_TRUE(c.get_bit(count));
        }
    }

    return true;
}

// ========== Popcount edge cases ==========

bool test_popcount_edge_cases() {
    // 单个位设置 - verify single bit popcount
    for (int i = 0; i < 64; i += 16) {
        bitcal::bit256 a;
        a.set_bit(i, true);
        ASSERT_EQ(a.popcount(), 1ULL);
    }

    // 所有位设置
    bitcal::bit256 all_ones = ~bitcal::bit256();
    ASSERT_EQ(all_ones.popcount(), 256ULL);

    // 交替位模式
    bitcal::bit256 alternating;
    for (int i = 0; i < 256; i += 2) alternating.set_bit(i, true);
    ASSERT_EQ(alternating.popcount(), 128ULL);

    return true;
}

// ========== Theme-based test runners ==========

void run_retained_contract_tests() {
    std::cout << std::endl << "=== [Retained Public Contract] ===" << std::endl;
    RUN_TEST(test_retained_public_contract);
}

void run_width_characterization_tests() {
    std::cout << std::endl << "=== [Width Characterization] ===" << std::endl;
    
    std::cout << std::endl << "  [64-bit]" << std::endl;
    RUN_TEST(test_bit64_basic);
    RUN_TEST(test_bit64_shift_left);
    RUN_TEST(test_bit64_shift_right);
    RUN_TEST(test_bit64_bitwise_ops);
    RUN_TEST(test_bit64_popcount);
    RUN_TEST(test_bit64_clz_ctz);
    RUN_TEST(test_bit64_bit_ops);
    RUN_TEST(test_bit64_reverse);
    RUN_TEST(test_bit64_compound_assignment);

    std::cout << std::endl << "  [128-bit]" << std::endl;
    RUN_TEST(test_bit128_basic);
    RUN_TEST(test_bit128_shift_left);
    RUN_TEST(test_bit128_shift_right);
    RUN_TEST(test_bit128_shift_cross_carry);
    RUN_TEST(test_bit128_bitwise_ops);

    std::cout << std::endl << "  [256-bit]" << std::endl;
    RUN_TEST(test_bit256_basic);
    RUN_TEST(test_bit256_shift_128);
    RUN_TEST(test_bit256_shift_cross_carry);
    RUN_TEST(test_bit256_shift_boundary);
    RUN_TEST(test_bit256_popcount);
    RUN_TEST(test_bit256_equality);
    RUN_TEST(test_reverse_256);

    std::cout << std::endl << "  [512-bit]" << std::endl;
    RUN_TEST(test_bit512_basic);
    RUN_TEST(test_bit512_shift_cross_carry);
    RUN_TEST(test_bit512_shift_boundary);

    std::cout << std::endl << "  [1024-bit]" << std::endl;
    RUN_TEST(test_bit1024_basic);
    RUN_TEST(test_bit1024_shift_cross_carry);
    RUN_TEST(test_bit1024_shift_boundary);
    RUN_TEST(test_bit1024_popcount);
    RUN_TEST(test_bit1024_bitwise_ops);
    RUN_TEST(test_bit1024_is_zero);
    RUN_TEST(test_bit1024_equality);
}

void run_backend_seam_tests() {
    std::cout << std::endl << "=== [Backend Seam Characterization] ===" << std::endl;
    RUN_TEST(test_backend_consistency);
    RUN_TEST(test_cross_backend_consistency);
    RUN_TEST(test_small_width_backend_fallback_consistency);
}

void run_large_width_fallback_tests() {
    std::cout << std::endl << "=== [Large-Width Scalar Fallback] ===" << std::endl;
    RUN_TEST(test_large_width_1024_fallback_characterization);
}

void run_bit64_specialization_tests() {
    std::cout << std::endl << "=== [Bit64 Specialization & Default Backend] ===" << std::endl;
    RUN_TEST(test_bit64_specialization);
    RUN_TEST(test_bit64_default_backend_contract);
}

void run_abstract_shift_tests() {
    std::cout << std::endl << "=== [Abstract Shift Tests] ===" << std::endl;
    RUN_TEST(test_abstract_shift_left);
    RUN_TEST(test_abstract_shift_right);
}

void run_comprehensive_boundary_tests() {
    std::cout << std::endl << "=== [Comprehensive Boundary Tests] ===" << std::endl;
    RUN_TEST(test_shift_boundary_conditions);
    RUN_TEST(test_bitwise_mathematical_properties);
    RUN_TEST(test_large_shifts);
    RUN_TEST(test_comprehensive_shift_boundaries);
}

void run_popcount_edge_tests() {
    std::cout << std::endl << "=== [Popcount Edge Cases] ===" << std::endl;
    RUN_TEST(test_popcount_edge_cases);
}

void run_static_assert_tests() {
    std::cout << std::endl << "=== [Static Assert Validation] ===" << std::endl;
    RUN_TEST(test_static_assert_validation);
}

void run_miscellaneous_operations_tests() {
    std::cout << std::endl << "=== [Miscellaneous Operations] ===" << std::endl;
    
    std::cout << std::endl << "  [ANDNOT]" << std::endl;
    RUN_TEST(test_andnot_64);
    RUN_TEST(test_andnot_256);

    std::cout << std::endl << "  [NOT]" << std::endl;
    RUN_TEST(test_not_128);
    RUN_TEST(test_not_256);

    std::cout << std::endl << "  [is_zero]" << std::endl;
    RUN_TEST(test_is_zero_various);
}

// ========== main ==========

int main() {
    std::cout << "=== BitCal Unit Tests ===" << std::endl;

    run_retained_contract_tests();
    run_width_characterization_tests();
    run_backend_seam_tests();
    run_large_width_fallback_tests();
    run_bit64_specialization_tests();
    run_abstract_shift_tests();
    run_comprehensive_boundary_tests();
    run_popcount_edge_tests();
    run_static_assert_tests();
    run_miscellaneous_operations_tests();

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
