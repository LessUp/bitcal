<script setup lang="ts">
defineProps<{
  items: Array<{
    label: string
    value: string
    note?: string
    tone?: 'default' | 'accent'
  }>
}>()
</script>

<template>
  <dl class="bitcal-evidence-strip">
    <div
      v-for="item in items"
      :key="item.label"
      class="bitcal-evidence-strip__item"
      :data-evidence-tone="item.tone ?? 'default'"
    >
      <dt>{{ item.label }}</dt>
      <dd>{{ item.value }}</dd>
      <p v-if="item.note" class="bitcal-evidence-strip__note">{{ item.note }}</p>
    </div>
  </dl>
</template>
