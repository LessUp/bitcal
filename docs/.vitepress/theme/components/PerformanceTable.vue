<script setup lang="ts">
import { computed } from 'vue'

export interface PerformanceRow {
  operation: string
  bitcal: string
  stdBitset: string
  ratio: string
  highlight?: boolean
}

const props = withDefaults(defineProps<{
  title?: string
  caption?: string
  rows: PerformanceRow[]
  highlightBest?: boolean
  tone?: 'default' | 'accent'
}>(), {
  highlightBest: false,
  tone: 'default',
})

const processedRows = computed(() => {
  return props.rows.map(row => ({
    ...row,
    isBest: props.highlightBest && row.highlight
  }))
})
</script>

<template>
  <figure class="bitcal-perf-table" :data-performance-tone="props.tone">
    <figcaption v-if="title">
      <strong>{{ title }}</strong>
      <span v-if="caption">{{ caption }}</span>
    </figcaption>
    <table>
      <thead>
        <tr>
          <th>Operation</th>
          <th>BitCal (ns)</th>
          <th>std::bitset (ns)</th>
          <th>Ratio</th>
        </tr>
      </thead>
      <tbody>
        <tr
          v-for="(row, idx) in processedRows"
          :key="idx"
          :class="{ 'best': row.isBest }"
        >
          <td class="operation">{{ row.operation }}</td>
          <td class="bitcal">{{ row.bitcal }}</td>
          <td class="std">{{ row.stdBitset }}</td>
          <td class="ratio">
            <span :class="{'faster': parseFloat(row.ratio) > 1, 'slower': parseFloat(row.ratio) < 1}">
              {{ row.ratio }}x
            </span>
          </td>
        </tr>
      </tbody>
    </table>
  </figure>
</template>
