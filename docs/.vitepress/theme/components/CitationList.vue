<script setup lang="ts">
defineProps<{
  items: Array<{
    title: string
    href: string
    note?: string
  }>
}>()
</script>

<template>
  <ol class="bitcal-citations">
    <li v-for="item in items" :key="item.href" class="bitcal-citations__item">
      <a :href="item.href" class="bitcal-citations__title">{{ item.title }}</a>
      <p v-if="item.note" class="bitcal-citations__note">{{ item.note }}</p>
    </li>
  </ol>
</template>
