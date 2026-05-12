# 核心操作

BitCal 3.0 保留的核心位操作参考。

> **BitCal 3.0 迁移说明：** 所有公共核心操作通过 `bitarray` 运算符和成员函数表达。已移除的 `bitcal::ops` 命名空间不再是支持的公共 API。

## 位运算

所有位运算分发到 `bitarray` 特化所选的后端。

### AND

```cpp
bitarray operator&(const bitarray& other) const noexcept;
bitarray& operator&=(const bitarray& other) noexcept;
```

### OR

```cpp
bitarray operator|(const bitarray& other) const noexcept;
bitarray& operator|=(const bitarray& other) noexcept;
```

### XOR

```cpp
bitarray operator^(const bitarray& other) const noexcept;
bitarray& operator^=(const bitarray& other) noexcept;
```

### NOT

```cpp
bitarray operator~() const noexcept;
```

对每一位进行位反转。

### ANDNOT

```cpp
bitarray andnot(const bitarray& mask) const noexcept;
```

使用所选后端实现计算 `*this & ~mask`。

## 比较

```cpp
bool operator==(const bitarray& other) const noexcept;
bool operator!=(const bitarray& other) const noexcept;
```

## 状态检测

### is_zero

```cpp
bool is_zero() const noexcept;
```

当所有位都清零时返回 `true`。

### clear

```cpp
void clear() noexcept;
```

将整个对象清零。

## 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 a(0xFF00);
    bitcal::bit256 b(0x0FF0);

    auto and_result = a & b;
    auto or_result = a | b;
    auto xor_result = a ^ b;
    auto not_result = ~a;
    auto andnot_result = a.andnot(b);

    a &= b;
    a |= b;
    a ^= b;

    if (and_result != or_result && !not_result.is_zero()) {
        std::cout << "core operations succeeded\n";
    }

    andnot_result.is_zero();
    a.clear();
    std::cout << std::boolalpha << a.is_zero() << "\n";
    return 0;
}
```

## 性能提示

1. 当意图匹配时，优先使用 `andnot()` 而非 `a & ~b`
2. 就地更新现有对象时，优先使用复合赋值
3. 保持值为 `bitarray` 形式，而非降级为临时原始指针辅助函数
