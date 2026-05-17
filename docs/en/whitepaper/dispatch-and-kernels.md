# Dispatch and Kernels

BitCal documents dispatch as a boundary, not as a personality trait.

<FigureFrame title="Contract versus implementation" caption="Public docs stay above the line. Kernel freedom lives below it.">
  <svg viewBox="0 0 760 320" role="img" aria-label="Contract versus implementation boundary">
    <rect x="40" y="28" width="680" height="58" rx="18" data-fill="accent" />
    <text x="70" y="64" fill="currentColor" font-size="26" font-weight="700">Public contract</text>
    <text x="290" y="64" fill="currentColor" font-size="16">include seam, owner/view model, algorithm semantics</text>

    <path d="M60 128 L700 128" stroke-width="3" fill="none" data-stroke="primary" />
    <text x="70" y="118" fill="currentColor" font-size="14">implementation boundary</text>

    <rect x="40" y="160" width="200" height="94" rx="18" data-fill="surface" />
    <text x="72" y="198" fill="currentColor" font-size="22" font-weight="680">detail dispatch</text>
    <text x="72" y="226" fill="currentColor" font-size="14">policy, heuristics, backend selection</text>

    <rect x="280" y="160" width="200" height="94" rx="18" data-fill="surface" />
    <text x="312" y="198" fill="currentColor" font-size="22" font-weight="680">x86-64 kernels</text>
    <text x="312" y="226" fill="currentColor" font-size="14">primary optimization and validation path</text>

    <rect x="520" y="160" width="200" height="94" rx="18" data-fill="surface" />
    <text x="552" y="198" fill="currentColor" font-size="22" font-weight="680">scalar fallback</text>
    <text x="552" y="226" fill="currentColor" font-size="14">portable behavior floor</text>
  </svg>
</FigureFrame>

## Support posture

| Layer | Status | Documentation stance |
| --- | --- | --- |
| x86-64 optimized path | primary | claim carefully, measure openly |
| scalar fallback | retained | document as portability floor |
| secondary targets | follow-up | never claim stronger support than retained evidence |

## What this means for documentation

- describe backend policy only when it affects reader-visible behavior
- keep the whitepaper focused on boundaries, not on enumerating every intrinsic
- let performance pages explain where the active evidence is concentrated

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      note: 'Useful when mapping user-visible behavior to implementation-level instruction choices.'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: 'Helpful for reasoning about latency, throughput, and the cost of x86 dispatch choices.'
    }
  ]"
/>
