# 快速上手

## 基本用法

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // 创建 256 位的位数组
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    // 位运算
    auto c = a & b;  // AND
    auto d = a | b;  // OR
    auto e = a ^ b;  // XOR
    auto f = ~a;     // NOT

    // ANDNOT: a & ~b，利用原生 SIMD 指令
    auto g = a.andnot(b);

    // 位移操作
    a <<= 10;  // 左移 10 位
    b >>= 5;   // 右移 5 位

    // 位计数
    uint64_t ones = a.popcount();
    int clz = a.count_leading_zeros();
    int ctz = a.count_trailing_zeros();

    // 单个位操作
    a.set_bit(42, true);
    bool bit = a.get_bit(42);
    a.flip_bit(42);

    // 检测与清零
    if (!a.is_zero()) {
        a.clear();
    }

    // 位反转
    a.reverse();

    std::cout << "Popcount: " << ones << std::endl;
    return 0;
}
```

## 可用类型

| 类型 | 位宽 | 说明 |
|------|------|------|
| `bitcal::bit64` | 64 | 单个 uint64_t |
| `bitcal::bit128` | 128 | SSE2/NEON 原生宽度 |
| `bitcal::bit256` | 256 | AVX2 原生宽度 |
| `bitcal::bit512` | 512 | 两个 AVX2 或四个 NEON |
| `bitcal::bit1024` | 1024 | 大位宽 |

## 自定义位宽

```cpp
bitcal::bitarray<2048> huge;
huge[0] = 0x123456789ABCDEF0;
huge.shift_left(128);
```

## 指定 SIMD 后端

```cpp
// 默认：编译器自动选择最优后端
bitcal::bit256 auto_opt;

// 强制使用 AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> avx2_opt;

// 强制使用标量（可移植）
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable;
```

## 低级操作

```cpp
// 直接访问底层 uint64_t 数组
uint64_t* data = arr.data();

// 使用函数式 API（ops 命名空间）
uint64_t result = bitcal::ops::popcount<256>(data);
int clz = bitcal::ops::count_leading_zeros<256>(data);
```
