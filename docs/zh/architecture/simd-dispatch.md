# SIMD 分发机制

## 编译时分发

BitCal 使用 C++17 `if constexpr` 在编译时选择最优实现路径。此分发机制具有**零运行时开销**。

## 二元操作分发

v2.1 将 `bit_and`/`bit_or`/`bit_xor`/`bit_andnot` 统一为单一模板分发：

```cpp
enum class binop { op_and, op_or, op_xor, op_andnot };

template<binop Op>
static void dispatch_binop(const bitarray& a, const bitarray& b, bitarray& out) {
    if constexpr (Bits == 64) {
        // 标量 64 位操作
        if constexpr (Op == binop::op_and) {
            out.data_[0] = a.data_[0] & b.data_[0];
        }
        // ... 其他操作
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        // AVX2 256 位操作
        if constexpr (Op == binop::op_and) {
            avx::bit_and_256(a.data_, b.data_, out.data_);
        }
        // ... 其他操作
    }
    // ... 其他后端
    else {
        // 标量数组回退
        for (size_t i = 0; i < u64_count; ++i) {
            if constexpr (Op == binop::op_and) {
                out.data_[i] = a.data_[i] & b.data_[i];
            }
            // ... 其他操作
        }
    }
}
```

## 移位操作分发

移位操作在 `shift_left()` / `shift_right()` 内部进行分发：

```cpp
void shift_left(int count) {
    if constexpr (Bits == 64) {
        // 标量
        data_[0] <<= count;
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        avx::shift_left_256(data_, count);
    }
    // ...
}
```

## 移位实现策略

所有 SIMD 移位操作分两阶段执行：

1. **字移位**（count ≥ 64）：标量整字移动
2. **位移位**（count < 64）：SIMD 并行移位 + 进位传播

### AVX2 256 位左移示例

```
阶段 1：字移位（标量）
  data[3] = data[2], data[2] = data[1], data[1] = data[0], data[0] = 0

阶段 2：位移位（AVX2）
  shifted = _mm256_slli_epi64(v, count)           # 每个 qword 独立移位
  carry   = _mm256_permute4x64_epi64(v, 0x93)    # 旋转：[q3,q2,q1,q0] -> [q0,q3,q2,q1]
  carry   = _mm256_srli_epi64(carry, 64-count)   # 提取进位位
  carry   = _mm256_blend_epi32(carry, zero, 0x03) # 清除最低 qword 的进位
  result  = shifted | carry
```

## 为何不使用 `_mm256_slli_si256`？

`_mm256_slli_si256` **独立操作 128 位通道**，不会跨通道移位。这是常见的 AVX2 陷阱。

```cpp
// 错误：无法按预期工作
__m256i shifted = _mm256_slli_si256(v, count);  // 通道边界问题

// 正确：BitCal 的方法
// 阶段 1：跨通道移动整字（标量）
// 阶段 2：通道内移位 + 进位传播（SIMD）
```

BitCal v2.1 通过两阶段方法修复了此问题。

## 后端特定实现文件

### scalar_ops.hpp

使用标准 C++ 的可移植回退：

```cpp
namespace bitcal::scalar {
    inline void bit_and_256(uint64_t* out, const uint64_t* a, const uint64_t* b) {
        for (size_t i = 0; i < 4; ++i) {
            out[i] = a[i] & b[i];
        }
    }
    // ... 其他操作
}
```

### avx_ops.hpp

AVX2 实现：

```cpp
namespace bitcal::avx {
    inline void bit_and_256(uint64_t* out, const uint64_t* a, const uint64_t* b) {
        __m256i va = _mm256_load_si256((__m256i*)a);
        __m256i vb = _mm256_load_si256((__m256i*)b);
        __m256i vr = _mm256_and_si256(va, vb);
        _mm256_store_si256((__m256i*)out, vr);
    }
    // ... 其他操作
}
```

### neon_ops.hpp

ARM NEON 实现：

```cpp
namespace bitcal::neon {
    inline void bit_and_256(uint64_t* out, const uint64_t* a, const uint64_t* b) {
        uint64x2_t va0 = vld1q_u64(a);
        uint64x2_t va1 = vld1q_u64(a + 2);
        uint64x2_t vb0 = vld1q_u64(b);
        uint64x2_t vb1 = vld1q_u64(b + 2);
        vst1q_u64(out, vandq_u64(va0, vb0));
        vst1q_u64(out + 2, vandq_u64(va1, vb1));
    }
    // ... 其他操作
}
```

## 寄存器压力

分发机制设计为最小化寄存器压力：
- 小位宽（64 位）：使用标量寄存器
- 中等位宽（128/256 位）：使用单一 SIMD 寄存器
- 大位宽（512/1024 位）：拆分为多个 SIMD 操作或使用标量循环

## 编译流程

1. **模板实例化**：`bitarray<256, avx2>` 以已知 `Bits` 和 `Backend` 实例化
2. **分发消除**：`if constexpr` 选择活动分支，其他分支被丢弃
3. **内联**：`BITCAL_FORCEINLINE` 沿调用链传播
4. **代码生成**：编译器为特定位宽/后端生成最优机器码

## 调试分发

检查正在使用的后端：

```cpp
#include <iostream>

int main() {
    // 检查默认后端
    std::cout << "默认后端: "
              << static_cast<int>(bitcal::get_default_backend()) << std::endl;

    // 检查 bitarray 的后端
    bitcal::bit256 arr;
    std::cout << "数组后端: "
              << static_cast<int>(decltype(arr)::backend) << std::endl;

    return 0;
}
```

## 未来改进

- AVX-512 支持 512 位操作
- SVE/SVE2 支持 ARM
- 动态分发用于运行时 CPU 特性检测
