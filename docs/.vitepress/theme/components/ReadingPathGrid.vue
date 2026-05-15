<script setup lang="ts">
import { withBase } from 'vitepress'

export type ReadingPathItem = {
  title: string
  href: string
  summary: string
  badge?: string
}

defineProps<{
  items: ReadingPathItem[]
}>()
</script>

<template>
  <div class="bc-reading-grid">
    <a
      v-for="item in items"
      :key="item.href"
      :href="withBase(item.href)"
      class="bc-reading-card"
    >
      <div class="bc-reading-card-meta">
        <span v-if="item.badge" class="bc-reading-card-badge">{{ item.badge }}</span>
      </div>
      <div class="bc-reading-card-title">{{ item.title }}</div>
      <p class="bc-reading-card-summary">{{ item.summary }}</p>
      <span class="bc-reading-card-arrow" aria-hidden="true">→</span>
    </a>
  </div>
</template>
