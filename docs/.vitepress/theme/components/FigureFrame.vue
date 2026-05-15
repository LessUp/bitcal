<script setup lang="ts">
defineProps<{
  title?: string
  eyebrow?: string
  caption?: string
}>()
</script>

<template>
  <figure class="bc-figure-frame">
    <div v-if="eyebrow || title" class="bc-figure-heading">
      <p v-if="eyebrow" class="bc-figure-eyebrow">{{ eyebrow }}</p>
      <div v-if="title" class="bc-figure-title">{{ title }}</div>
    </div>

    <div class="bc-figure-shell">
      <slot />
    </div>

    <figcaption v-if="caption" class="bc-figure-caption">{{ caption }}</figcaption>
  </figure>
</template>
