# 位计数

## popcount — 统计 1 的个数

```cpp
uint64_t popcount() const;
```

统计所有位中值为 1 的位数。

**实现**:
- 使用 `__builtin_popcountll`（GCC/Clang）或 `__popcnt64`（MSVC x64）
- 对 Bits > 64 逐 word 累加

**示例**:

```cpp
bitcal::bit256 a;
for (size_t i = 0; i < 4; ++i) a[i] = 0xFFFFFFFFFFFFFFFF;
assert(a.popcount() == 256);

bitcal::bit64 b(0xAAAAAAAAAAAAAAAA);
assert(b.popcount() == 32);
```

## count_leading_zeros — 前导零计数 (CLZ)

```cpp
int count_leading_zeros() const;
```

从最高位开始，统计连续的 0 的个数。

| 输入 | 返回值 |
|------|--------|
| 全零 | Bits |
| 最高位为 1 | 0 |

**实现**: 使用 `__builtin_clzll`（GCC/Clang）或 `_BitScanReverse64`（MSVC）

## count_trailing_zeros — 尾部零计数 (CTZ)

```cpp
int count_trailing_zeros() const;
```

从最低位开始，统计连续的 0 的个数。

| 输入 | 返回值 |
|------|--------|
| 全零 | Bits |
| 最低位为 1 | 0 |

**实现**: 使用 `__builtin_ctzll`（GCC/Clang）或 `_BitScanForward64`（MSVC）

## 函数式接口

```cpp
// 对原始 uint64_t 数组操作
uint64_t bitcal::ops::popcount<256>(const uint64_t* data);
int bitcal::ops::count_leading_zeros<256>(const uint64_t* data);
int bitcal::ops::count_trailing_zeros<256>(const uint64_t* data);
```
