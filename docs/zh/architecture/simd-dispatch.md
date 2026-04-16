# SIMD 分发机制

## 编译期分派

BitCal 使用 C++17 的 `if constexpr` 在编译期选择最优实现路径。整个分派过程不产生任何运行时开销。

### 二元操作分派

v2.1 将 `bit_and`/`bit_or`/`bit_xor`/`bit_andnot` 四种操作合并为统一的模板分派：

```cpp
enum class binop { op_and, op_or, op_xor, op_andnot };

template<binop Op>
static void dispatch_binop(const bitarray& a, const bitarray& b, bitarray& out) {
    if constexpr (Bits == 64) {
        // 标量 64-bit 操作
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        // AVX2 256-bit 操作
    }
    // ...其他后端
    else {
        // 标量 array 兜底
    }
}
```

### 位移操作分派

位移操作在 `shift_left()` / `shift_right()` 内部分派：

```cpp
void shift_left(int count) {
    if constexpr (Bits == 64) {
        // 标量
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        avx::shift_left_256(data_, count);
    }
    // ...
}
```

## 位移实现策略

所有 SIMD 位移操作分两阶段：

1. **Word-shift**（count ≥ 64）：标量搬移整个 word
2. **Bit-shift**（count < 64）：SIMD 并行移位 + carry 传播

### AVX2 256-bit 左移示例

```
阶段 1: word-shift (标量)
  data[3] = data[2], data[2] = data[1], data[1] = data[0], data[0] = 0

阶段 2: bit-shift (AVX2)
  shifted = _mm256_slli_epi64(v, count)      # 每个 qword 独立左移
  carry   = _mm256_permute4x64_epi64(v, 0x93) # 旋转: [q3,q2,q1,q0] -> [q0,q3,q2,q1]
  carry   = _mm256_srli_epi64(carry, 64-count) # 提取 carry bits
  carry   = _mm256_blend_epi32(carry, zero, 0x03) # 清除最低 qword 的 carry
  result  = shifted | carry
```

## 为什么不用 `_mm256_slli_si256`？

`_mm256_slli_si256` 是**按 128-bit lane 独立操作**的，不会跨 lane 移位。这是 AVX2 的一个常见陷阱。BitCal v2.1 修复了这个 bug，改用标量 word-shift + SIMD bit-shift 策略。
