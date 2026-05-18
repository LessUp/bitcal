# 迁移姿态

BitCal vNext 不是在旧公开模型外面做一层新包装，而是在替换契约中心。

## 为什么 vNext 要打破旧前提

旧的单体式公开结构，把 ownership、borrowing 与行为绑得太紧。这样既不利于把契约讲清楚，也不利于在不拖累公开 API 的前提下改写 dispatch 与 kernel。

因此当前方向明确选择：

- 用 `bit_block<Bits>` 表达 owning storage；
- 用 `bit_view` 与 `const_bit_view` 表达 borrowed access；
- 用自由算法承载行为中心。

## 什么仍然保持稳定

即使在重设计阶段，仍有一些点被刻意固定：

- `<bitcal/bitcal.hpp>` 依旧是唯一稳定 include seam；
- 基于 CMake 的本地验证路径仍是保留的评审入口；
- 文档、测试、benchmark 与 OpenSpec 需要一起移动。

## 什么被刻意保持不稳定

读者不应默认当前项目已经承诺：

- 提供旧 API 的代码级兼容层；
- 为所有架构维持同等成熟的优化水平；
- 在保留 x86-64 baseline 之外作出更宽泛的性能结论。

## 如何评估迁移风险

看这次重设计时，建议问三个问题：

1. owner / view / algorithm 的拆分，是否比旧中心模型更容易推理？
2. dispatch 边界，是否足以保护公开契约免于后端重写带来的噪音？
3. benchmark 与 Status 页面，是否足够克制，能把迁移承诺说实？

如果这三个问题的答案都是肯定的，那么即使 kernel 家族尚未全部成熟，这次重设计仍然是在做对的工程收口。
