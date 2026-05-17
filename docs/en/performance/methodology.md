# Methodology

Benchmark credibility starts with measurement design.

## Current measurement rules

| Rule | Why it exists |
| --- | --- |
| treat current results as a baseline checkpoint | avoids presenting smoke numbers as finished product claims |
| print or state the active backend | performance numbers are meaningless without target context |
| keep a reproducible local command path | readers need a retained way to challenge the result |

## Interpretation guardrails

1. Local measurements are local evidence.
2. x86-64-first optimization work does not imply equal maturity on every target.
3. Small synthetic loops are useful, but they are not the whole workload story.

## What still needs to mature

- aligned versus unaligned comparisons
- owner-versus-view workload differences
- broader kernel family coverage
- real workload traces that complement synthetic loops

<CitationList
  :items="[
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: 'Useful background when evaluating instruction-level trade-offs and why different microarchitectures can tell different stories.'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      note: 'A concrete reminder that bit-level microbenchmarks deserve careful engineering, not casual presentation.'
    }
  ]"
/>
