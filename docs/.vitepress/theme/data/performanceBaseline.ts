import summary from '../../../../benchmarks/results/retained/baseline-x86_64-avx2.summary.json'

type TableRow = {
  operation: string
  bitcal: string
  stdBitset: string
  ratio: string
  highlight?: boolean
}

type PerformanceBaseline = {
  profile: string
  statistic: string
  backend: string
  commit: string
  cpu: string
  warmupIterations: number
  samples: number
  iterationsPerSample: number
  tables: Record<string, TableRow[]>
}

export const performanceBaseline = summary as PerformanceBaseline

export const tableRows = (bits: string): TableRow[] => performanceBaseline.tables[bits] ?? []
