# 函数式接口 (ops 命名空间)

`bitcal::ops` 命名空间提供对原始 `uint64_t*` 指针的直接操作接口，适合不需要构造 `bitarray` 对象的场景。

## 位计数

### popcount

```cpp
template<size_t Bits>
uint64_t popcount(const uint64_t* data);
```

统计指定位宽数据中 1 的个数。

```cpp
uint64_t data[4] = {0xFF, 0xFF, 0, 0};
uint64_t ones = bitcal::ops::popcount<256>(data);  // 16
```

### count_leading_zeros

```cpp
template<size_t Bits>
int count_leading_zeros(const uint64_t* data);
```

统计前导零。

```cpp
uint64_t data[4] = {0, 0, 0, 0x8000000000000000};
int lz = bitcal::ops::count_leading_zeros<256>(data);  // 0（最高位为 1）
```

### count_trailing_zeros

```cpp
template<size_t Bits>
int count_trailing_zeros(const uint64_t* data);
```

统计尾部零。

```cpp
uint64_t data[4] = {1, 0, 0, 0};
int tz = bitcal::ops::count_trailing_zeros<256>(data);  // 0（最低位为 1）
```

## 工具函数

### reverse_bits_64

```cpp
uint64_t reverse_bits_64(uint64_t x);
```

反转单个 64 位整数的所有位。

```cpp
uint64_t reversed = bitcal::ops::reverse_bits_64(0xF0F0F0F0);
// reversed = 0x0F0F0F0F0F0F0F0F
```

### byte_swap_64

```cpp
uint64_t byte_swap_64(uint64_t x);
```

交换字节序（大端 ↔ 小端）。

```cpp
uint64_t swapped = bitcal::ops::byte_swap_64(0x0102030405060708ULL);
// swapped = 0x0807060504030201ULL
```

## 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    // 原始数据数组
    uint64_t data[4] = {0xFF00, 0x00FF, 0xF0F0, 0x0F0F};

    // 位计数
    std::cout << "popcount: " << bitcal::ops::popcount<256>(data) << std::endl;

    // 前导/尾部零
    std::cout << "CLZ: " << bitcal::ops::count_leading_zeros<256>(data) << std::endl;
    std::cout << "CTZ: " << bitcal::ops::count_trailing_zeros<256>(data) << std::endl;

    // 位反转
    uint64_t x = 0x123456789ABCDEF0ULL;
    std::cout << "reversed: 0x" << std::hex << bitcal::ops::reverse_bits_64(x) << std::endl;

    // 字节序交换
    std::cout << "swapped: 0x" << std::hex << bitcal::ops::byte_swap_64(x) << std::endl;

    return 0;
}
```

## 使用场景

- 与现有数据结构集成
- 不需要创建临时 `bitarray` 对象
- 批量处理多个位数组
- 底层库开发

## 性能

所有 `ops` 函数都是内联的，与 `bitarray` 成员函数性能相同。
