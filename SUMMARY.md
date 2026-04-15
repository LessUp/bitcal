# BitCal 2.1 - 项目总结

## 项目概况

| 项目 | 信息 |
|------|------|
| **名称** | BitCal - 高性能位运算加速库 |
| **版本** | 2.1.0 |
| **语言** | C++17 |
| **类型** | Header-only |
| **许可** | MIT |

## 核心特性

- ✅ **零开销抽象** — 编译期 SIMD 分派
- ✅ **跨平台** — x86 (SSE2/AVX/AVX2) + ARM (NEON)
- ✅ **丰富 API** — 位运算、位移、popcount、CLZ/CTZ 等
- ✅ **Header-only** — 无需编译，直接 include

## 架构设计

```
┌─────────────────────────────────────┐
│    用户 API (bitarray<N>)          │
├─────────────────────────────────────┤
│    编译期 SIMD 选择 (traits)       │
├──────────┬──────────┬───────────────┤
│  scalar  │  x86     │    ARM        │
│  (通用)  │SSE2/AVX2 │   NEON        │
└──────────┴──────────┴───────────────┘
```

## 文件结构

```
bitcal/
├── include/bitcal/     # 核心头文件 (7 个)
│   ├── config.hpp      # 平台检测
│   ├── simd_traits.hpp # SIMD 特征
│   ├── scalar_ops.hpp  # 标量实现
│   ├── sse_ops.hpp     # SSE2 实现
│   ├── avx_ops.hpp     # AVX2 实现
│   ├── neon_ops.hpp    # NEON 实现
│   └── bitcal.hpp      # 主入口
├── tests/              # 单元测试 (36 个)
├── benchmarks/         # 性能测试
├── examples/           # 示例代码
├── gitbook/            # 文档
└── changelog/          # 变更日志
```

## 性能

| 平台 | 加速比 |
|------|--------|
| x86 AVX2 | **5-6×** |
| ARM NEON | **2.5×** |

## 测试覆盖

| 位宽 | 测试项 |
|------|--------|
| 64-bit | 9 个测试 |
| 128-bit | 5 个测试 |
| 256-bit | 7 个测试 |
| 512-bit | 3 个测试 |
| 1024-bit | 7 个测试 |
| 其他 | 5 个测试 |
| **总计** | **36 个测试** |

## 平台支持

| 平台 | 架构 | 状态 |
|------|------|------|
| Linux | x86-64 | ✅ CI 验证 |
| Linux | ARM64 | ✅ CI 验证 |
| Linux | ARM32 | ✅ CI 验证 |
| Windows | x86-64 | ✅ CI 验证 |
| macOS | x86-64 | ✅ CI 验证 |
| macOS | ARM64 | ✅ CI 验证 |

## 快速开始

```cpp
#include <bitcal/bitcal.hpp>

bitcal::bit256 a(0xDEADBEEF);
bitcal::bit256 b(0xCAFEBABE);

auto c = a & b;           // AND
a <<= 10;                 // 左移
auto ones = a.popcount(); // 统计 1 的个数
```

## 文档

- [README.md](README.md) — 项目概述
- [README.zh-CN.md](README.zh-CN.md) — 中文概述
- [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) — 迁移指南
- [gitbook/](gitbook/) — 完整文档
