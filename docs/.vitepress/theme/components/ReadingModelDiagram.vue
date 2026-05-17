<script setup lang="ts">
defineProps<{
  lang: 'zh' | 'en'
}>()

const labels: Record<string, { zh: string; en: string }> = {
  publicContract: { zh: '公共契约', en: 'Public contract' },
  publicContractDetail: {
    zh: 'bit_block, bit_view, const_bit_view, 自由算法, 稳定 include seam',
    en: 'bit_block, bit_view, const_bit_view, free algorithms, stable include seam',
  },
  whitepaper: { zh: '白皮书', en: 'Whitepaper' },
  whitepaperDetail: {
    zh: '设计主线、分层、分发边界',
    en: 'Design thesis, layering, dispatch boundaries',
  },
  performance: { zh: '性能', en: 'Performance' },
  performanceDetail: {
    zh: '基线数字、限制条件、方法学说明',
    en: 'Baseline numbers, limits, methodology notes',
  },
  reference: { zh: '参考', en: 'Reference' },
  referenceDetail: {
    zh: '类型、视图、算法级契约',
    en: 'Types, views, algorithm-level contract',
  },
  research: { zh: '研究', en: 'Research' },
  researchDetail: {
    zh: '比较、资料来源、演进笔记',
    en: 'Comparisons, sources, evolution notes',
  },
}
</script>

<template>
  <svg viewBox="0 0 720 360" role="img" :aria-label="lang === 'zh' ? 'BitCal 阅读模型图' : 'BitCal reading model diagram'">
    <rect x="32" y="28" width="656" height="64" rx="16" data-fill="surface" />
    <text x="60" y="67" fill="currentColor" font-size="24" font-weight="700">{{ labels.publicContract[lang] }}</text>
    <text x="60" y="92" fill="currentColor" font-size="15">{{ labels.publicContractDetail[lang] }}</text>

    <rect x="86" y="142" width="250" height="72" rx="18" data-fill="accent" />
    <text x="118" y="181" fill="currentColor" font-size="22" font-weight="650">{{ labels.whitepaper[lang] }}</text>
    <text x="118" y="205" fill="currentColor" font-size="14">{{ labels.whitepaperDetail[lang] }}</text>

    <rect x="384" y="142" width="250" height="72" rx="18" data-fill="surface" />
    <text x="416" y="181" fill="currentColor" font-size="22" font-weight="650">{{ labels.performance[lang] }}</text>
    <text x="416" y="205" fill="currentColor" font-size="14">{{ labels.performanceDetail[lang] }}</text>

    <rect x="86" y="256" width="250" height="72" rx="18" data-fill="surface" />
    <text x="118" y="295" fill="currentColor" font-size="22" font-weight="650">{{ labels.reference[lang] }}</text>
    <text x="118" y="319" fill="currentColor" font-size="14">{{ labels.referenceDetail[lang] }}</text>

    <rect x="384" y="256" width="250" height="72" rx="18" data-fill="surface" />
    <text x="416" y="295" fill="currentColor" font-size="22" font-weight="650">{{ labels.research[lang] }}</text>
    <text x="416" y="319" fill="currentColor" font-size="14">{{ labels.researchDetail[lang] }}</text>

    <path d="M360 92 L360 128" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M360 214 L360 242" stroke-width="2.5" fill="none" data-stroke="muted" />
    <path d="M210 214 L210 242" stroke-width="2.5" fill="none" data-stroke="muted" />
    <path d="M510 214 L510 242" stroke-width="2.5" fill="none" data-stroke="muted" />
  </svg>
</template>
