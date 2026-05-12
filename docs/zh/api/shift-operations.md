# 移位操作

## 左移

```cpp
void shift_left(int count);
bitarray operator<<(int count) const;
bitarray& operator<<=(int count);
```

将所有位向更高有效位置（左）移动，用零填充。移出宽度的位被丢弃。

**SIMD 分发**：
- 64 位：Scalar
- 128 位：SSE2 / NEON
- 256 位：AVX2 / SSE2(2×128) / NEON(2×128)
- 512 位：AVX2(2×256) / NEON(4×128)
- 其他：标量数组回退

## 右移

```cpp
void shift_right(int count);
bitarray operator>>(int count) const;
bitarray& operator>>=(int count);
```

将所有位向更低有效位置（右）移动，用零填充（逻辑右移）。

## 边界行为

| 条件 | 行为 |
|------|------|
| `count == 0` | 无操作 |
| `count >= Bits` | 所有位清零 |
| `count < 0` | 未定义行为 |

## 示例

```cpp
bitcal::bit256 a;
a[0] = 0x8000000000000000ULL;  // 位 63 = 1

a.shift_left(1);
// a[0] == 0, a[1] == 1  （进位正确传播到下一个字）

bitcal::bit256 b(1);
b <<= 255;
// b[3] == 0x8000000000000000  （最高有效位）
```

## 实现细节

移位操作分两阶段执行：
1. **字级移位**（count ≥ 64）：标量整字移动
2. **位级移位**（count < 64）：SIMD 并行移位 + 进位传播

### AVX2 256 位左移示例

```
阶段 1：字移位（标量）
  data[3] = data[2], data[2] = data[1], data[1] = data[0], data[0] = 0

阶段 2：位移位（AVX2）
  shifted = _mm256_slli_epi64(v, count)          // 每个 qword 独立移位
  carry   = _mm256_permute4x64_epi64(v, 0x93)   // 旋转：[q3,q2,q1,q0] -> [q0,q3,q2,q1]
  carry   = _mm256_srli_epi64(carry, 64-count)  // 提取进位位
  carry   = _mm256_blend_epi32(carry, zero, 0x03) // 清除最低 qword 中的进位
  result  = shifted | carry
```

## 为什么不用 `_mm256_slli_si256`？

`_mm256_slli_si256` **在 128 位通道上独立操作**，不会跨通道移位。这是一个常见的 AVX2 陷阱。BitCal v2.1 通过使用标量字移位 + SIMD 位移位策略修复了这个问题。

## 性能说明

- 64 的倍数移位最快（纯字移动）
- 跨字进位增加开销但为保证正确性所必需
- AVX2 256 位移位对大多数移位量明显快于标量

## 另见

- [类型](types.md) — 可用的 bitarray 类型
- [架构](../architecture/simd-dispatch.md) — SIMD 分发详情
