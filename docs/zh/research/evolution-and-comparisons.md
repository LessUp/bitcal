# 演进与比较

只有当 BitCal 明确说出自己的邻近参照物与设计代价时，这套白皮书叙事才真正可信。这一页就是要把比较层从隐性前提变成显性资料。

## 引用文献

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      meta: '指令参考',
      note: '为 x86 SIMD 讨论提供可点击、可追溯的指令级资料。'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: '优化手册',
      note: '为吞吐、延迟与 dispatch 成本提供微架构语境。'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'Broadword 论文',
      note: '给 word-parallel 位操作提供理论与工程之间的桥梁。'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      meta: '库与源码',
      note: '提醒我们：哪怕只有一个计数原语，也值得高度严肃的工程投入。'
    }
  ]"
/>

## 相关系统

| 系统 | 值得比较的地方 |
| --- | --- |
| [Boost dynamic_bitset](https://www.boost.org/doc/libs/release/libs/dynamic_bitset/) | 展示另一种以动态 bitset 可用性为中心的 C++ 契约组织方式。 |
| [CRoaring](https://github.com/RoaringBitmap/CRoaring) | 展示在不同存储模型下，bitmap 性能工程可以被做到多系统化。 |
| [libpopcnt](https://github.com/kimwalisch/libpopcnt) | 范围更窄，但很好地说明“专注一个原语”可以带来多深的实现强度。 |

## 对比层级

BitCal 现在把对比范围拆成明确层级，而不是让一个 benchmark 二进制承担所有叙事：

1. **Retained baseline** —— `benchmark_compare` 只测当前 vNext 公共算法，对照 `std::bitset`，并直接喂给提交到仓库里的性能 artifacts。
2. **未来可选 comparator** —— Boost `dynamic_bitset`、CRoaring、`libpopcnt` 等仍然值得研究，但应始终以显式 opt-in 的方式出现在 retained 主叙事之外。

## 演进说明

BitCal 当前方向可以被理解为一连串刻意的收缩与重心转移：

1. 放弃单体单类型作为唯一公共中心；
2. 把 owner / view / algorithm 拆分正式写成公开模型；
3. 保留 `<bitcal/bitcal.hpp>` 作为稳定接缝，同时释放内部层次继续演化；
4. 把平台与性能主张收紧到 x86-64-first 的证据姿态。

这不是一个“以后再慢慢补功能”的松散路线图，而是在主动把系统变得更容易推理、benchmark 与审查。

## 设计取舍

这次重设计明确接受以下代价，以换取更清晰的边界：

- **减少兼容怀旧，换取更清楚的契约模型** —— 放弃旧 API 熟悉感，换来更干净的公开角色划分；
- **缩窄支持话术，换取更强证据密度** —— x86-64-first 比泛泛而谈的“广泛支持”更有价值；
- **优先契约表达，而不是源码树导览** —— 不是每个内部 helper 都值得升级为公开文档；
- **优先性能方法学，而不是 headline chasing** —— 让 retained baseline 和未来 opt-in comparator 实验保持分层，未来主张才能持续被挑战。

## 什么时候最值得读这页

当你想判断 BitCal 的姿态是否足够诚实时，就该读这一页；如果你要的是当下精确支持边界，请最后回到 [状态](/zh/status/)；如果你要的是契约原文，请回到 [Reference](/zh/reference/)。
