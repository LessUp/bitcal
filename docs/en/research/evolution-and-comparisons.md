# Evolution and Comparisons

BitCal becomes easier to trust when it names its neighbors and trade-offs directly. This page exists to make that comparison layer explicit instead of implicit.

## Citations

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      meta: 'Instruction reference',
      note: 'Grounds the x86 SIMD discussion in instruction-level reference material.'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: 'Optimization manuals',
      note: 'Adds microarchitectural context to throughput, latency, and dispatch arguments.'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'Broadword paper',
      note: 'Provides theoretical and practical background for word-parallel bit operations.'
    },
    {
      title: 'libpopcnt',
      href: 'https://github.com/kimwalisch/libpopcnt',
      meta: 'Library and source code',
      note: 'Reminds us how much rigor even one counting primitive can justify.'
    }
  ]"
/>

## Related systems

| System | Useful contrast |
| --- | --- |
| [Boost dynamic_bitset](https://www.boost.org/doc/libs/release/libs/dynamic_bitset/) | Flexible C++ dynamic bitset ergonomics with a different contract center. |
| [CRoaring](https://github.com/RoaringBitmap/CRoaring) | Performance-oriented bitmap engineering with a different storage model and workload focus. |
| [libpopcnt](https://github.com/kimwalisch/libpopcnt) | Narrower scope, but a strong reminder that bit-level performance work rewards deep specialization. |

## Evolution notes

BitCal's current direction can be read as a sequence of deliberate contractions and re-centering moves:

1. move away from a monolithic `bitarray` public center;
2. make the owner / view / algorithm split explicit;
3. keep `<bitcal/bitcal.hpp>` as the stable seam while freeing internal layout to evolve;
4. narrow platform and performance claims to an x86-64-first evidence posture.

This is not a generic “more features later” roadmap. It is a decision to make the system easier to reason about, benchmark, and defend.

## Design trade-offs

The redesign accepts some costs in exchange for clearer boundaries:

- **Less compatibility nostalgia, more contract clarity** — older API familiarity is traded away for a cleaner public role model.
- **Narrower support language, stronger evidence** — x86-64-first wording is more useful than broad claims that the repository cannot validate.
- **Reference discipline over source-tree tours** — not every internal helper is promoted into public docs.
- **Performance methodology over headline chasing** — baseline and method stay attached so that future claims remain challengeable.

## When this comparison layer is most useful

Use this page when you want to ask whether BitCal's posture is intellectually honest. If you want the exact present-tense support boundary, finish on [Status](/en/status/index). If you want the contract wording itself, go back to [Reference](/en/reference/index).
