# 状态

Status 的任务，是把 BitCal 所有叙事中的修辞成分都剥掉，只留下现在时边界。

## 发布姿态

- BitCal 当前处于主动推进的 **vNext / 4.0.0 级重设计**，而不是长期稳定维护阶段。
- 公开语言基线是 **C++23**。
- 主要优化与测量姿态是 **x86-64 first**。
- 当前文档中心模型是 `bit_block<Bits>`、`bit_view`、`const_bit_view` 与自由算法。
- 不承诺为旧 `bitarray` API 提供代码级兼容层。

## 支持矩阵

| 范围 | 当前姿态 | 读者可以据此推断什么 |
| --- | --- | --- |
| Linux x86-64（GCC / Clang） | 主支持范围 | 主要构建、correctness 与 benchmark 注意力集中在这里。 |
| Windows x86-64（MSVC） | 主支持范围 | 与 Linux x86-64 一起构成当前主验证面。 |
| ARM64 与 macOS 等后续目标 | 次级范围 | 可以继续保留可构建性，但性能与成熟度主张必须更谨慎。 |
| 超出保留验证面的其他内容 | 不在宣称范围 | 没有新增证据之前，不应升级成支持承诺。 |

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

- 需要回看完整评审路径时，回到 [Guide](/zh/guide/index)；
- 需要重新理解架构论证时，回到 [Whitepaper](/zh/whitepaper/index)；
- 想把局部证据变成更稳妥结论前，请结合 [Performance](/zh/performance/index) 与 [Research](/zh/research/index) 一起看。
