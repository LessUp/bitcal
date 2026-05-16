# BitCal vNext 白皮书

## 核心论点

BitCal vNext 可以用一句话概括：

> **用 block 持有存储，用 view 借用存储，用自由算法表达工作，并把后端细节留在狭窄的边界之后。**

这句话决定了公开 API 的形状，也决定了验证方式与性能叙事。

## 公开模型

### 拥有型 block

`bit_block<Bits>` 是拥有型基本单元。它提供：

- 由 `Bits` 固定的编译期宽度
- 连续的 `std::uint64_t` 字存储
- 通过 `bits` 与 `word_count` 暴露的公开元数据
- 显式的 `view()` 借用入口

### 非拥有视图

`bit_view` 与 `const_bit_view` 让算法可以直接处理现有缓冲区，而不需要转移所有权。

视图之所以是一等公民，是因为 BitCal 希望算法组合足够便宜且足够显式：

- 算法可以通过 `const_bit_view` 读取现有存储
- 可变访问通过 `bit_view` 明确保留在调用点
- 所有权决策在代码中保持可见

```cpp
bitcal::bit_block<256> block;
auto writable = block.view();
writable.data()[0] = 0xFULL;

const auto& block_const = block;
auto readable = block_const.view();
auto first = readable.word(0);
```

### 自由算法

redesign 的方向确实是让自由函数成为作用于 view 与 block 的中心，但今天已发货的命名空间级算法集合仍然刻意保持得很窄。

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
const auto& lhs_const = lhs;
const auto& rhs_const = rhs;

auto produced = bitcal::bit_and<256>(lhs_const.view(), rhs_const.view());
bitcal::bit_block<256> scratch;
bitcal::and_into(lhs_const.view(), rhs_const.view(), scratch.view());

const auto& produced_const = produced;
const auto& scratch_const = scratch;

auto empty = bitcal::is_zero(produced_const.view());
auto ones = bitcal::popcount(scratch_const.view());
```

当前已发货的自由函数表面包括：

- `bit_and<Bits>()`
- `and_into()`
- `is_zero()`
- `popcount()`
- 作为公开后端词汇表的 `backend_kind`

`bit_or`、`bit_xor`、`bit_andnot`、`equals`、`shift_left`、`shift_right` 这些更宽的 redesign 名称今天并没有作为命名空间级自由算法导出，因此文档不会把它们写成当前可直接使用的公开表面。

这样做可以让契约围绕“可观察行为”展开，而不是围绕一个不断膨胀的成员型对象展开。

## 为什么要这样拆分

### 1. 所有权应该显式

当所有权是真正的类型时，调用方可以明确看见何时分配、复制或保留存储，这让 API 更容易与现代标准库设施组合，也更容易读懂。

### 2. 借用应该足够便宜

当数据本来就存在于别处时，非拥有视图才是正确桥梁。BitCal 不应为了调用一个算法就强迫用户构造临时拥有型对象。

### 3. 算法应该可复用

自由算法天然可以同时服务拥有型与借用型数据，也给实现层保留了演进内核与分发策略的自由度，而不必重写公开存储故事。

## 后端边界

BitCal 只保留一个稳定 include 入口：

```cpp
#include <bitcal/bitcal.hpp>
```

在这个入口之下，当前实现把公开模型与执行细节分开：

```text
bitcal/bitcal.hpp
├── bit_block.hpp
├── bit_view.hpp
├── algorithms.hpp
└── detail/
    ├── backend.hpp
    └── x64_dispatch.hpp
```

这条边界的意义在于：BitCal 可以继续调整内部分发与内核结构，而不必把这些变化变成调用方必须跟着改写的类型系统负担。

## x86-64 优先的验证姿态

vNext 明确是 x86-64-first。

当前含义是：

- 标量路径永远存在，作为可移植性下限
- 编译期目标/配置宏决定保留的 x86-64 内核能否使用 AVX2，否则就停留在标量路径
- 当前实现重心仍主要集中在 AND 写路径及其相关内核上
- `backend_kind` 负责提供公开文档可见的后端词汇，而具体的后端选择机制仍是实现细节，除非后续 spec 再明确纳入
- 正确性与基准结论都锚定在仓库内保留的验证路径上

这是一条优先级声明，不意味着每个平台都会得到同等深度的优化承诺。

## 白皮书刻意不承诺什么

本白皮书有意保持克制，它**不**承诺：

- 为旧公开模型提供兼容垫片
- 把持久 public spec 之外的 helper API 说成保留兼容承诺
- 把运行时 CPU 分发纳入当前契约
- 为所有操作系统与 ISA 提供同等深度的优化
- 脱离具体编译设置与硬件上下文单独宣称性能数字

## 验证路径

支撑当前叙事的仓库基线命令如下：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --parallel
ctest --test-dir build-test --output-on-failure -C Release
```

然后从 `build-test` 构建树中运行生成的 `bitcal_benchmark` 可执行文件；如果使用 multi-config generator，请使用 `Release` 配置对应的输出位置。

## 下一步阅读

- [SIMD 分发](./simd-dispatch.md) —— 了解当前执行路径
- [平台支持](./platform-support.md) —— 了解保留的支持边界
- [类型参考](../api/types.md) 与 [核心操作](../api/core-operations.md) —— 以参考形式查看公开术语表
