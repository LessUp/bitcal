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
- 显式的 `view()` 借用入口
- 基于 `span` 的导入/导出辅助接口

```cpp
std::array<std::uint64_t, 4> words{0xFULL, 0x0ULL, 0xF0ULL, 0x0ULL};
auto block = bitcal::bit_block<256>::from_words(std::span<const std::uint64_t>(words));
```

### 非拥有视图

`bit_view` 与 `const_bit_view` 让算法可以直接处理现有缓冲区，而不需要转移所有权。

视图之所以是一等公民，是因为 BitCal 希望算法组合足够便宜且足够显式：

- 算法可以读取或写回现有存储
- 调用方可以预分配输出缓冲区
- 所有权决策在代码中保持可见

```cpp
bitcal::bit_block<256> block;
auto writable = block.view();
bitcal::const_bit_view readable = writable;
```

### 自由算法

算法以自由函数的形式作用在 block 与 view 之上。

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
bitcal::bit_block<256> out;

auto produced = bitcal::bit_and<256>(lhs.view(), rhs.view());
bitcal::and_into(lhs.view(), rhs.view(), out.view());

auto zero = bitcal::is_zero(produced.view());
auto ones = bitcal::popcount(out.view());
```

这样可以让契约围绕“可观察行为”展开，而不是围绕一个不断膨胀的成员型对象展开。

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
- `default_backend()` 由 x86-64 特性检测驱动
- 当前保留的快路径集中在 AVX2 支撑的 `and_into()` / `bit_and<Bits>()`
- 正确性与基准结论都锚定在仓库内保留的验证路径上

这是一条优先级声明，不意味着每个平台都会得到同等深度的优化承诺。

## 白皮书刻意不承诺什么

本白皮书有意保持克制，它**不**承诺：

- 为旧公开模型提供兼容垫片
- 把运行时 CPU 分发纳入当前契约
- 为所有操作系统与 ISA 提供同等深度的优化
- 脱离具体编译设置与硬件上下文单独宣称性能数字

## 验证路径

支撑当前叙事的仓库基线命令如下：

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release -j"$(nproc)"
ctest --test-dir build-test --output-on-failure -C Release
./build-test/benchmarks/bitcal_benchmark
```

## 下一步阅读

- [SIMD 分发](./simd-dispatch.md) —— 了解当前执行路径
- [平台支持](./platform-support.md) —— 了解保留的支持边界
- [类型参考](../api/types.md) 与 [核心操作](../api/core-operations.md) —— 以参考形式查看公开术语表
