# 位移操作

## 左移

```cpp
void shift_left(int count);
bitarray operator<<(int count) const;
bitarray& operator<<=(int count);
```

将所有位向高位方向移动 `count` 位，低位补零。超出位宽的部分被丢弃。

**SIMD 分派**:
- 64-bit: 标量
- 128-bit: SSE2 / NEON
- 256-bit: AVX2 / SSE2(2×128) / NEON(2×128)
- 512-bit: AVX2(2×256) / NEON(4×128)
- 其他: 标量 array 实现

## 右移

```cpp
void shift_right(int count);
bitarray operator>>(int count) const;
bitarray& operator>>=(int count);
```

将所有位向低位方向移动 `count` 位，高位补零（逻辑右移）。

## 边界行为

| 条件 | 行为 |
|------|------|
| `count == 0` | 无操作 |
| `count >= Bits` | 所有位清零 |
| `count < 0` | 未定义行为 |

## 示例

```cpp
bitcal::bit256 a;
a[0] = 0x8000000000000000ULL;  // bit 63 = 1

a.shift_left(1);
// a[0] == 0, a[1] == 1  （carry 正确传播到下一个 word）

bitcal::bit256 b(1);
b <<= 255;
// b[3] == 0x8000000000000000  （最高位）
```

## 实现说明

位移操作分两阶段执行：
1. **Word 级移位**（count ≥ 64）：标量整 word 搬移
2. **Bit 级移位**（count < 64）：SIMD 并行移位 + carry 传播

v2.1 修复了 SSE/AVX 实现中跨 128-bit lane 和跨 256-bit 边界的 carry 传播 bug。
