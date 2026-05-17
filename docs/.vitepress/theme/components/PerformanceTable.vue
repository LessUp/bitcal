<script setup lang="ts">
import { computed } from 'vue'

export interface PerformanceRow {
  operation: string
  bitcal: string
  stdBitset: string
  ratio: string
  highlight?: boolean
}

const props = defineProps<{
  title?: string
  caption?: string
  rows: PerformanceRow[]
  highlightBest?: boolean
}>()

const processedRows = computed(() => {
  return props.rows.map(row => ({
    ...row,
    isBest: props.highlightBest && row.highlight
  }))
})
</script>

<template>
  <figure class="bitcal-perf-table">
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

<style scoped>
.bitcal-perf-table {
  margin: 1.5rem 0;
  overflow-x: auto;
}

.bitcal-perf-table figcaption {
  margin-bottom: 0.75rem;
  color: var(--vp-c-text-2);
}

.bitcal-perf-table figcaption strong {
  color: var(--vp-c-text-1);
  display: block;
  margin-bottom: 0.25rem;
}

.bitcal-perf-table table {
  width: 100%;
  border-collapse: separate;
  border-spacing: 0;
  border: 1px solid var(--vp-c-border);
  border-radius: 12px;
  overflow: hidden;
}

.bitcal-perf-table th,
.bitcal-perf-table td {
  padding: 0.65rem 0.9rem;
  text-align: left;
  border-bottom: 1px solid var(--vp-c-divider);
}

.bitcal-perf-table th {
  background: var(--bitcal-surface-2);
  font-weight: 620;
  font-size: 0.85rem;
  color: var(--vp-c-text-2);
}

.bitcal-perf-table tr:last-child td {
  border-bottom: none;
}

.bitcal-perf-table tr.best {
  background: color-mix(in oklab, var(--bitcal-positive) 8%, transparent);
}

.bitcal-perf-table tr.best td {
  font-weight: 600;
}

.bitcal-perf-table .operation {
  font-family: var(--vp-font-family-mono);
  font-size: 0.9rem;
}

.bitcal-perf-table .bitcal {
  color: var(--vp-c-brand-1);
  font-weight: 600;
}

.bitcal-perf-table .ratio span {
  display: inline-block;
  padding: 0.15rem 0.5rem;
  border-radius: 999px;
  font-size: 0.8rem;
  font-weight: 600;
}

.bitcal-perf-table .ratio span.faster {
  background: color-mix(in oklab, var(--bitcal-positive) 15%, transparent);
  color: var(--bitcal-positive);
}

.bitcal-perf-table .ratio span.slower {
  background: color-mix(in oklab, var(--bitcal-caution) 15%, transparent);
  color: var(--bitcal-caution);
}

.bitcal-perf-table tr:hover td {
  background: var(--bitcal-surface-1);
}
</style>
