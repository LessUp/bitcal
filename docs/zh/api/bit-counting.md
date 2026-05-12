# 位计数

BitCal 3.0 保留的位计数成员函数参考。

> **BitCal 3.0 迁移说明：** 公共 `bitcal::ops::{popcount,count_leading_zeros,count_trailing_zeros}` 辅助函数已移除。请将数据包装在 `bitarray` 中并调用此处记录的成员函数。

## popcount — 计数置位

```cpp
uint64_t popcount() const noexcept;
```

计算设为 `1` 的位数。

**实现：**
- 每个 64 位字使用标量内置函数
- 委托给所选后端进行累加

**示例：**

```cpp
bitcal::bit256 a;
a.set_word(0, 0xFFFFFFFFFFFFFFFFULL);
a.set_word(1, 0xFFFFFFFFFFFFFFFFULL);
a.set_word(2, 0xFFFFFFFFFFFFFFFFULL);
a.set_word(3, 0xFFFFFFFFFFFFFFFFULL);
assert(a.popcount() == 256);

bitcal::bit64 b(0xAAAAAAAAAAAAAAAAULL);
assert(b.popcount() == 32);
```

## count_leading_zeros — 前导零计数 (CLZ)

```cpp
int count_leading_zeros() const noexcept;
```

从最高有效位开始计数连续的零位。

| 输入 | 返回值 |
|------|--------|
| 全零 | `Bits` |
| MSB 为 1 | `0` |

## count_trailing_zeros — 后导零计数 (CTZ)

```cpp
int count_trailing_zeros() const noexcept;
```

从最低有效位开始计数连续的零位。

| 输入 | 返回值 |
|------|--------|
| 全零 | `Bits` |
| LSB 为 1 | `0` |

## 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 arr;
    arr.set_word(0, 0x0000000000000100ULL);
    arr.set_word(3, 0x8000000000000000ULL);

    std::cout << "Popcount: " << arr.popcount() << "\n";
    std::cout << "Leading zeros: " << arr.count_leading_zeros() << "\n";
    std::cout << "Trailing zeros: " << arr.count_trailing_zeros() << "\n";

    bitcal::bit256 zero;
    std::cout << "CLZ (all zeros): " << zero.count_leading_zeros() << "\n";
    std::cout << "CTZ (all zeros): " << zero.count_trailing_zeros() << "\n";

    return 0;
}
```

## 性能特征

| 操作 | 复杂度 | 备注 |
|------|--------|------|
| `popcount()` | `O(Bits / 64)` | 每个 64 位字计数一次 |
| `count_leading_zeros()` | `O(Bits / 64)` | 从最高字开始扫描 |
| `count_trailing_zeros()` | `O(Bits / 64)` | 从最低字开始扫描 |

## 平台说明

- GCC/Clang 构建在可用时使用内置位计数操作
- MSVC 构建在实现层使用匹配的标量内联函数
- 返回值是位宽感知的，对所有支持的 `bitarray<Bits>` 特化都有效

## 迁移说明

如果您之前使用已移除的 `bitcal::ops` API 对原始 `uint64_t*` 缓冲区计数位，请将缓冲区适配为 `bitarray` 并调用成员函数：

```cpp
bitcal::bit256 arr;
for (size_t i = 0; i < bitcal::bit256::u64_count; ++i) {
    arr.set_word(i, source[i]);
}

uint64_t ones = arr.popcount();
```
