<script setup lang="ts">
defineProps<{
  items: Array<{
    title: string
    href: string
    summary: string
    detail?: string
  }>
}>()
</script>

<template>
  <ol class="bitcal-reading-path" aria-label="Reading path">
    <li v-for="(item, index) in items" :key="item.href" class="bitcal-reading-path__item">
      <span class="bitcal-reading-path__index">{{ String(index + 1).padStart(2, '0') }}</span>
      <div class="bitcal-reading-path__body">
        <a :href="item.href" class="bitcal-reading-path__title">{{ item.title }}</a>
        <p class="bitcal-reading-path__summary">{{ item.summary }}</p>
        <p v-if="item.detail" class="bitcal-reading-path__detail">{{ item.detail }}</p>
      </div>
    </li>
  </ol>
</template>
