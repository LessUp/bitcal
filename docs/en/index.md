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
    { text: 'Start with the guide', href: '/en/guide/index', theme: 'brand' },
    { text: 'Review the whitepaper', href: '/en/whitepaper/index', theme: 'alt' },
    { text: 'Check status', href: '/en/status/index', theme: 'alt' }
  ]"
>
</BitcalHero>

<FigureFrame eyebrow="Orientation" title="Reading model" tone="contrast" caption="BitCal leads with guide-level orientation, then proves itself through whitepaper, performance, reference, research, and status evidence.">
  <ReadingModelDiagram lang="en" />
</FigureFrame>

## Read in this order

<ReadingPathGrid
  :items="[
    {
      title: 'Guide',
      href: '/en/guide/index',
      badge: 'Entry',
      tone: 'primary',
      summary: 'Start with the reading path, verification route, and migration posture.',
      detail: 'This is the shortest route from curiosity to serious review.'
    },
    {
      title: 'Whitepaper',
      href: '/en/whitepaper/index',
      badge: 'Core thesis',
      tone: 'whitepaper',
      summary: 'Read the public model, algorithm posture, and dispatch boundaries.',
      detail: 'This is the site thesis, not an accessory narrative.'
    },
    {
      title: 'Performance',
      href: '/en/performance/index',
      badge: 'Evidence',
      summary: 'Interpret current benchmark numbers through retained methodology and claim guardrails.',
      detail: 'BitCal prefers honest baselines over broad unverified claims.'
    },
    {
      title: 'Reference',
      href: '/en/reference/index',
      badge: 'Contract',
      summary: 'Confirm the contract surface after the architectural story is clear.',
      detail: 'Use this section for types, views, and algorithms, not marketing language.'
    },
    {
      title: 'Research',
      href: '/en/research/index',
      badge: 'Context',
      summary: 'Compare BitCal against external references, adjacent libraries, and design alternatives.',
      detail: 'This is where context and evolution become explicit.'
    },
    {
      title: 'Status',
      href: '/en/status/index',
      badge: 'Boundary',
      summary: 'Check support boundaries, breaking-change posture, and maintained sources of truth.',
      detail: 'This is the site section that narrows claims back to what the repo can defend.'
    }
  ]"
/>

## What the site is actually claiming

<EvidenceStrip
  :items="[
    { label: 'Stable public include seam', value: '&lt;bitcal/bitcal.hpp&gt;', tone: 'accent' },
    { label: 'Primary support posture', value: 'C++23 + x86-64 first' },
    { label: 'Documentation attitude', value: 'Proof over hype' },
    { label: 'Benchmark posture', value: 'Baseline before bravado', note: 'Method and scope stay attached to every number.' }
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
      meta: 'Intel · reference',
      note: 'Authoritative reference when discussing x86 SIMD intrinsics and instruction-level constraints.'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: 'Agner Fog · manuals',
      note: 'Useful for latency, throughput, and dispatch trade-off interpretation on x86-64.'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'arXiv · paper',
      note: 'A strong background reference for broadword and word-parallel bit techniques.'
    }
  ]"
/>
