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
    { text: '进入白皮书', href: './whitepaper/index', theme: 'brand' },
    { text: '检查证据页', href: './performance/index', theme: 'alt' },
    { text: '查看仓库', href: 'https://github.com/LessUp/bitcal', theme: 'alt' }
  ]"
>
</BitcalHero>

<FigureFrame title="阅读模型" caption="BitCal 先给出公共契约，再用证据、参考和外部比较支撑它。">
  <svg viewBox="0 0 720 360" role="img" aria-label="BitCal 阅读模型图">
    <rect x="32" y="28" width="656" height="64" rx="16" data-fill="surface" />
    <text x="60" y="67" fill="currentColor" font-size="24" font-weight="700">公共契约</text>
    <text x="60" y="92" fill="currentColor" font-size="15">bit_block, bit_view, const_bit_view, 自由算法, 稳定 include seam</text>

    <rect x="86" y="142" width="250" height="72" rx="18" data-fill="accent" />
    <text x="118" y="181" fill="currentColor" font-size="22" font-weight="650">白皮书</text>
    <text x="118" y="205" fill="currentColor" font-size="14">设计主线、分层、分发边界</text>

    <rect x="384" y="142" width="250" height="72" rx="18" data-fill="surface" />
    <text x="416" y="181" fill="currentColor" font-size="22" font-weight="650">性能</text>
    <text x="416" y="205" fill="currentColor" font-size="14">基线数字、限制条件、方法学说明</text>

    <rect x="86" y="256" width="250" height="72" rx="18" data-fill="surface" />
    <text x="118" y="295" fill="currentColor" font-size="22" font-weight="650">参考</text>
    <text x="118" y="319" fill="currentColor" font-size="14">类型、视图、算法级契约</text>

    <rect x="384" y="256" width="250" height="72" rx="18" data-fill="surface" />
    <text x="416" y="295" fill="currentColor" font-size="22" font-weight="650">研究</text>
    <text x="416" y="319" fill="currentColor" font-size="14">比较、资料来源、演进笔记</text>

    <path d="M360 92 L360 128" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M360 214 L360 242" stroke-width="2.5" fill="none" data-stroke="muted" />
    <path d="M210 214 L210 242" stroke-width="2.5" fill="none" data-stroke="muted" />
    <path d="M510 214 L510 242" stroke-width="2.5" fill="none" data-stroke="muted" />
  </svg>
</FigureFrame>

## 建议阅读顺序

<ReadingPathGrid
  :items="[
    {
      title: '白皮书',
      href: '/zh/whitepaper/index',
      summary: '先读公开模型、算法姿态与分发边界。',
      detail: '这里是整站的主论文线，不是附属说明。'
    },
    {
      title: '性能',
      href: '/zh/performance/index',
      summary: '再用 retained methodology 和支持范围解释当前 benchmark。',
      detail: 'BitCal 更偏好诚实基线，而不是泛化宣称。'
    },
    {
      title: '参考',
      href: '/zh/reference/index',
      summary: '在架构叙事清晰后，再确认契约面。',
      detail: '这里讲类型、视图和算法，而不是营销话术。'
    },
    {
      title: '研究',
      href: '/zh/research/index',
      summary: '最后用外部资料、相邻库与演进笔记做横向判断。',
      detail: '这部分负责补齐语境和比较维度。'
    }
  ]"
/>

## 这个站点实际在宣称什么

<EvidenceStrip
  :items="[
    { label: '稳定公开 include seam', value: '&lt;bitcal/bitcal.hpp&gt;' },
    { label: '主要支持姿态', value: 'C++23 + x86-64 优先' },
    { label: '文档态度', value: '证据优先于话术' },
    { label: 'Benchmark 姿态', value: '先有 baseline，再谈胆量' }
  ]"
/>

## 架构主线

<FigureFrame title="契约与实现边界" caption="库可以持续演进内核层，但不应把后端选择变成自身的公共身份。">
  <svg viewBox="0 0 720 260" role="img" aria-label="BitCal 契约与实现边界">
    <rect x="28" y="24" width="664" height="44" rx="14" data-fill="surface" />
    <text x="56" y="52" fill="currentColor" font-size="21" font-weight="680">稳定 include seam</text>
    <text x="250" y="52" fill="currentColor" font-size="18">&lt;bitcal/bitcal.hpp&gt;</text>

    <rect x="28" y="92" width="664" height="52" rx="14" data-fill="accent" />
    <text x="56" y="123" fill="currentColor" font-size="22" font-weight="680">公开模型</text>
    <text x="208" y="123" fill="currentColor" font-size="16">bit_block、bit_view、const_bit_view、自由算法</text>

    <rect x="28" y="168" width="664" height="52" rx="14" data-fill="surface" />
    <text x="56" y="199" fill="currentColor" font-size="22" font-weight="680">detail 层</text>
    <text x="208" y="199" fill="currentColor" font-size="16">dispatch 启发式、x86-64 内核、标量回退、保留基准路径</text>

    <path d="M360 68 L360 90" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M360 144 L360 166" stroke-width="2.5" fill="none" data-stroke="primary" />
  </svg>
</FigureFrame>

## 研究支点

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      note: '讨论 x86 SIMD intrinsic 与指令约束时的权威入口。'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: '用于理解 x86-64 上的延迟、吞吐与 dispatch 权衡。'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      note: '解释 broadword / word-parallel bit 技术时很有价值的背景资料。'
    }
  ]"
/>
