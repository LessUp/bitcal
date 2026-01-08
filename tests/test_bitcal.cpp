#include <bitcal/bitcal.hpp>
#include <cassert>
#include <iostream>

#define TEST(name) \
    void test_##name(); \
    struct test_##name##_runner { \
        test_##name##_runner() { \
            std::cout << "Running test: " #name << "..." << std::endl; \
            test_##name(); \
            std::cout << "  PASSED" << std::endl; \
        } \
    } test_##name##_instance; \
    void test_##name()

TEST(bit64_basic) {
    bitcal::bit64 a(0xDEADBEEFCAFEBABE);
    assert(a[0] == 0xDEADBEEFCAFEBABE);
    assert(!a.is_zero());
    
    a.clear();
    assert(a.is_zero());
    assert(a[0] == 0);
}

TEST(bit64_shift_left) {
    bitcal::bit64 a(1);
    a.shift_left(10);
    assert(a[0] == 1024);
    
    bitcal::bit64 b(0x8000000000000000ULL);
    b.shift_left(1);
    assert(b[0] == 0);
}

TEST(bit64_shift_right) {
    bitcal::bit64 a(1024);
    a.shift_right(10);
    assert(a[0] == 1);
    
    bitcal::bit64 b(1);
    b.shift_right(1);
    assert(b[0] == 0);
}

TEST(bit64_bitwise_ops) {
    bitcal::bit64 a(0xFF00FF00FF00FF00);
    bitcal::bit64 b(0xF0F0F0F0F0F0F0F0);
    
    auto c = a & b;
    assert(c[0] == 0xF000F000F000F000);
    
    auto d = a | b;
    assert(d[0] == 0xFFF0FFF0FFF0FFF0);
    
    auto e = a ^ b;
    assert(e[0] == 0x0FF00FF00FF00FF0);
    
    auto f = ~a;
    assert(f[0] == 0x00FF00FF00FF00FF);
}

TEST(bit64_popcount) {
    bitcal::bit64 a(0);
    assert(a.popcount() == 0);
    
    bitcal::bit64 b(0xFFFFFFFFFFFFFFFF);
    assert(b.popcount() == 64);
    
    bitcal::bit64 c(0xAAAAAAAAAAAAAAAA);
    assert(c.popcount() == 32);
}

TEST(bit64_leading_trailing_zeros) {
    bitcal::bit64 a(0);
    assert(a.count_leading_zeros() == 64);
    assert(a.count_trailing_zeros() == 64);
    
    bitcal::bit64 b(1);
    assert(b.count_leading_zeros() == 63);
    assert(b.count_trailing_zeros() == 0);
    
    bitcal::bit64 c(0x8000000000000000ULL);
    assert(c.count_leading_zeros() == 0);
    assert(c.count_trailing_zeros() == 63);
}

TEST(bit64_bit_operations) {
    bitcal::bit64 a;
    
    a.set_bit(0);
    assert(a.get_bit(0) == true);
    assert(a[0] == 1);
    
    a.set_bit(63);
    assert(a.get_bit(63) == true);
    assert(a[0] == 0x8000000000000001ULL);
    
    a.set_bit(0, false);
    assert(a.get_bit(0) == false);
    assert(a[0] == 0x8000000000000000ULL);
    
    a.flip_bit(63);
    assert(a.get_bit(63) == false);
    assert(a[0] == 0);
}

TEST(bit128_basic) {
    bitcal::bit128 a(0xDEADBEEF);
    assert(a[0] == 0xDEADBEEF);
    assert(a[1] == 0);
}

TEST(bit128_shift_left) {
    bitcal::bit128 a;
    a[0] = 0xFFFFFFFFFFFFFFFF;
    a[1] = 0;
    
    a.shift_left(64);
    assert(a[0] == 0);
    assert(a[1] == 0xFFFFFFFFFFFFFFFF);
    
    bitcal::bit128 b;
    b[0] = 1;
    b[1] = 0;
    b.shift_left(10);
    assert(b[0] == 1024);
    assert(b[1] == 0);
}

TEST(bit128_shift_right) {
    bitcal::bit128 a;
    a[0] = 0;
    a[1] = 0xFFFFFFFFFFFFFFFF;
    
    a.shift_right(64);
    assert(a[0] == 0xFFFFFFFFFFFFFFFF);
    assert(a[1] == 0);
}

TEST(bit128_bitwise_ops) {
    bitcal::bit128 a;
    a[0] = 0xFF00FF00FF00FF00;
    a[1] = 0x00FF00FF00FF00FF;
    
    bitcal::bit128 b;
    b[0] = 0xF0F0F0F0F0F0F0F0;
    b[1] = 0x0F0F0F0F0F0F0F0F;
    
    auto c = a & b;
    assert(c[0] == 0xF000F000F000F000);
    assert(c[1] == 0x000F000F000F000F);
    
    auto d = a | b;
    assert(d[0] == 0xFFF0FFF0FFF0FFF0);
    assert(d[1] == 0x0FFF0FFF0FFF0FFF);
}

TEST(bit256_basic) {
    bitcal::bit256 a(0x123456789ABCDEF0);
    assert(a[0] == 0x123456789ABCDEF0);
    assert(a[1] == 0);
    assert(a[2] == 0);
    assert(a[3] == 0);
}

TEST(bit256_shift) {
    bitcal::bit256 a;
    a[0] = 0xFFFFFFFFFFFFFFFF;
    a.shift_left(128);
    assert(a[0] == 0);
    assert(a[1] == 0);
    assert(a[2] == 0xFFFFFFFFFFFFFFFF);
    assert(a[3] == 0);
}

TEST(bit256_popcount) {
    bitcal::bit256 a;
    for (size_t i = 0; i < 4; ++i) {
        a[i] = 0xFFFFFFFFFFFFFFFF;
    }
    assert(a.popcount() == 256);
}

TEST(bit512_basic) {
    bitcal::bit512 a(0xDEADBEEF);
    assert(a[0] == 0xDEADBEEF);
    for (size_t i = 1; i < 8; ++i) {
        assert(a[i] == 0);
    }
}

TEST(equality) {
    bitcal::bit256 a(123);
    bitcal::bit256 b(123);
    bitcal::bit256 c(456);
    
    assert(a == b);
    assert(a != c);
    assert(b != c);
}

TEST(compound_assignment) {
    bitcal::bit64 a(0xFF00);
    bitcal::bit64 b(0xF0F0);
    
    a &= b;
    assert(a[0] == 0xF000);
    
    a = bitcal::bit64(0xFF00);
    a |= b;
    assert(a[0] == 0xFFF0);
    
    a = bitcal::bit64(0xFF00);
    a ^= b;
    assert(a[0] == 0x0FF0);
    
    a = bitcal::bit64(1);
    a <<= 10;
    assert(a[0] == 1024);
    
    a >>= 5;
    assert(a[0] == 32);
}

TEST(reverse_bits) {
    bitcal::bit64 a(0x0000000000000001);
    a.reverse();
    assert(a[0] == 0x8000000000000000ULL);
    
    bitcal::bit64 b(0x00000000000000FF);
    b.reverse();
    assert(b[0] == 0xFF00000000000000ULL);
}

int main() {
    std::cout << "=== BitCal 单元测试 ===" << std::endl << std::endl;
    std::cout << std::endl << "所有测试通过!" << std::endl;
    return 0;
}
