# Performance

The value of the performance section lies in making the project's claims smaller but more credible.

<EvidenceStrip
  :items="[
    { label: 'Current Status', value: 'Reproducible baseline' },
    { label: 'Active Backend', value: 'AVX2' },
    { label: 'Optimization Focus', value: 'x86-64 first' }
  ]"
/>

## BitCal vs std::bitset Performance Comparison

The following data is from local benchmark tests using the AVX2 backend.

<PerformanceTable
  title="256-bit Operations"
  caption="BitCal uses AVX2 backend, std::bitset uses standard library implementation"
  :rows="[
    { operation: 'and<256>', bitcal: '1.34', stdBitset: '1.22', ratio: '0.91' },
    { operation: 'or<256>', bitcal: '1.02', stdBitset: '1.04', ratio: '1.02' },
    { operation: 'xor<256>', bitcal: '1.02', stdBitset: '1.02', ratio: '1.00' },
    { operation: 'popcount<256>', bitcal: '5.06', stdBitset: '1.90', ratio: '0.38' },
    { operation: 'shift_left<256>', bitcal: '1.68', stdBitset: '2.61', ratio: '1.56', highlight: true },
    { operation: 'shift_right<256>', bitcal: '1.27', stdBitset: '2.56', ratio: '2.01', highlight: true }
  ]"
  :highlightBest="true"
/>

<PerformanceTable
  title="512-bit Operations"
  caption="At larger widths, BitCal's SIMD advantage is more pronounced"
  :rows="[
    { operation: 'and<512>', bitcal: '1.76', stdBitset: '4.27', ratio: '2.43', highlight: true },
    { operation: 'shift_right<512>', bitcal: '3.13', stdBitset: '11.78', ratio: '3.76', highlight: true }
  ]"
  :highlightBest="true"
/>

<PerformanceTable
  title="1024-bit Operations"
  :rows="[
    { operation: 'popcount<1024>', bitcal: '8.10', stdBitset: '11.75', ratio: '1.45', highlight: true }
  ]"
  :highlightBest="true"
/>

## Key Findings

1. **Shift operations show significant advantage**: BitCal is **3.76x** faster on `shift_right<512>`
2. **Larger widths show bigger gains**: 512-bit operations have more significant speedup than 256-bit
3. **popcount needs optimization**: std::bitset is faster on popcount (likely using better intrinsics)

## Measurement Methodology

### Reproduction Commands

```bash
cmake -B build -DBITCAL_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --target benchmark_compare
./build/benchmarks/benchmark_compare
```

### Measurement Rules

| Rule | Purpose |
| --- | --- |
| Treat current results as baseline checkpoint | Avoid packaging smoke numbers as production promises |
| Present numbers with active backend context | Performance numbers without target context are meaningless |
| Preserve reproducible local command paths | Readers need a continuously challengeable verification path |

### Interpretation Guardrails

1. Local measurements are just local evidence
2. x86-64 priority optimization doesn't mean all targets are equally mature
3. Small synthetic loops are useful but don't represent all workloads

## Areas Still Maturing

- Aligned vs unaligned comparison
- Owner vs view workload differences
- More complete kernel family coverage
- Real workload traces complementary to synthetic loops

<CitationList
  :items="[
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: 'Helpful for evaluating instruction-level tradeoffs and why different microarchitectures tell different performance stories.'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      note: 'Reminds us that bit-level microbenchmarks deserve serious engineering, not casual display.'
    }
  ]"
/>
