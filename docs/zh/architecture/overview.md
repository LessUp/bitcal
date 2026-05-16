# 架构概览

本节是 BitCal vNext 的白皮书入口页。核心论点很简单：把所有权说清楚、把视图做成一等公民、把工作表达为自由算法，并把这些能力收敛在一个窄而稳定的 header-only 边界上。

## 推荐阅读顺序

1. [vNext 白皮书](./vnext-whitepaper.md) —— 先看架构论点与公开模型
2. [SIMD 分发](./simd-dispatch.md) —— 再看算法如何跨过后端边界
3. [平台支持](./platform-support.md) —— 了解什么是主要目标、次要目标与刻意不承诺的范围
4. [性能基线](./performance-baseline.md) —— 最后看如何解读当前保留的基准证据

## 执行摘要

- **拥有型 block：** `bit_block<Bits>` 持有固定宽度、连续的 `std::uint64_t` 存储，并公开 `view()` 访问器以及 `bits` / `word_count` 元数据。
- **非拥有视图：** `bit_view` 与 `const_bit_view` 让 BitCal 在不改变所有权的前提下操作连续字存储。
- **当前已发货的自由算法：** `bit_and<Bits>()`、`and_into()`、`is_zero()`、`popcount()` 是今天真实导出的命名空间级算法。
- **更宽 redesign 说明：** `bit_or`、`bit_xor`、`bit_andnot`、`equals`、`shift_left`、`shift_right` 这些名字仍属于 redesign 讨论，而不是当前已发货的自由函数文档表面。
- **后端边界：** 调用方只需要 `<bitcal/bitcal.hpp>`、公开类型以及 `backend_kind`；后端选择和 ISA 专用内核仍留在内部实现层。

## vNext 的变化

BitCal 现在从首页到深层正文都围绕同一个心智模型：

- 语言基线是 **C++23**
- 交付方式仍然是 **header-only**
- 公开模型是 block / view / algorithm，而不是单体值类型
- x86-64 是主要优化与验证目标
- 性能结论被视为可复现基线，而不是长期不变的宣传文案

## 当前保留的公开表面

```text
<bitcal/bitcal.hpp>
├── bit_block<Bits>                              拥有型存储
├── bit_view / const_bit_view                    借用字访问
├── backend_kind                                 已文档化的后端词汇表
├── bit_and<Bits>()                              返回 AND 结果
├── and_into()                                   复用可写存储写入 AND
├── is_zero() / popcount()                       查询算法
└── 仅存在于 redesign 讨论中的自由函数名           不按已发货表面文档化
```

redesign 方向仍然瞄准更宽的 block / view / algorithm 故事，但当前文档只描述公开 umbrella header 今天实际导出的自由函数名。

## 验证姿态

当前契约以仓库内保留的验证路径为锚点：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

先用白皮书理解模型，再进入指南与参考页落地使用方式。
