<script setup lang="ts">
import { withBase } from 'vitepress'

export type ReadingPathItem = {
  title: string
  href: string
  summary: string
  detail?: string
  badge?: string
  tone?: 'default' | 'primary' | 'whitepaper'
}

defineProps<{
  items: ReadingPathItem[]
}>()

const externalHrefPattern = /^(?:[a-z]+:)?\/\//i

function resolveReadingPathHref(href: string) {
  if (!href || href.startsWith('#') || href.startsWith('mailto:') || href.startsWith('tel:') || externalHrefPattern.test(href)) {
    return href
  }

  const match = href.match(/^([^?#]+)([?#].*)?$/)
  const path = match?.[1] ?? href
  const suffix = match?.[2] ?? ''
  const normalized = path.endsWith('/') || path.endsWith('.html') ? path : `${path}.html`

  return withBase(`${normalized}${suffix}`)
}
</script>

<template>
  <div class="bc-reading-grid">
    <a
      v-for="item in items"
      :key="item.href"
      :href="resolveReadingPathHref(item.href)"
      class="bc-reading-card"
      :data-reading-tone="item.tone ?? 'whitepaper'"
    >
      <div class="bc-reading-card-meta">
        <span v-if="item.badge" class="bc-reading-card-badge">{{ item.badge }}</span>
      </div>
      <div class="bc-reading-card-title">{{ item.title }}</div>
      <p class="bc-reading-card-summary">{{ item.summary }}</p>
      <p v-if="item.detail" class="bc-reading-card-detail">{{ item.detail }}</p>
      <span class="bc-reading-card-arrow" aria-hidden="true">→</span>
    </a>
  </div>
</template>
