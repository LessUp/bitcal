<script setup lang="ts">
import { useSlots } from 'vue'

defineProps<{
  eyebrow: string
  title: string
  lead: string
  stats: Array<{
    label: string
    value: string
  }>
  actions?: Array<{
    text: string
    href: string
    theme?: 'brand' | 'alt'
  }>
}>()

const slots = useSlots()
</script>

<template>
  <section class="bitcal-hero" :class="{ 'bitcal-hero--split': Boolean(slots.default) }">
    <div class="bitcal-hero__copy">
      <p class="bitcal-hero__eyebrow">{{ eyebrow }}</p>
      <h1 class="bitcal-hero__title">{{ title }}</h1>
      <p class="bitcal-hero__lead">{{ lead }}</p>
      <dl class="bitcal-hero__stats">
        <div v-for="stat in stats" :key="stat.label" class="bitcal-hero__stat">
          <dt>{{ stat.label }}</dt>
          <dd>{{ stat.value }}</dd>
        </div>
      </dl>
      <div v-if="actions?.length" class="bitcal-hero__actions">
        <a
          v-for="action in actions"
          :key="action.href"
          class="VPButton medium"
          :class="action.theme ?? 'alt'"
          :href="action.href"
        >
          {{ action.text }}
        </a>
      </div>
    </div>

    <div v-if="slots.default" class="bitcal-hero__aside">
      <slot />
    </div>
  </section>
</template>
