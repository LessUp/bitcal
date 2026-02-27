# 整体架构

## 分层设计

```
┌─────────────────────────────────────┐
│    用户 API (bitarray<N>)          │  高层接口
├─────────────────────────────────────┤
│    编译期 SIMD 选择 (traits)       │  中间层
├─────────────────────────────────────┤
│  SIMD 实现层                        │  底层实现
│  ├── scalar_ops (通用)              │
│  ├── sse_ops (x86)                  │
│  ├── avx_ops (x86)                  │
│  └── neon_ops (ARM)                 │
├─────────────────────────────────────┤
│  硬件指令集                         │  硬件层
│  SSE2/AVX/AVX2/NEON/标量            │
└─────────────────────────────────────┘
```

## 文件结构

```
include/bitcal/
├── config.hpp        # 平台检测、宏定义、simd_backend 枚举
├── simd_traits.hpp   # SIMD 类型特征（寄存器宽度、对齐要求）
├── scalar_ops.hpp    # 标量后备实现（所有平台可用）
├── sse_ops.hpp       # x86 SSE2 实现
├── avx_ops.hpp       # x86 AVX2 实现
├── neon_ops.hpp      # ARM NEON 实现
└── bitcal.hpp        # 主入口（聚合所有头文件 + bitarray 类）
```

## 设计原则

1. **Header-only** — 零编译依赖，用户只需 `#include <bitcal/bitcal.hpp>`
2. **编译期分派** — 通过 `if constexpr` + 模板特化在编译期选择最优实现
3. **零开销抽象** — 所有分派在编译期完成，关键路径 `BITCAL_FORCEINLINE`
4. **优雅降级** — 不支持的位宽/后端组合自动回退到标量实现
5. **内存对齐** — 内部数据 `alignas(64)` 对齐，提供 aligned load/store

## 核心类

`bitarray<Bits, Backend>` 是库的核心类型：

- `Bits` 编译期确定位宽
- `Backend` 编译期确定 SIMD 后端
- 内部存储为 `alignas(64) uint64_t data_[u64_count]`
- 所有操作通过 `dispatch_binop<Op>` 统一模板分派
