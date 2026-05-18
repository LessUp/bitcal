# Performance

BitCal treats performance as an evidence discipline, not as ambient marketing. This section deliberately separates the current baseline snapshot from the methodology used to produce and interpret it.

## Baseline snapshot

<EvidenceStrip
  :items="[
    { label: 'Current evidence target', value: 'AVX2 on x86-64', tone: 'accent' },
    { label: 'Status of numbers', value: 'Baseline checkpoint' },
    { label: 'What is stable', value: 'Method + command path' },
    { label: 'What is not promised', value: 'Universal wins', note: 'Local numbers are not blanket product guarantees.' }
  ]"
/>

The current retained baseline compares BitCal against `std::bitset` on representative fixed-width operations. These measurements are useful because they show where the redesign is already promising and where it is still immature.

<PerformanceTable
  title="256-bit operations"
  caption="Current local baseline on the active AVX2 path"
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
  title="512-bit operations"
  caption="Larger widths currently show a clearer SIMD advantage on selected paths"
  :rows="[
    { operation: 'and<512>', bitcal: '1.76', stdBitset: '4.27', ratio: '2.43', highlight: true },
    { operation: 'shift_right<512>', bitcal: '3.13', stdBitset: '11.78', ratio: '3.76', highlight: true }
  ]"
  :highlightBest="true"
/>

<PerformanceTable
  title="1024-bit operations"
  caption="A single larger-width checkpoint retained in the current benchmark set"
  :rows="[
    { operation: 'popcount<1024>', bitcal: '8.10', stdBitset: '11.75', ratio: '1.45', highlight: true }
  ]"
  :highlightBest="true"
/>

What the current baseline actually says:

- shift-heavy paths already show meaningful upside on the active x86-64 route;
- width matters, because some advantages only emerge once work amortizes dispatch and setup cost;
- counting primitives still need scrutiny and should not be described as solved simply because the redesign is performance-oriented.

## Measurement methodology

Performance claims remain attached to a reproducible command path and interpretation rules.

### Reproduction commands

```bash
cmake -S . -B build-test -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_BENCHMARKS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build-test --config Release --target benchmark_compare -j"$(nproc)"
./build-test/benchmarks/benchmark_compare
```

### Benchmark binary split

The performance evidence on this page comes from `benchmark_compare`, not from the smaller `bitcal_benchmark` smoke executable used in the guide.

| Binary | Role in the docs set | Why the split exists |
| --- | --- | --- |
| `benchmark_compare` | Publishes the BitCal-vs-`std::bitset` comparison tables shown on this page. | The baseline needs an explicit comparison harness with method and interpretation guardrails. |
| `bitcal_benchmark` | Stays in [Verification Path](/en/guide/verification) as the smoke-level executable baseline. | Verification needs a lighter executable check that is distinct from the published comparison experiment. |

### Method rules

| Rule | Why it exists |
| --- | --- |
| Treat current numbers as a retained **baseline checkpoint** | Prevent local benchmark output from becoming timeless marketing copy. |
| Always report the active backend and platform context | A speedup without ISA and platform context is meaningless. |
| Keep benchmark stories tied to public algorithm shapes | Numbers should map back to documented operations, not to unnamed kernel trivia. |
| Separate measurement from interpretation | Reproduction commands are evidence; conclusions are arguments that can be challenged. |

### Interpretation guardrails

- Synthetic loops are useful, but they do not represent every workload.
- An x86-64-first posture is a support choice, not proof that all other targets are equally mature.
- A benchmark win in one algorithm family does not automatically justify a broader API or platform claim.

## Claim boundary

This section intentionally refuses to claim more than the evidence supports.

**Safe claims today**

- BitCal retains a reproducible x86-64 benchmark path.
- Some fixed-width operations already outperform `std::bitset` in the current local baseline.
- Performance discussion is grounded in named algorithms, named widths, and explicit backend context.

**Claims that remain out of bounds**

- universal superiority over standard-library or specialized bitmap implementations;
- equal maturity across ARM64, macOS, and future x86 backends;
- workload-level promises that are not backed by retained traces or scenario-specific measurements.

## Where performance work goes next

The next useful expansions are methodological, not theatrical:

- aligned versus unaligned comparisons;
- owner versus borrowed-view workload differences;
- more complete coverage of counting and scan-style primitives;
- workload traces that complement the synthetic baseline.

For design context, return to the [Whitepaper](/en/whitepaper/). For contract language, continue into the [Reference](/en/reference/). For external comparison material, use [Research](/en/research/).
