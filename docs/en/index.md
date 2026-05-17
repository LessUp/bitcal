---
layout: home
title: BitCal
---

<BitcalHero
  eyebrow="Whitepaper-first systems documentation"
  title="Evaluate BitCal the way you would evaluate an architecture review."
  lead="BitCal vNext is presented as a technical whitepaper: public model, dispatch boundaries, benchmark posture, and reference material arranged for readers who care about contract fidelity more than feature slogans."
  :stats="[
    { label: 'Delivery', value: 'Header-only' },
    { label: 'Language baseline', value: 'C++23' },
    { label: 'Primary optimization posture', value: 'x86-64 first' }
  ]"
  :actions="[
    { text: 'Read the whitepaper', href: './whitepaper/index', theme: 'brand' },
    { text: 'Inspect the evidence', href: './performance/index', theme: 'alt' },
    { text: 'View the repository', href: 'https://github.com/LessUp/bitcal', theme: 'alt' }
  ]"
>
</BitcalHero>

<FigureFrame title="Reading model" caption="BitCal leads with public contract, then proves itself through evidence, reference, and comparative context.">
  <svg viewBox="0 0 720 360" role="img" aria-label="BitCal reading model diagram">
    <rect x="32" y="28" width="656" height="64" rx="16" data-fill="surface" />
    <text x="60" y="67" fill="currentColor" font-size="24" font-weight="700">Public contract</text>
    <text x="60" y="92" fill="currentColor" font-size="15">bit_block, bit_view, const_bit_view, free algorithms, stable include seam</text>

    <rect x="86" y="142" width="250" height="72" rx="18" data-fill="accent" />
    <text x="118" y="181" fill="currentColor" font-size="22" font-weight="650">Whitepaper</text>
    <text x="118" y="205" fill="currentColor" font-size="14">Design thesis, layering, dispatch boundaries</text>

    <rect x="384" y="142" width="250" height="72" rx="18" data-fill="surface" />
    <text x="416" y="181" fill="currentColor" font-size="22" font-weight="650">Performance</text>
    <text x="416" y="205" fill="currentColor" font-size="14">Baseline numbers, limits, methodology notes</text>

    <rect x="86" y="256" width="250" height="72" rx="18" data-fill="surface" />
    <text x="118" y="295" fill="currentColor" font-size="22" font-weight="650">Reference</text>
    <text x="118" y="319" fill="currentColor" font-size="14">Types, views, algorithm-level contract</text>

    <rect x="384" y="256" width="250" height="72" rx="18" data-fill="surface" />
    <text x="416" y="295" fill="currentColor" font-size="22" font-weight="650">Research</text>
    <text x="416" y="319" fill="currentColor" font-size="14">Comparisons, sources, evolution notes</text>

    <path d="M360 92 L360 128" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M360 214 L360 242" stroke-width="2.5" fill="none" data-stroke="muted" />
    <path d="M210 214 L210 242" stroke-width="2.5" fill="none" data-stroke="muted" />
    <path d="M510 214 L510 242" stroke-width="2.5" fill="none" data-stroke="muted" />
  </svg>
</FigureFrame>

## Read in this order

<ReadingPathGrid
  :items="[
    {
      title: 'Whitepaper',
      href: '/en/whitepaper/index',
      summary: 'Read the public model, algorithm posture, and dispatch boundaries first.',
      detail: 'This is the site thesis, not an accessory narrative.'
    },
    {
      title: 'Performance',
      href: '/en/performance/index',
      summary: 'Interpret current benchmark numbers through retained methodology and support scope.',
      detail: 'BitCal prefers honest baselines over broad unverified claims.'
    },
    {
      title: 'Reference',
      href: '/en/reference/index',
      summary: 'Confirm the contract surface after the architectural story is clear.',
      detail: 'Use this section for types, views, and algorithms, not marketing language.'
    },
    {
      title: 'Research',
      href: '/en/research/index',
      summary: 'Compare BitCal against external references, adjacent libraries, and design alternatives.',
      detail: 'This is where context and evolution become explicit.'
    }
  ]"
/>

## What the site is actually claiming

<EvidenceStrip
  :items="[
    { label: 'Stable public include seam', value: '&lt;bitcal/bitcal.hpp&gt;' },
    { label: 'Primary support posture', value: 'C++23 + x86-64 first' },
    { label: 'Documentation attitude', value: 'Proof over hype' },
    { label: 'Benchmark posture', value: 'Baseline before bravado' }
  ]"
/>

## Architectural through-line

<FigureFrame title="Contract and implementation boundary" caption="The library can evolve its kernel layer without turning backend choices into its public identity.">
  <svg viewBox="0 0 720 260" role="img" aria-label="BitCal contract and implementation boundary">
    <rect x="28" y="24" width="664" height="44" rx="14" data-fill="surface" />
    <text x="56" y="52" fill="currentColor" font-size="21" font-weight="680">Stable include seam</text>
    <text x="250" y="52" fill="currentColor" font-size="18">&lt;bitcal/bitcal.hpp&gt;</text>

    <rect x="28" y="92" width="664" height="52" rx="14" data-fill="accent" />
    <text x="56" y="123" fill="currentColor" font-size="22" font-weight="680">Public model</text>
    <text x="208" y="123" fill="currentColor" font-size="16">bit_block, bit_view, const_bit_view, free algorithms</text>

    <rect x="28" y="168" width="664" height="52" rx="14" data-fill="surface" />
    <text x="56" y="199" fill="currentColor" font-size="22" font-weight="680">Detail layer</text>
    <text x="208" y="199" fill="currentColor" font-size="16">dispatch heuristics, x86-64 kernels, scalar fallback, retained benchmark path</text>

    <path d="M360 68 L360 90" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M360 144 L360 166" stroke-width="2.5" fill="none" data-stroke="primary" />
  </svg>
</FigureFrame>

## Research footing

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      note: 'Authoritative reference when discussing x86 SIMD intrinsics and instruction-level constraints.'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      note: 'Useful for latency, throughput, and dispatch trade-off interpretation on x86-64.'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      note: 'A strong background reference for broadword and word-parallel bit techniques.'
    }
  ]"
/>
