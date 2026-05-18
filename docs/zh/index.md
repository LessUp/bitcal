---
layout: home
title: BitCal
---

<BitcalHero
  eyebrow="白皮书优先的系统文档"
  title="像做架构评审一样评估 BitCal。"
  lead="BitCal vNext 不再把自己包装成泛化的功能清单，而是用技术白皮书的方式展开：公开模型、分发边界、benchmark 姿态与参考材料围绕同一条设计主线组织。"
  :stats="[
    { label: '交付方式', value: 'Header-only' },
    { label: '语言基线', value: 'C++23' },
    { label: '主要优化姿态', value: 'x86-64 优先' }
  ]"
  :actions="[
    { text: '先看导读', href: '/zh/guide/index', theme: 'brand' },
    { text: '进入白皮书', href: '/zh/whitepaper/index', theme: 'alt' },
    { text: '查看状态', href: '/zh/status/index', theme: 'alt' }
  ]"
>
</BitcalHero>

<FigureFrame eyebrow="Reading order" title="阅读模型" tone="contrast" caption="BitCal 先用导读建立评估上下文，再用白皮书、性能、参考、研究和状态页逐层收敛主张。">
  <ReadingModelDiagram lang="zh" />
</FigureFrame>

## 建议阅读顺序

<ReadingPathGrid
  :items="[
    {
      title: '导读',
      href: '/zh/guide/index',
      badge: '入口',
      tone: 'primary',
      summary: '先建立阅读路径、验证入口与迁移姿态。',
      detail: '这是从“刚打开仓库”到“能严肃评估它”的最短路径。'
    },
    {
      title: '白皮书',
      href: '/zh/whitepaper/index',
      badge: '主论文线',
      tone: 'whitepaper',
      summary: '再读公开模型、算法姿态与分发边界。',
      detail: '这里是整站的主论文线，不是附属说明。'
    },
    {
      title: '性能',
      href: '/zh/performance/index',
      badge: '证据',
      summary: '用 retained methodology 与 claim guardrails 解读 benchmark。',
      detail: 'BitCal 更偏好诚实基线，而不是泛化宣称。'
    },
    {
      title: '参考',
      href: '/zh/reference/index',
      badge: '契约',
      summary: '在架构叙事清晰后，再确认契约面。',
      detail: '这里讲类型、视图和算法，而不是营销话术。'
    },
    {
      title: '研究',
      href: '/zh/research/index',
      badge: '语境',
      summary: '最后用外部资料、相邻库与演进笔记做横向判断。',
      detail: '这部分负责补齐语境和比较维度。'
    },
    {
      title: '状态',
      href: '/zh/status/index',
      badge: '边界',
      summary: '检查支持边界、破坏性变更姿态与单一真相入口。',
      detail: '这里负责把整站主张收窄回仓库真正能支撑的范围。'
    }
  ]"
/>

## 这个站点实际在宣称什么

<EvidenceStrip
  :items="[
    { label: '稳定公开 include seam', value: '&lt;bitcal/bitcal.hpp&gt;', tone: 'accent' },
    { label: '主要支持姿态', value: 'C++23 + x86-64 优先' },
    { label: '文档态度', value: '证据优先于话术' },
    { label: 'Benchmark 姿态', value: '先有 baseline，再谈胆量', note: '所有数字都需要继续绑定方法与边界。' }
  ]"
/>

## 架构主线

<FigureFrame title="契约与实现边界" caption="库可以持续演进内核层，但不应把后端选择变成自身的公共身份。">
  <ArchitectureDiagram lang="zh" />
</FigureFrame>

## 研究支点

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      meta: 'Intel · reference',
      note: '讨论 x86 SIMD intrinsic 与指令约束时的权威入口。'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: 'Agner Fog · manuals',
      note: '用于理解 x86-64 上的延迟、吞吐与 dispatch 权衡。'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'arXiv · paper',
      note: '解释 broadword / word-parallel bit 技术时很有价值的背景资料。'
    }
  ]"
/>
