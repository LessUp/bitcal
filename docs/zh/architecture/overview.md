# 架构设计概述

本文档描述 BitCal 3.0 保留后的架构与公开接口边界。

> **公开契约说明：** BitCal 3.0 对外暴露 `bitarray`、预定义类型别名、后端枚举，以及文档中保留的成员/运算符 API。已删除的原始指针辅助接口和公开 traits 不再属于受支持的公开表面。

## 目录

- [分层设计](#分层设计)
- [文件结构](#文件结构)
- [设计原则](#设计原则)
- [SIMD 分派机制](#simd-分派机制)
- [性能特性](#性能特性)
- [线程安全](#线程安全)

---

## 分层设计

BitCal 以较小的公开表面封装后端实现细节：

```
┌─────────────────────────────────────────────────────────────┐
│                      公开 API 层                            │
│  bitarray<N>、预定义类型别名、文档化运算符与成员函数         │
├─────────────────────────────────────────────────────────────┤
│                    编译期选择层                             │
│  后端标签、位宽约束、get_default_backend()                  │
├─────────────────────────────────────────────────────────────┤
│                    后端分派层                               │
│  backend_ops.hpp 将操作映射到 scalar/SSE/AVX/...           │
├─────────────────────────────────────────────────────────────┤
│                  ISA 专用实现层                             │
│  scalar_ops.hpp / sse_ops.hpp / avx_ops.hpp / ...          │
└─────────────────────────────────────────────────────────────┘
```

---

## 文件结构

```
include/bitcal/
├── bitcal.hpp              # 公开聚合头文件
├── bitarray.hpp            # 公开 bitarray 模板
├── config.hpp              # 版本宏、后端枚举、平台检测
├── backend_ops.hpp         # 位宽/后端分派胶水层
├── scalar_ops.hpp          # 标量回退实现
├── sse_ops.hpp             # x86 SSE2 实现
├── avx_ops.hpp             # x86 AVX2 实现
├── avx512_ops.hpp          # x86 AVX-512 实现
└── neon_ops.hpp            # ARM NEON 实现
```

**公开头文件：** `bitcal.hpp`、`bitarray.hpp`、`config.hpp`。

---

## 设计原则

### 1. 纯头文件交付

用户通过正常 include 路径消费 BitCal：

```cpp
#include <bitcal/bitcal.hpp>
```

### 2. 收口后的公开契约

受支持的 API 围绕固定宽度值类型展开：

- `bitarray<Bits, Backend>`
- `bit256` 等预定义类型别名
- `docs/en/api/` 中保留的运算符、计数函数和位操作成员

`bitcal::ops` 与公开 traits 等已删除表面，刻意不再属于 3.0 契约。

### 3. 编译期后端选择

默认模板参数在编译期选择后端：

```cpp
template<size_t Bits,
         simd_backend Backend = (Bits == 64 ? simd_backend::scalar : get_default_backend())>
class bitarray;
```

### 4. 单一类型屏蔽后端差异

`backend_ops.hpp` 在保持 `bitarray` 公开 API 稳定的同时，将具体工作路由到不同后端实现。

### 5. 按位宽自适应对齐

对齐按位宽扩展：

- 64 位值使用 8 字节对齐
- 128 位值使用 16 字节对齐
- 256 位值使用 32 字节对齐
- 512 位及以上使用 64 字节对齐

---

## SIMD 分派机制

### 后端选择流程

```
编译器目标 / ISA 标志
         │
         ▼
config.hpp 定义 BITCAL_HAS_* 宏
         │
         ▼
get_default_backend() 选择 avx512 / avx2 / sse2 / neon / scalar
         │
         ▼
bitarray<Bits, Backend> 在编译期绑定后端
         │
         ▼
backend_ops.hpp 将每个操作分派到对应实现
```

### 后端优先级

```cpp
constexpr simd_backend get_default_backend() noexcept {
#if BITCAL_HAS_AVX512
    return simd_backend::avx512;
#elif BITCAL_HAS_AVX2
    return simd_backend::avx2;
#elif BITCAL_HAS_SSE2
    return simd_backend::sse2;
#elif BITCAL_HAS_NEON
    return simd_backend::neon;
#else
    return simd_backend::scalar;
#endif
}
```

---

## 性能特性

| 位宽 | 典型后端 | 说明 |
|------|----------|------|
| 64 位 | Scalar | 单字快速路径 |
| 128 位 | SSE2 / NEON / AVX-512 VL | 原生 128 位向量宽度 |
| 256 位 | AVX2 / AVX-512 VL | 常见高吞吐路径 |
| 512 位 | AVX-512 或循环 AVX2 | 更大固定宽度工作负载 |
| 1024 位 | 循环选定后端 | 吞吐随字数扩展 |

---

## 线程安全

### 无需额外同步即可安全

- 不同线程操作不同 `bitarray` 实例
- 对共享 `bitarray` 的只读访问

### 需要外部同步

- 并发修改同一个 `bitarray`
- 对同一实例进行读写混用
