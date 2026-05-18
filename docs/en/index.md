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
    { text: 'Read the whitepaper', href: '/en/whitepaper/index', theme: 'brand' },
    { text: 'Inspect the evidence', href: '/en/whitepaper/performance', theme: 'alt' },
    { text: 'View the repository', href: 'https://github.com/LessUp/bitcal', theme: 'alt' }
  ]"
>
</BitcalHero>

<FigureFrame title="Reading model" caption="BitCal leads with public contract, then proves itself through evidence, reference, and comparative context.">
  <ReadingModelDiagram lang="en" />
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
      href: '/en/whitepaper/performance',
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
  <ArchitectureDiagram lang="en" />
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
