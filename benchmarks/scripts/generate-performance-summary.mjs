import fs from 'node:fs'
import path from 'node:path'

const [inputPath, outputPath] = process.argv.slice(2)

if (!inputPath || !outputPath) {
  console.error('Usage: node benchmarks/scripts/generate-performance-summary.mjs <raw.json> <summary.json>')
  process.exit(1)
}

const report = JSON.parse(fs.readFileSync(inputPath, 'utf8'))

const byBits = new Map()
for (const scenario of report.scenarios) {
  const key = String(scenario.bits)
  if (!byBits.has(key)) byBits.set(key, [])

  const bitcalMedian = Number(scenario.bitcal.median_ns)
  const stdMedian = Number(scenario.std_bitset.median_ns)
  byBits.get(key).push({
    operation: `${scenario.operation}<${scenario.bits}>`,
    bitcal: bitcalMedian.toFixed(2),
    stdBitset: stdMedian.toFixed(2),
    ratio: bitcalMedian === 0 ? '0.00' : (stdMedian / bitcalMedian).toFixed(2),
    highlight: stdMedian > bitcalMedian,
  })
}

const summary = {
  profile: report.metadata.profile,
  backend: report.environment.backend,
  commit: report.environment.commit,
  cpu: report.environment.cpu,
  warmupIterations: report.metadata.warmup_iterations,
  samples: report.metadata.samples,
  iterationsPerSample: report.metadata.iterations_per_sample,
  tables: Object.fromEntries(byBits),
}

fs.mkdirSync(path.dirname(outputPath), { recursive: true })
fs.writeFileSync(outputPath, `${JSON.stringify(summary, null, 2)}\n`)
