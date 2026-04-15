# 快速上手

## 你的第一个 BitCal 程序

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // 创建 256 位的位数组
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    // 位运算
    std::cout << "AND: " << (a & b).popcount() << " bits set\n";
    std::cout << "OR:  " << (a | b).popcount() << " bits set\n";
    std::cout << "XOR: " << (a ^ b).popcount() << " bits differ\n";

    return 0;
}
```

编译运行：

```bash
g++ -std=c++17 -O2 -march=native main.cpp -o main && ./main
```

输出：
```
AND: 11 bits set
OR:  24 bits set
XOR: 13 bits differ
```

## 核心操作

### 位运算

```cpp
bitcal::bit256 a(0xFF), b(0xAA);

auto and_result = a & b;       // AND
auto or_result  = a | b;       // OR
auto xor_result = a ^ b;       // XOR
auto not_result = ~a;          // NOT
auto andnot     = a.andnot(b); // a & ~b（原生 SIMD 指令）
```

### 位移

```cpp
bitcal::bit256 arr(1);

arr <<= 10;    // 左移 10 位
arr.shift_left(128);  // 左移 128 位（跨 word）

arr >>= 5;     // 右移 5 位
arr.shift_right(64);  // 右移 64 位
```

### 位计数

```cpp
bitcal::bit256 arr(0xF0F0F0F0);

uint64_t ones = arr.popcount();              // 统计 1 的个数
int lz = arr.count_leading_zeros();          // 前导零
int tz = arr.count_trailing_zeros();         // 尾部零
```

### 单个位操作

```cpp
bitcal::bit256 arr;

arr.set_bit(42, true);   // 设置第 42 位为 1
arr.set_bit(100, false); // 设置第 100 位为 0

bool b = arr.get_bit(42);  // 读取第 42 位
arr.flip_bit(42);          // 翻转第 42 位
```

### 其他操作

```cpp
bitcal::bit256 arr(0x123456789ABCDEF0);

arr.reverse();        // 位反转
arr.clear();          // 清零所有位

if (arr.is_zero()) {  // 零检测（SIMD 加速）
    // ...
}

// 直接访问底层数据
uint64_t* data = arr.data();
data[0] = 0xFFFFFFFFFFFFFFFF;
```

## 可用类型

| 类型 | 位宽 | 用途 |
|------|------|------|
| `bitcal::bit64` | 64 | 单个机器字 |
| `bitcal::bit128` | 128 | SSE2/NEON 原生宽度 |
| `bitcal::bit256` | 256 | AVX2 原生宽度 |
| `bitcal::bit512` | 512 | 大位宽操作 |
| `bitcal::bit1024` | 1024 | 超大位宽 |

## 自定义位宽

```cpp
// 任意位宽（必须是 64 的倍数）
bitcal::bitarray<2048> huge;
huge[0] = 0x123456789ABCDEF0;
huge.shift_left(128);
```

## 指定 SIMD 后端

```cpp
// 默认：编译器自动选择最优后端
bitcal::bit256 auto_selected;

// 强制使用 AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> avx2_arr;

// 强制使用 NEON
bitcal::bitarray<128, bitcal::simd_backend::neon> neon_arr;

// 强制使用标量（最大可移植性）
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable_arr;
```

## 低级 API

`ops` 命名空间提供直接操作原始指针的函数：

```cpp
uint64_t data[4] = {1, 2, 3, 4};

// 函数式 API
uint64_t ones = bitcal::ops::popcount<256>(data);
int lz = bitcal::ops::count_leading_zeros<256>(data);
int tz = bitcal::ops::count_trailing_zeros<256>(data);

// 工具函数
uint64_t reversed = bitcal::ops::reverse_bits_64(0xF0F0F0F0);
uint64_t swapped = bitcal::ops::byte_swap_64(0x123456789ABCDEF0);
```

## 下一步

- [类型别名](../api/types.md) — 了解所有可用类型
- [核心操作](../api/core-operations.md) — 完整 API 参考
- [位移操作](../api/shift-operations.md) — 位移详解
