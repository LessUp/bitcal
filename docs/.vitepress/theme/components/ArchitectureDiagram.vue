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
    <rect x="28" y="24" width="664" height="44" rx="14" data-fill="surface" />
    <text x="56" y="52" fill="currentColor" font-size="21" font-weight="680">{{ labels.stableIncludeSeam[lang] }}</text>
    <text x="250" y="52" fill="currentColor" font-size="18">&lt;bitcal/bitcal.hpp&gt;</text>

    <rect x="28" y="92" width="664" height="52" rx="14" data-fill="accent" />
    <text x="56" y="123" fill="currentColor" font-size="22" font-weight="680">{{ labels.publicModel[lang] }}</text>
    <text x="208" y="123" fill="currentColor" font-size="16">{{ labels.publicModelDetail[lang] }}</text>

    <rect x="28" y="168" width="664" height="52" rx="14" data-fill="surface" />
    <text x="56" y="199" fill="currentColor" font-size="22" font-weight="680">{{ labels.detailLayer[lang] }}</text>
    <text x="208" y="199" fill="currentColor" font-size="16">{{ labels.detailLayerDetail[lang] }}</text>

    <path d="M360 68 L360 90" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M360 144 L360 166" stroke-width="2.5" fill="none" data-stroke="primary" />
  </svg>
</template>
