<script setup lang="ts">
import { useSlots } from 'vue'

const slots = useSlots()

defineProps<{
  title?: string
  eyebrow?: string
  caption?: string
}>()
</script>

<template>
  <figure class="bc-figure-frame">
    <div v-if="eyebrow || slots.title || title" class="bc-figure-heading">
      <p v-if="eyebrow" class="bc-figure-eyebrow">{{ eyebrow }}</p>
      <div v-if="slots.title || title" class="bc-figure-title">
        <slot name="title">{{ title }}</slot>
      </div>
    </div>

    <div class="bc-figure-shell">
      <slot />
    </div>

    <figcaption v-if="slots.caption || caption" class="bc-figure-caption">
      <slot name="caption">{{ caption }}</slot>
    </figcaption>
  </figure>
</template>
