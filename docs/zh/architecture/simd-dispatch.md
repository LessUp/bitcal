# SIMD 分发

BitCal vNext 把 SIMD 分发视为公开 block / view / algorithm 模型之后的内部执行问题。

## 先看公开故事

调用方不会通过选择不同的公开存储类型来选后端。标准使用方式是：

1. 用 `bit_block<Bits>` 持有数据
2. 用 `bit_view` / `const_bit_view` 借用数据
3. 调用自由算法

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;
bitcal::bit_block<256> out;

bitcal::and_into(lhs.view(), rhs.view(), out.view());
```

从这一步往下，就是后端边界。

## 当前分发流程

```text
使用者的编译标志 / 目标 ISA
        │
        ▼
config.hpp 计算 BITCAL_ARCH_X86 / BITCAL_HAS_SSE2 / BITCAL_HAS_AVX2 / BITCAL_HAS_AVX512
        │
        ▼
自由算法入口
        │
        ▼
detail::word_ops.hpp
        │
        ▼
detail::x64_dispatch.hpp
        │
        ├── x86-64 且可用时走 AVX2 字循环
        └── 其他情况走标量回退
```

## 当前哪些能力走这层分发

### `and_into()` 与 `bit_and<Bits>()`

当前写路径是后端边界最直观的例子：

- `and_into()` 用调试断言检查字数一致
- 然后转发到 `detail::and_words()`
- `detail::and_words()` 再转发到 `and_into_x64()`
- `and_into_x64()` 会按当前编译目标/配置宏编译；在 x86-64 且启用 AVX2 时使用向量循环，否则回退到标量逐字操作

### `is_zero()` 与 `popcount()`

这些查询当前仍然直接遍历字数组。它们共享同一套公开模型，但并没有试图提前宣称“已经拥有完整向量化故事”。

## 为什么要保留这条边界

后端边界让 BitCal 可以演进执行细节，而不必改写应用层面对的公开类型。

它带来的好处有三点：

- 公开术语表保持精简且可讲清
- x86-64 内核可以独立于所有权语义持续优化
- 标量执行仍然是泛化构建与非主要平台的可移植下限

## 诊断接口，不是耦合入口

`backend_kind` 与 `default_backend()` 是有用的诊断工具：

```cpp
auto backend = bitcal::default_backend();
```

它们适合用来解释一次构建或记录一次 benchmark 运行。它们只是对同一套编译期目标状态的摘要，并不会在热路径里运行时选择实现；除非你愿意自己承担维护成本，否则不要把它们当成把公开 API 分叉成“后端特定业务代码”的理由。

## 会影响分发的构建开关

| 开关 | 作用 |
| --- | --- |
| `-march=native` | 让 GCC/Clang 为当前机器启用最合适的指令集 |
| `-mavx2` | 强制生成可用 AVX2 的 x86 构建 |
| `/arch:AVX2` | 在 MSVC 下启用面向 AVX2 的开发目标 |
| `BITCAL_NATIVE_ARCH=ON` | 为仓库内测试/示例/benchmark 添加本机 CPU 标志 |
| `BITCAL_NATIVE_ARCH=OFF` | 让开发目标走更保守的可移植路径 |

## 本页刻意不宣称的内容

- 不把运行时分发写成当前契约
- 不声称今天每个查询都有专门的向量内核
- 不声称今天每个非 x86 平台都已有专门 SIMD 后端

支持边界请看[平台支持](./platform-support.md)，架构动机请看 [vNext 白皮书](./vnext-whitepaper.md)。
