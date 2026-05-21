# Performance

<script setup>
import { performanceBaseline, tableRows } from '../../.vitepress/theme/data/performanceBaseline'
</script>

BitCal treats performance as an evidence discipline, not as ambient marketing. This section now renders from the retained benchmark artifacts committed under `benchmarks/results/retained/`, so the public tables and the repository evidence stay coupled.

## Baseline snapshot

<EvidenceStrip
  :items="[
    { label: 'Current evidence target', value: `${performanceBaseline.backend} on x86-64`, tone: 'accent' },
    { label: 'Status of numbers', value: 'Retained checkpoint' },
    { label: 'Published statistic', value: 'median ns/op', note: 'Each row comes from the committed summary artifact, not a hand-picked run.' },
    { label: 'Committed source', value: performanceBaseline.commit },
    { label: 'ARM status', value: 'ARM rows stay blank', note: 'No retained ARM benchmark path exists yet.' },
    { label: 'What is not promised', value: 'Universal wins', note: 'Committed local numbers are checkpoints, not blanket product guarantees.' }
  ]"
/>

The retained baseline now limits itself to the current vNext public surface: `bit_and`, `popcount`, and `is_zero`. Wider `bitarray`-era comparisons still exist, but only in the legacy/research lane and not as the primary whitepaper evidence chain.

ARM rows stay blank until the project retains an ARM benchmark path with the same level of reproducibility and committed artifacts.

<PerformanceTable
  title="128-bit retained operations"
  :caption="`Retained baseline on ${performanceBaseline.backend} (${performanceBaseline.commit})`"
  :rows="tableRows('128')"
  :highlightBest="true"
/>

<PerformanceTable
  title="192-bit retained operations"
  caption="Custom-width checkpoint kept in the retained baseline"
  :rows="tableRows('192')"
  :highlightBest="true"
/>

<PerformanceTable
  title="256-bit retained operations"
  caption="Representative fixed-width checkpoint on the active x86-64 path"
  :rows="tableRows('256')"
  :highlightBest="true"
/>

<PerformanceTable
  title="512-bit retained operations"
  caption="Larger-width public algorithms on the retained path"
  :rows="tableRows('512')"
  :highlightBest="true"
/>

What the current retained baseline actually says:

- wins are narrower than the old hand-written table implied;
- some vNext public operations are currently at parity with `std::bitset`, not ahead of it;
- 128-bit and 192-bit `bit_and` are still materially behind `std::bitset`, which is precisely the kind of gap a retained evidence path should expose instead of hiding.

## Measurement methodology

Performance claims remain attached to a reproducible command path and interpretation rules.

### Reproduction commands

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --target benchmark_compare -j"$(nproc)"
./build-test/benchmarks/benchmark_compare --json-out benchmarks/results/retained/baseline-x86_64-avx2.json
node benchmarks/scripts/generate-performance-summary.mjs \
  benchmarks/results/retained/baseline-x86_64-avx2.json \
  benchmarks/results/retained/baseline-x86_64-avx2.summary.json
```

### Benchmark binary split

The performance evidence on this page comes from `benchmark_compare`, not from the smaller `bitcal_benchmark` smoke executable used in the guide.

| Binary | Role in the docs set | Why the split exists |
| --- | --- | --- |
| `benchmark_compare` | Publishes the retained vNext baseline for the current public algorithms and writes the raw JSON artifact. | The baseline needs a reproducible comparison harness, a structured report, and an explicit claim boundary. |
| `benchmark_compare_legacy` | Keeps the broader `bitarray`-era comparison as a compatibility/research lane. | Exploration is useful, but it must not be confused with the retained whitepaper evidence chain. |
| `bitcal_benchmark` | Stays in [Verification Path](/en/guide/verification) as the smoke-level executable baseline. | Verification needs a lighter executable check that is distinct from the published comparison experiment. |

### Method rules

| Rule | Why it exists |
| --- | --- |
| Treat current numbers as a retained **baseline checkpoint** | Prevent local benchmark output from becoming timeless marketing copy. |
| Publish table rows from per-scenario medians | Median rows are less sensitive to occasional scheduling noise than a single best case or ad-hoc average. |
| Always report the active backend, CPU, and commit context | A speedup without ISA, machine, and revision context is meaningless. |
| Keep benchmark stories tied to public algorithm shapes | Numbers should map back to documented public algorithms, not unnamed kernel trivia. |
| Separate retained evidence from legacy or research lanes | Not every useful experiment belongs in the primary whitepaper story. |

### Interpretation guardrails

- Synthetic loops are useful, but they do not represent every workload.
- An x86-64-first posture is a support choice, not proof that all other targets are equally mature.
- A benchmark win in one algorithm family does not automatically justify a broader API or platform claim.
- A benchmark loss is still valuable evidence; the point of the retained baseline is honesty, not theater.

## Claim boundary

This section intentionally refuses to claim more than the evidence supports.

**Safe claims today**

- BitCal retains a reproducible x86-64 benchmark path with committed raw and summary artifacts.
- The retained baseline is grounded in named public algorithms, named widths, and explicit backend/commit context.
- BitCal is not uniformly ahead of `std::bitset` yet, and the retained evidence makes that visible.

**Claims that remain out of bounds**

- universal superiority over standard-library or specialized bitmap implementations;
- equal maturity across ARM64, macOS, and future x86 backends;
- workload-level promises that are not backed by retained traces or scenario-specific measurements.

## Where performance work goes next

The next useful expansions are methodological, not theatrical:

- aligned versus unaligned comparisons;
- owner versus borrowed-view workload differences;
- more complete coverage once the free-algorithm surface expands beyond the current vNext trio;
- workload traces that complement the synthetic retained baseline.

For design context, return to the [Whitepaper](/en/whitepaper/). For contract language, continue into the [Reference](/en/reference/). For external comparison material, use [Research](/en/research/).
