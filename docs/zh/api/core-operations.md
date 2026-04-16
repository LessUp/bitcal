# 核心操作

## 位运算

所有位运算自动分派到最优 SIMD 实现。

### AND

```cpp
bitarray operator&(const bitarray& other) const;
bitarray& operator&=(const bitarray& other);
```

按位与运算。

### OR

```cpp
bitarray operator|(const bitarray& other) const;
bitarray& operator|=(const bitarray& other);
```

按位或运算。

### XOR

```cpp
bitarray operator^(const bitarray& other) const;
bitarray& operator^=(const bitarray& other);
```

按位异或运算。

### NOT

```cpp
bitarray operator~() const;
```

按位取反。

**SIMD 加速** (v2.1+):
- SSE: `_mm_xor_si128`
- AVX: `_mm256_xor_si256`
- NEON: `veorq_u64`

### ANDNOT

```cpp
bitarray andnot(const bitarray& mask) const;
```

计算 `*this & ~mask`，比分开执行 `~mask` 再 `&` 更快。

**原生 SIMD 指令**:
| 后端 | 指令 |
|------|------|
| SSE | `_mm_andnot_si128` |
| AVX | `_mm256_andnot_si256` |
| NEON | `vbicq_u64` |

**示例**:

```cpp
bitcal::bit256 data(0xFFFF0000FFFF0000);
bitcal::bit256 mask(0xFF00FF00FF00FF00);

auto result = data.andnot(mask);
// 等价于 data & ~mask
// = 0xFFFF0000FFFF0000 & 0x00FF00FF00FF00FF
// = 0x00FF000000FF0000
```

## 比较

### 相等

```cpp
bool operator==(const bitarray& other) const;
```

### 不等

```cpp
bool operator!=(const bitarray& other) const;
```

## 状态检测

### is_zero

```cpp
bool is_zero() const;
```

检测所有位是否都为零。

**SIMD 加速** (v2.1+):
- SSE: `_mm_movemask_epi8` + `_mm_cmpeq_epi8`
- AVX: `_mm256_testz_si256`
- NEON: `vceqq_u64` + lane 检查

```cpp
bitcal::bit256 arr;
if (arr.is_zero()) {
    std::cout << "All bits are zero\n";
}
```

### clear

```cpp
void clear();
```

将所有位清零。内部使用 `std::memset`。

```cpp
bitcal::bit256 arr(0xFFFFFFFF);
arr.clear();  // 所有位变为 0
```

## 完整示例

```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    bitcal::bit256 a(0xFF00);
    bitcal::bit256 b(0x0FF0);

    // 基本位运算
    auto and_result = a & b;    // 0x0F00
    auto or_result  = a | b;    // 0xFFF0
    auto xor_result = a ^ b;    // 0xF0F0
    auto not_result = ~a;       // ~0xFF00

    // ANDNOT
    auto andnot_result = a.andnot(b);  // a & ~b = 0xF000

    // 复合赋值
    a &= b;
    a |= b;
    a ^= b;

    // 比较
    if (a == b) {
        std::cout << "a equals b\n";
    }

    // 状态检测
    a.clear();
    if (a.is_zero()) {
        std::cout << "a is zero\n";
    }

    return 0;
}
```

## 性能提示

1. **使用 ANDNOT** — 比分开的 NOT + AND 快约 2 倍
2. **使用 is_zero** — SIMD 加速，比手动循环检查快
3. **使用复合赋值** — `a &= b` 可能比 `a = a & b` 更高效（避免临时对象）
