#include <bitcal/bitcal.hpp>
#include <iostream>
#include <iomanip>

void print_hex(const char* name, const bitcal::bit256& arr) {
    std::cout << name << ": ";
    for (int i = 3; i >= 0; --i) {
        std::cout << std::hex << std::setw(16) << std::setfill('0') << arr[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "=== BitCal 基本使用示例 ===" << std::endl << std::endl;
    
    bitcal::bit256 a(0xDEADBEEFCAFEBABE);
    bitcal::bit256 b(0x0123456789ABCDEF);
    
    std::cout << "初始值:" << std::endl;
    print_hex("a", a);
    print_hex("b", b);
    std::cout << std::endl;
    
    std::cout << "位运算操作:" << std::endl;
    auto c = a & b;
    print_hex("a & b", c);
    
    auto d = a | b;
    print_hex("a | b", d);
    
    auto e = a ^ b;
    print_hex("a ^ b", e);
    
    auto f = ~a;
    print_hex("~a", f);
    std::cout << std::endl;
    
    std::cout << "位移操作:" << std::endl;
    auto g = a << 32;
    print_hex("a << 32", g);
    
    auto h = a >> 16;
    print_hex("a >> 16", h);
    std::cout << std::endl;
    
    std::cout << "位计数操作:" << std::endl;
    std::cout << "popcount(a): " << a.popcount() << std::endl;
    std::cout << "count_leading_zeros(a): " << a.count_leading_zeros() << std::endl;
    std::cout << "count_trailing_zeros(a): " << a.count_trailing_zeros() << std::endl;
    std::cout << std::endl;
    
    std::cout << "位操作:" << std::endl;
    bitcal::bit256 arr;
    arr.set_bit(0);
    arr.set_bit(63);
    arr.set_bit(128);
    arr.set_bit(255);
    print_hex("设置位 0,63,128,255", arr);
    
    std::cout << "bit[0] = " << arr.get_bit(0) << std::endl;
    std::cout << "bit[1] = " << arr.get_bit(1) << std::endl;
    std::cout << "bit[255] = " << arr.get_bit(255) << std::endl;
    std::cout << std::endl;
    
    std::cout << "位反转:" << std::endl;
    bitcal::bit64 small(0x123456789ABCDEF0);
    std::cout << "原始: 0x" << std::hex << small[0] << std::endl;
    small.reverse();
    std::cout << "反转: 0x" << std::hex << small[0] << std::dec << std::endl;
    
    return 0;
}
