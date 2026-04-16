# BitCal 文档

BitCal 是一个高性能、跨平台的 C++17 位运算加速库。

## 快速开始

```cpp
#include <bitcal/bitcal.hpp>

int main() {
    bitcal::bit256 a(0xDEADBEEF);
    bitcal::bit256 b(0xCAFEBABE);

    auto c = a & b;           // AND
    a <<= 10;                 // 左移
    auto ones = a.popcount(); // 统计 1 的个数

    return 0;
}
```

## 目录结构

```
bitcal/
├── include/bitcal/       # 核心头文件
│   ├── bitcal.hpp        # 主入口
│   ├── config.hpp        # 平台配置
│   ├── simd_traits.hpp   # SIMD 特征
│   ├── scalar_ops.hpp    # 标量实现
│   ├── sse_ops.hpp       # SSE2 实现
│   ├── avx_ops.hpp       # AVX2 实现
│   └── neon_ops.hpp      # NEON 实现
├── tests/                # 单元测试
├── benchmarks/           # 性能测试
├── examples/             # 示例代码
└── gitbook/              # 完整文档
```

## 核心类型

| 类型 | 位宽 | 说明 |
|------|------|------|
| `bitcal::bit64` | 64 | 单个机器字 |
| `bitcal::bit128` | 128 | SSE2/NEON 原生宽度 |
| `bitcal::bit256` | 256 | AVX2 原生宽度 |
| `bitcal::bit512` | 512 | 大位宽 |
| `bitcal::bit1024` | 1024 | 超大位宽 |

## SIMD 支持

| 平台 | 指令集 | 加速比 |
|------|--------|--------|
| x86-64 | AVX2 | 5-6× |
| ARM | NEON | 2.5× |

## 更多文档

- [GitBook 文档](../gitbook/) — 完整 API 参考
- [README.md](../README.md) — 项目概述
- [CHANGELOG.md](../CHANGELOG.md) — 变更日志
