# 术语表

BitCal 文档中使用的术语定义。

## 核心类型

| 术语 | 英文 | 定义 |
|------|------|------|
| 位块 | bit_block | 拥有型存储容器，承载固定位宽的数据 |
| 位视图 | bit_view | 可变借用型访问器，不拥有存储 |
| 常量位视图 | const_bit_view | 只读借用型访问器，不拥有存储 |
| 字 | word | 64 位存储单位，BitCal 的基础数据单元 |

## 设计概念

| 术语 | 英文 | 定义 |
|------|------|------|
| 公开模型 | public model | BitCal 对外暴露的类型和操作契约 |
| 分发边界 | dispatch boundary | 编译时选择后端的实现边界 |
| 后端 | backend | 特定 ISA 的实现（scalar/SSE2/AVX2/NEON） |
| 内核 | kernel | 后端中的具体算法实现 |
| 契约 | contract | 用户可依赖的行为保证 |

## 存储术语

| 术语 | 英文 | 定义 |
|------|------|------|
| 对齐 | alignment | 内存地址是某个值的倍数（BitCal 默认 32 字节对齐） |
| 小端序 | little-endian | 低位字节存储在低地址 |
| 固定位宽 | fixed-width | 编译时确定的位数量 |

## SIMD 术语

| 术语 | 英文 | 定义 |
|------|------|------|
| SIMD | Single Instruction, Multiple Data | 单指令多数据，并行处理技术 |
| SSE2 | Streaming SIMD Extensions 2 | 128 位 SIMD 指令集 |
| AVX2 | Advanced Vector Extensions 2 | 256 位整数 SIMD 指令集 |
| AVX-512 | Advanced Vector Extensions 512 | 512 位 SIMD 指令集 |
| NEON | ARM NEON | ARM 128 位 SIMD 指令集 |
| Intrinsic | 内联函数 | 编译器提供的 SIMD 函数接口 |

## 操作术语

| 术语 | 英文 | 定义 |
|------|------|------|
| 按位与 | bit_and | 逐位 AND 运算 |
| 按位或 | bit_or | 逐位 OR 运算 |
| 按位异或 | bit_xor | 逐位 XOR 运算 |
| 位计数 | popcount | 统计 1 的个数 |
| 前导零计数 | clz | Count Leading Zeros |
| 尾随零计数 | ctz | Count Trailing Zeros |
| 位反转 | bit_reverse | 位顺序反转 |

## 文档术语

| 术语 | 英文 | 定义 |
|------|------|------|
| 白皮书 | whitepaper | 技术设计文档 |
| 基线 | baseline | 当前可复现的性能基准 |
| 方法学 | methodology | 测量和解释的规则体系 |
| 证据 | evidence | 可验证的性能或正确性数据 |

## C++ 术语

| 术语 | 英文 | 定义 |
|------|------|------|
| Header-only | 头文件式 | 无需编译库文件，只需包含头文件 |
| if constexpr | 编译时条件 | C++17 特性，编译时分支选择 |
| 零开销抽象 | zero-overhead abstraction | 不引入运行时成本的抽象 |
| SFINAE | Substitution Failure Is Not An Error | 模板元编程技术 |

---

> 返回 [学院首页](./index.md) 或继续阅读 [白皮书](../whitepaper/index.md)。
