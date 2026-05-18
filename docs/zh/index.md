---
layout: home
title: BitCal
---

<BitcalHero
  eyebrow="这是系统设计资料，不是产品宣传页"
  title="把 BitCal 当作一次重设计审查来读，而不是当作一张功能海报。"
  lead="BitCal 把当前 vNext 工作组织成一套面向高级 C++ 读者的技术白皮书：公开模型、算法组织、分发边界、性能方法学、契约参考和研究对照连成一条证据链。"
  :stats="[
    { label: '交付形态', value: 'Header-only' },
    { label: '稳定接缝', value: '<bitcal/bitcal.hpp>' },
    { label: '当前基线', value: 'C++23 · x86-64 first' }
  ]"
  :actions="[
    { text: '先看导读', href: '/zh/guide/', theme: 'brand' },
    { text: '进入白皮书', href: '/zh/whitepaper/', theme: 'alt' },
    { text: '查看性能证据', href: '/zh/performance/', theme: 'alt' }
  ]"
>
</BitcalHero>

## 白皮书阅读契约

这个站点默认读者会先问六个问题，再决定要不要继续相信一个底层库：

1. **公开模型是什么？** BitCal 正在收敛到 `bit_block<Bits>`、`bit_view`、`const_bit_view` 与自由算法。
2. **什么是稳定承诺？** `<bitcal/bitcal.hpp>` 仍是唯一稳定 include seam。
3. **实现自由从哪里开始？** dispatch 与 kernel 位于公开契约之下。
4. **性能到底证明了多少？** 只对当前保留下来的 baseline 与 methodology 发言。
5. **哪些页面可以当参考资料？** Reference 用契约语言解释角色与语义，而不是逐个内部头文件导览。
6. **设计依据来自哪里？** Research 收录指令手册、论文与相关系统，用来支撑而不是装饰叙事。

<ReadingPathGrid
  :items="[
    {
      title: 'Guide',
      href: '/zh/guide/',
      badge: '1 · 定位',
      tone: 'primary',
      summary: '先看读者范围、验证路径与迁移姿态。',
      detail: '这是从“刚看到仓库”到“可以认真评审设计”的最短路线。'
    },
    {
      title: 'Whitepaper',
      href: '/zh/whitepaper/',
      badge: '2 · 架构',
      tone: 'whitepaper',
      summary: '沿着系统架构主线阅读公开模型、算法组织与分发边界。',
      detail: 'owner / view / algorithm、自由算法组织和 kernel 边界都在这里被说清。'
    },
    {
      title: 'Performance',
      href: '/zh/performance/',
      badge: '3 · 证据',
      summary: '把当前 baseline 与测量方法学分开阅读。',
      detail: '任何数字都必须带着复现命令、活跃后端语境和宣称边界一起出现。'
    },
    {
      title: 'Reference',
      href: '/zh/reference/',
      badge: '4 · 契约',
      summary: '在架构主线清楚之后，再确认类型、视图和算法契约。',
      detail: 'Reference 解释读者可以依赖什么，而不是展示今天的内部实现细节。'
    },
    {
      title: 'Research',
      href: '/zh/research/',
      badge: '5 · 对照',
      summary: '查看引用资料、相关系统、演进说明与设计取舍。',
      detail: 'Research 的目标是提升判断密度，而不是制造“学术感”。'
    },
    {
      title: 'Status',
      href: '/zh/status/',
      badge: '6 · 边界',
      summary: '最后回到发布姿态、支持矩阵与文档真相入口。',
      detail: 'Status 会把所有主张重新收束到仓库当前真能支撑的范围。'
    }
  ]"
/>

## 公开模型速览

<EvidenceStrip
  :items="[
    { label: '拥有者角色', value: 'bit_block<Bits>', tone: 'accent' },
    { label: '借用者角色', value: 'bit_view / const_bit_view' },
    { label: '行为中心', value: '自由算法' },
    { label: '实现边界', value: 'Dispatch + kernel layer', note: '不是稳定用户身份的一部分。' }
  ]"
/>

首页不打算替代深层页面，它只负责告诉你阅读地图：

- [Guide](/zh/guide/) 解释现在应该如何读这个项目；
- [Whitepaper](/zh/whitepaper/) 陈述架构论点；
- [Performance](/zh/performance/) 把 baseline 与 methodology 分离；
- [Reference](/zh/reference/) 说明公开契约表面；
- [Research](/zh/research/) 提供外部对照和资料来源；
- [Status](/zh/status/) 定义发布与支持边界。

## 证据姿态

<FigureFrame title="为什么阅读顺序必须成立" caption="BitCal 希望读者按架构 → 证据 → 契约 → 对照 → 状态边界的顺序形成判断。">
  <ArchitectureDiagram lang="zh" />
</FigureFrame>

这个站点刻意采用更窄、但更能自证的叙事：

- **C++23 baseline** 是已经落下的设计前提，不是未来愿景。
- **x86-64-first posture** 表示优化与性能解释优先集中在当前保有证据的目标平台。
- **Header-only 形态** 并不意味着可以跳过验证路径、benchmark discipline 或支持边界说明。
- **Whitepaper-first 文档策略** 要求架构、方法学和参考资料彼此连通，而不是被拆成营销页与源码注释两条线。

## 研究支点

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      meta: '指令参考',
      note: '讨论 x86 SIMD 时，优先回到权威的指令级资料。'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: '微架构手册',
      note: '用来解释延迟、吞吐与 dispatch 策略为什么会改变底层库的真实表现。'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'Broadword 论文',
      note: '把位级语义与 word-parallel 技术路线连接起来，是理解算法设计的重要背景。'
    }
  ]"
/>
