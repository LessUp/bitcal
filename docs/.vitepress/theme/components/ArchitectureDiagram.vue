<script setup lang="ts">
defineProps<{
  lang: 'zh' | 'en'
}>()

const labels: Record<string, { zh: string; en: string }> = {
  stableIncludeSeam: { zh: '稳定 include seam', en: 'Stable include seam' },
  publicModel: { zh: '公开模型', en: 'Public model' },
  publicModelDetail: {
    zh: 'bit_block、bit_view、const_bit_view、自由算法',
    en: 'bit_block, bit_view, const_bit_view, free algorithms',
  },
  detailLayer: { zh: 'detail 层', en: 'Detail layer' },
  detailLayerDetail: {
    zh: 'dispatch 启发式、x86-64 内核、标量回退、保留基准路径',
    en: 'dispatch heuristics, x86-64 kernels, scalar fallback, retained benchmark path',
  },
}
</script>

<template>
  <svg viewBox="0 0 720 260" role="img" :aria-label="lang === 'zh' ? 'BitCal 契约与实现边界' : 'BitCal contract and implementation boundary'">
    <path d="M24 82 H696" fill="none" stroke-width="1" data-stroke="grid" />
    <path d="M24 156 H696" fill="none" stroke-width="1" data-stroke="grid" />

    <rect x="28" y="24" width="664" height="44" rx="14" data-fill="surface" data-node="seam" />
    <text x="56" y="46" font-size="19" font-weight="680" dominant-baseline="middle" data-slot="label">{{ labels.stableIncludeSeam[lang] }}</text>
    <text x="270" y="46" font-size="16" dominant-baseline="middle" data-slot="detail">&lt;bitcal/bitcal.hpp&gt;</text>

    <path d="M360 68 L360 90" stroke-width="2" fill="none" data-stroke="primary" />

    <rect x="28" y="90" width="664" height="56" rx="14" data-fill="accent" data-node="contract" />
    <text x="56" y="118" font-size="21" font-weight="680" dominant-baseline="middle" data-slot="label">{{ labels.publicModel[lang] }}</text>
    <text x="196" y="118" font-size="14" dominant-baseline="middle" data-slot="detail">{{ labels.publicModelDetail[lang] }}</text>

    <path d="M360 146 L360 164" stroke-width="2" fill="none" data-stroke="primary" />

    <rect x="28" y="164" width="664" height="56" rx="14" data-fill="node" data-node="detail" />
    <text x="56" y="192" font-size="21" font-weight="680" dominant-baseline="middle" data-slot="label">{{ labels.detailLayer[lang] }}</text>
    <text x="196" y="192" font-size="14" dominant-baseline="middle" data-slot="detail">{{ labels.detailLayerDetail[lang] }}</text>
  </svg>
</template>
