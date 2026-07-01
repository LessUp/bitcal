# 状态

Status 的任务，是把 BitCal 所有叙事中的修辞成分都剥掉，只留下现在时边界。

## 发布姿态

- BitCal 当前处于主动推进的 **vNext / 4.0.0 级重设计**，而不是长期稳定维护阶段。
- 公开语言基线是 **C++23**。
- 主要优化与测量姿态是 **x86-64 first**。
- 当前文档中心模型是 `bit_block<Bits>`、`bit_view`、`const_bit_view` 与自由算法。
- 旧的单体式 API 形态已不再作为当前 retained vNext 公共表面的一部分随仓库交付。
- 不承诺为早期那种单体式 API 形态提供代码级兼容层。

## 支持矩阵

| 范围 | 当前姿态 | 读者可以据此推断什么 |
| --- | --- | --- |
| Linux x86-64（GCC / Clang） | 主支持范围 | 主要构建、correctness 与 benchmark 注意力集中在这里。 |
| Windows x86-64（MSVC） | 主支持范围 | 与 Linux x86-64 一起构成当前主验证面。 |
| ARM64 与 macOS 等后续目标 | 次级范围 | 可以继续保留可构建性，但性能数据先留空，直到项目保留了对应 benchmark 证据路径；成熟度主张也必须更谨慎。 |
| 国产 CPU SIMD 扩展 | 研究范围 | LoongArch LSX/LASX、兆芯、鲲鹏、飞腾等特化加速需要单独 OpenSpec、测试和 benchmark 证据，才能升级成支持声明。 |
| CUDA、GPU 与 NPU 加速 | core library 不支持 | 这些不属于当前保留的 header-only 核心库。CUDA/GPU 需要 opt-in extension 设计；NPU 不适合当前 bitset 风格公开算法。 |
| 超出保留验证面的其他内容 | 不在宣称范围 | 没有新增证据之前，不应升级成支持承诺。 |

## 证据层级

BitCal 对不同证据使用不同表述：

| 层级 | 含义 |
| --- | --- |
| 可构建 | 保留 CI 或文档命令可以为目标平台完成编译。 |
| 正确性 | 保留测试能在该目标或等价 runner 上执行。 |
| SIMD 加速 | 保留公开算法路径使用目标相关 SIMD 实现。 |
| 保留 benchmark 证据 | 目标、backend 与 commit 都有结构化 benchmark artifact。 |

当前 x86 SIMD 后端由 AVX2、AVX-512 等编译目标特性宏选择，不是单一通用二进制的运行时 CPU feature dispatch。

## 文档真相

当前主要真相源仍然是：

- `openspec/` 中的活动需求与变更；
- `docs/README.md` 中的文档架构政策；
- 稳定公开 include seam `<bitcal/bitcal.hpp>`；
- 仓库中定义版本语义的单一事实来源。

## 变更纪律

BitCal 期望以下几类资产同步移动：

- OpenSpec 与 Status 文案；
- Reference 表述与公开示例；
- benchmark 方法学与 benchmark claim；
- 实现进展与支持矩阵。

## 接下来如何读

- 需要回看完整评审路径时，回到 [导读](/zh/guide/)；
- 需要重新理解架构论证时，回到 [白皮书](/zh/whitepaper/)；
- 想把局部证据变成更稳妥结论前，请结合 [性能](/zh/performance/) 与 [研究](/zh/research/) 一起看。
