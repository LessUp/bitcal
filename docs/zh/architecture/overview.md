# 架构设计概述

本文档描述 BitCal 的设计原则和实现细节。

## 目录

- [分层设计](#分层设计)
- [文件结构](#文件结构)
- [设计原则](#设计原则)
- [SIMD 分派机制](#simd-分派机制)
- [性能特性](#性能特性)
- [线程安全](#线程安全)
- [内存使用](#内存使用)

---

## 分层设计

BitCal 采用分层架构，将用户接口与实现细节分离：

```
┌─────────────────────────────────────────────────────────────┐
│                      用户 API 层                            │
│  bitarray<N>、运算符、类型别名 (bit64、bit256 等)           │
├─────────────────────────────────────────────────────────────┤
│                    编译期分派层                             │
│  if constexpr、类型特征、后端选择                            │
├─────────────────────────────────────────────────────────────┤
│                    SIMD 实现层                              │
│  ├── scalar_ops.hpp  (可移植回退实现)                       │
│  ├── sse_ops.hpp     (x86 SSE2 128位)                       │
│  ├── avx_ops.hpp     (x86 AVX2 256位)                       │
│  └── neon_ops.hpp    (ARM NEON 128位)                       │
├─────────────────────────────────────────────────────────────┤
│                    硬件指令层                               │
│  SSE2 / AVX / AVX2 / AVX-512 / NEON / 标量                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 文件结构

```
include/bitcal/
├── config.hpp              # 平台检测宏、simd_backend 枚举
├── simd_traits.hpp         # SIMD 寄存器宽度、对齐要求
├── scalar_ops.hpp          # 可移植标量实现（基线）
├── sse_ops.hpp             # x86 SSE2 128 位运算
├── avx_ops.hpp             # x86 AVX2 256 位运算
├── neon_ops.hpp            # ARM NEON 128 位运算
└── bitcal.hpp              # 主头文件：聚合所有头文件 + bitarray 类
```

**关键文件：**

| 文件 | 用途 | 代码行数（约） |
|------|------|--------------|
| `config.hpp` | 编译器/平台检测 | ~150 |
| `simd_traits.hpp` | SIMD 后端的类型特征 | ~200 |
| `scalar_ops.hpp` | 回退标量实现 | ~300 |
| `sse_ops.hpp` | SSE2 优化运算 | ~400 |
| `avx_ops.hpp` | AVX2 优化运算 | ~450 |
| `neon_ops.hpp` | NEON 优化运算 | ~400 |
| `bitcal.hpp` | 主 API（bitarray 类） | ~600 |

---

## 设计原则

### 1. 纯头文件库

无需编译。用户只需：

```cpp
#include <bitcal/bitcal.hpp>
```

**优势：**
- 零链接时依赖
- 易于集成（复制头文件）
- 跨编译单元的最佳内联

### 2. 编译期分派

所有后端选择在编译期完成：

```cpp
template<binop Op, size_t Bits, simd_backend Backend>
static void dispatch_binop(const uint64_t* a, const uint64_t* b, uint64_t* out) {
    if constexpr (Bits == 64) {
        // 标量 64 位运算（最高效）
        out[0] = apply_op<Op>(a[0], b[0]);
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        // AVX2 256 位运算
        avx2::binop<Op>(a, b, out);
    }
    else if constexpr (Backend == simd_backend::neon && Bits >= 128) {
        // NEON 128 位运算（大位宽使用循环）
        neon::binop_loop<Op, Bits>(a, b, out);
    }
    else {
        // 其他情况的标量回退
        scalar::binop_loop<Op, Bits>(a, b, out);
    }
}
```

### 3. 零开销抽象

关键路径使用强制内联：

```cpp
#define BITCAL_FORCEINLINE inline __attribute__((always_inline))

class bitarray {
    BITCAL_FORCEINLINE bitarray operator&(const bitarray& other) const {
        bitarray result;
        dispatch_binop<binop::AND>(*this, other, result);
        return result;
    }
};
```

优化后，`a & b` 编译为：
```asm
; AVX2，bit256
vmovdqu ymm0, [rdi]
vmovdqu ymm1, [rdi+32]
vpand   ymm0, ymm0, [rsi]
vpand   ymm1, ymm1, [rsi+32]
vmovdqu [rdx], ymm0
vmovdqu [rdx+32], ymm1
ret
```

### 4. 优雅降级

不支持的组合自动回退到标量：

```cpp
// 在没有 AVX2 的系统上，这仍然可以工作：
bitcal::bitarray<256, bitcal::simd_backend::avx2> arr;
// 回退到标量，无运行时错误
```

### 5. 内存对齐

所有内部存储都是 64 字节对齐：

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
    alignas(64) uint64_t data_[Bits / 64];
};
```

**为什么是 64 字节？**
- 匹配 x86 缓存行大小
- AVX-512 的要求（如果支持）
- DMA 和 GPU 互操作的最佳选择

---

## SIMD 分派机制

### 后端选择流程

```
使用 -march=native 编译
         │
         ▼
┌─────────────────────────────────┐
│  编译器定义 __AVX2__ 等宏       │
└─────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│ config.hpp 设置 BITCAL_HAS_*    │
└─────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│ select_best_backend<Bits>()     │
│ 为位宽选择最优后端              │
└─────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│ bitarray 使用选定的后端         │
│ 执行所有运算                    │
└─────────────────────────────────┘
```

### 后端优先级

```cpp
constexpr simd_backend get_default_backend() {
    #if defined(BITCAL_HAS_AVX2)
        return simd_backend::avx2;      // x86 上性能最高
    #elif defined(BITCAL_HAS_SSE2)
        return simd_backend::sse2;      // 通用 x86 支持
    #elif defined(BITCAL_HAS_NEON)
        return simd_backend::neon;      // ARM 支持
    #else
        return simd_backend::scalar;    // 通用回退
    #endif
}
```

---

## 性能特性

### 运算加速比

| 运算 | 标量 | SSE2 | AVX2 | NEON | 说明 |
|------|------|------|------|------|------|
| 与/或/异或 | 1.0× | ~2.0× | ~5.9× | ~2.6× | 大位宽时受内存限制 |
| 非运算 | 1.0× | ~1.9× | ~1.7× | ~2.0× | 与全 1 异或 |
| 位移 | 1.0× | ~2.3× | ~3.6× | ~1.8× | 跨通道进位处理 |
| is_zero | 1.0× | ~2.1× | ~2.5× | ~2.2× | Movemask/testz 优化 |
| 人口计数 | 1.0× | 1.0× | 2.0× | 1.0× | 使用 popcnt 指令 |

### 扩展行为

| 位宽 | 最优后端 | 相对性能 |
|------|----------|---------|
| 64 位 | 标量 | 基线 |
| 128 位 | SSE2/NEON | ~2× |
| 256 位 | AVX2 | ~6× |
| 512 位 | AVX2（2 次操作） | ~6× |
| 1024 位 | AVX2（4 次操作） | ~6× |

---

## 线程安全

### 线程安全操作

- 不同线程操作**不同**的 `bitarray` 实例
- 对共享实例的**只读**操作

### 非线程安全

- 无同步情况下对**同一**实例的并发读/写

**示例：**
```cpp
bitcal::bit256 shared;

// 线程 1（无互斥锁时不安全）
shared |= other;

// 线程 2（无互斥锁时不安全）
shared.set_bit(42, true);
```

**解决方案：**
```cpp
std::mutex mtx;
bitcal::bit256 shared;

// 线程 1
{
    std::lock_guard<std::mutex> lock(mtx);
    shared |= other;
}

// 线程 2
{
    std::lock_guard<std::mutex> lock(mtx);
    shared.set_bit(42, true);
}
```

---

## 内存使用

### 存储需求

| 类型 | 用户数据 | 对齐填充 | 总计 |
|------|---------|---------|------|
| bit64 | 8 字节 | 56 字节 | 64 字节 |
| bit128 | 16 字节 | 48 字节 | 64 字节 |
| bit256 | 32 字节 | 32 字节 | 64 字节 |
| bit512 | 64 字节 | 0 字节 | 64 字节 |
| bit1024 | 128 字节 | 0 字节 | 128 字节 |

### 缓存效率

```cpp
// 适合单个缓存行（最优）
bitcal::bit512 a, b, c;
c = a & b;  // 3 × 64字节 = 192 字节（3 个缓存行）

// 稍大
bitcal::bit1024 x, y, z;
z = x & y;  // 3 × 128字节 = 384 字节（6 个缓存行）
```

### 栈使用警告

极大的位宽可能导致栈溢出：

```cpp
// 深度递归函数有栈溢出风险
void recursive_function() {
    bitcal::bitarray<65536> huge;  // 栈上 8KB
    // ...
    recursive_function();  // 栈增长
}

// 解决方案：大位宽使用堆分配
auto huge = std::make_unique<bitcal::bitarray<65536>>();
```

---

## 另请参阅

- [SIMD 分派](simd-dispatch.md) — 详细分派机制
- [平台支持](platform-support.md) — 支持的平台和编译器
