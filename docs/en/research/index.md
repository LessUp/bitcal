# Research

Research gives BitCal a context layer that is useful to engineers, not just decorative to readers. It collects the external references, related systems, and evolution notes that inform the redesign.

## Research map

This section answers three kinds of questions:

- which papers, manuals, and references underpin the performance and algorithm narrative;
- which nearby libraries or systems are the most relevant comparison points;
- which trade-offs BitCal is making by choosing a C++23, x86-64-first, owner/view/algorithm model.

## Reference set

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      meta: 'Instruction reference',
      note: 'Used when discussing x86 SIMD capabilities and instruction-level constraints.'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: 'Optimization manuals',
      note: 'Relevant for dispatch costs, latency, throughput, and measurement interpretation on x86-64.'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'Broadword paper',
      note: 'Important background for word-parallel bit techniques and for understanding why algorithm shape matters.'
    }
  ]"
/>

## Related systems

Use [Evolution and Comparisons](/en/research/evolution-and-comparisons) for the detailed comparison layer. The short list of meaningful reference points is:

- **Boost dynamic_bitset** for a C++-native dynamic bitset baseline;
- **CRoaring** for performance-conscious bitmap engineering with a different domain focus;
- **libpopcnt** for a focused example of how serious one low-level bit primitive can become.

## How this section fits the reading chain

Research should usually be read after [Reference](/en/reference/index), because external comparison only makes sense once BitCal's own contract is clear. End with [Status](/en/status/index) to bring the scope back down to what the repository actually supports today.
