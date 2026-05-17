# 分发与内核

BitCal 记录 dispatch，是为了说明边界，而不是为了把它包装成个性标签。

<FigureFrame title="契约与实现的边界线" caption="公共文档留在线上方，内核自由留在线下方。">
  <svg viewBox="0 0 760 320" role="img" aria-label="契约与实现边界">
    <rect x="40" y="28" width="680" height="58" rx="18" data-fill="accent" />
    <text x="70" y="64" fill="currentColor" font-size="26" font-weight="700">公共契约</text>
    <text x="290" y="64" fill="currentColor" font-size="16">include seam、owner/view 模型、算法语义</text>

    <path d="M60 128 L700 128" stroke-width="3" fill="none" data-stroke="primary" />
    <text x="70" y="118" fill="currentColor" font-size="14">implementation boundary</text>

    <rect x="40" y="160" width="200" height="94" rx="18" data-fill="surface" />
    <text x="72" y="198" fill="currentColor" font-size="22" font-weight="680">detail dispatch</text>
    <text x="72" y="226" fill="currentColor" font-size="14">策略、启发式、backend 选择</text>

    <rect x="280" y="160" width="200" height="94" rx="18" data-fill="surface" />
    <text x="312" y="198" fill="currentColor" font-size="22" font-weight="680">x86-64 kernels</text>
    <text x="312" y="226" fill="currentColor" font-size="14">主要优化与验证路径</text>

    <rect x="520" y="160" width="200" height="94" rx="18" data-fill="surface" />
    <text x="552" y="198" fill="currentColor" font-size="22" font-weight="680">scalar fallback</text>
    <text x="552" y="226" fill="currentColor" font-size="14">可移植行为地板</text>
  </svg>
</FigureFrame>

## 支持姿态

| 层 | 状态 | 文档原则 |
| --- | --- | --- |
| x86-64 优化路径 | primary | 谨慎宣称，公开测量 |
| scalar fallback | retained | 作为可移植行为下限来描述 |
| secondary targets | follow-up | 绝不宣称比 retained evidence 更强的支持 |

## 这对文档意味着什么

- 只有当 backend 策略会影响用户可观察行为时，才解释它
- 白皮书应聚焦边界，而不是枚举所有 intrinsic
- 性能页负责说明当前证据主要集中在哪一层

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      note: '把用户可观察行为映射到实现层指令选择时的权威参考。'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: '有助于理解 x86 下的延迟、吞吐与 dispatch 代价。'
    }
  ]"
/>
