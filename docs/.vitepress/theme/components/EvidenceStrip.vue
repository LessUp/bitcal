<script setup lang="ts">
defineProps<{
  items: Array<{
    label: string
    value: string
  }>
}>()
</script>

<template>
  <dl class="bitcal-evidence-strip">
    <div v-for="item in items" :key="item.label" class="bitcal-evidence-strip__item">
      <dt>{{ item.label }}</dt>
      <dd>{{ item.value }}</dd>
    </div>
  </dl>
</template>
