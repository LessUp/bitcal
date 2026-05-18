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
  <svg viewBox="0 0 720 380" preserveAspectRatio="xMidYMid meet" role="img" :aria-label="lang === 'zh' ? 'BitCal 阅读模型图' : 'BitCal reading model diagram'">
    <!-- 公共契约 / Public contract -->
    <rect x="32" y="28" width="656" height="68" rx="16" data-fill="surface" />
    <text x="60" y="56" fill="currentColor" font-size="22" font-weight="700" dominant-baseline="auto">{{ labels.publicContract[lang] }}</text>
    <text x="60" y="80" fill="currentColor" font-size="14" opacity="0.72">{{ labels.publicContractDetail[lang] }}</text>

    <!-- 连线：契约 → 中层 -->
    <path d="M360 96 L360 138" stroke-width="2" fill="none" data-stroke="primary" />

    <!-- 中层：白皮书 / Whitepaper -->
    <rect x="40" y="138" width="296" height="76" rx="16" data-fill="accent" />
    <text x="72" y="171" fill="currentColor" font-size="21" font-weight="650">{{ labels.whitepaper[lang] }}</text>
    <text x="72" y="196" fill="currentColor" font-size="13" opacity="0.8">{{ labels.whitepaperDetail[lang] }}</text>

    <!-- 中层：性能 / Performance -->
    <rect x="384" y="138" width="296" height="76" rx="16" data-fill="surface" />
    <text x="416" y="171" fill="currentColor" font-size="21" font-weight="650">{{ labels.performance[lang] }}</text>
    <text x="416" y="196" fill="currentColor" font-size="13" opacity="0.72">{{ labels.performanceDetail[lang] }}</text>

    <!-- 连线：中层 → 底层 -->
    <path d="M188 214 L188 258" stroke-width="2" fill="none" data-stroke="muted" />
    <path d="M532 214 L532 258" stroke-width="2" fill="none" data-stroke="muted" />

    <!-- 底层：参考 / Reference -->
    <rect x="40" y="258" width="296" height="76" rx="16" data-fill="surface" />
    <text x="72" y="291" fill="currentColor" font-size="21" font-weight="650">{{ labels.reference[lang] }}</text>
    <text x="72" y="315" fill="currentColor" font-size="13" opacity="0.72">{{ labels.referenceDetail[lang] }}</text>

    <!-- 底层：研究 / Research -->
    <rect x="384" y="258" width="296" height="76" rx="16" data-fill="surface" />
    <text x="416" y="291" fill="currentColor" font-size="21" font-weight="650">{{ labels.research[lang] }}</text>
    <text x="416" y="315" fill="currentColor" font-size="13" opacity="0.72">{{ labels.researchDetail[lang] }}</text>
  </svg>
</template>
