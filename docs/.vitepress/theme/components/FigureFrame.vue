<script setup lang="ts">
defineProps<{
  title: string
  caption?: string
}>()
</script>

<template>
  <figure class="bitcal-figure">
    <div class="bitcal-figure__header">
      <strong class="bitcal-figure__title">{{ title }}</strong>
      <p v-if="caption" class="bitcal-figure__caption">{{ caption }}</p>
    </div>
    <div class="bitcal-figure__canvas">
      <slot />
    </div>
  </figure>
</template>
