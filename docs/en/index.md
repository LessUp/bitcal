---
layout: home
title: BitCal
---

<BitcalHero
  eyebrow="Systems architecture notes, not product copy"
  title="Read BitCal like a redesign review, not like a landing page."
  lead="BitCal documents its vNext work as a technical whitepaper for advanced C++ readers: public model, algorithm organization, dispatch boundary, performance methodology, contract reference, and research context presented as one chain of evidence."
  :stats="[
    { label: 'Delivery', value: 'Header-only' },
    { label: 'Stable seam', value: '<bitcal/bitcal.hpp>' },
    { label: 'Baseline', value: 'C++23 · x86-64 first' }
  ]"
  :actions="[
    { text: 'Open the guide', href: '/en/guide/index', theme: 'brand' },
    { text: 'Read the whitepaper', href: '/en/whitepaper/index', theme: 'alt' },
    { text: 'Inspect performance evidence', href: '/en/performance/index', theme: 'alt' }
  ]"
>
</BitcalHero>

## Whitepaper reading contract

This site is for readers who want to answer six concrete questions before trusting a low-level library:

1. **What is the public model?** BitCal is converging on `bit_block<Bits>`, `bit_view`, `const_bit_view`, and free algorithms.
2. **What remains stable?** `<bitcal/bitcal.hpp>` is the only stable include seam.
3. **Where does implementation freedom begin?** Dispatch and kernels live below the public contract.
4. **How much performance is actually proven?** Only the retained baseline and methodology are claimed.
5. **What can I treat as reference material?** Contract-oriented pages explain roles and semantics, not every internal header.
6. **Which external work informs the design?** Research pages cite the instruction references, papers, and adjacent systems that shape the posture.

<ReadingPathGrid
  :items="[
    {
      title: 'Guide',
      href: '/en/guide/index',
      badge: '1 · Orientation',
      tone: 'primary',
      summary: 'Start with audience, verification path, and migration posture.',
      detail: 'This is the shortest route to understanding how BitCal expects to be reviewed.'
    },
    {
      title: 'Whitepaper',
      href: '/en/whitepaper/index',
      badge: '2 · Architecture',
      tone: 'whitepaper',
      summary: 'Follow the system architecture spine from public model to dispatch boundary.',
      detail: 'This is where owner / view / algorithm, free-algorithm organization, and kernel boundaries are made explicit.'
    },
    {
      title: 'Performance',
      href: '/en/performance/index',
      badge: '3 · Evidence',
      summary: 'Read the retained benchmark baseline separately from the measurement methodology.',
      detail: 'Numbers stay attached to reproduction commands, active backend context, and claim limits.'
    },
    {
      title: 'Reference',
      href: '/en/reference/index',
      badge: '4 · Contract',
      summary: 'Confirm the public role model and algorithm contract after the architecture story is clear.',
      detail: 'Reference pages explain what readers may rely on, not how every kernel happens to be written today.'
    },
    {
      title: 'Research',
      href: '/en/research/index',
      badge: '5 · Context',
      summary: 'Review citations, related systems, evolution notes, and design trade-offs.',
      detail: 'This section exists to deepen technical judgment, not to add decorative academic tone.'
    },
    {
      title: 'Status',
      href: '/en/status/index',
      badge: '6 · Boundary',
      summary: 'End with release posture, support matrix, and documentation truth sources.',
      detail: 'Status narrows every claim back down to what the repository can currently defend.'
    }
  ]"
/>

## Public model at a glance

<EvidenceStrip
  :items="[
    { label: 'Owning role', value: 'bit_block<Bits>', tone: 'accent' },
    { label: 'Borrowing role', value: 'bit_view / const_bit_view' },
    { label: 'Behavioral center', value: 'Free algorithms' },
    { label: 'Implementation boundary', value: 'Dispatch + kernel layer', note: 'Not part of the stable user identity.' }
  ]"
/>

The home page does not attempt to replace the deeper sections. It gives a map:

- the [Guide](/en/guide/index) tells you how to read the project now;
- the [Whitepaper](/en/whitepaper/index) states the architecture thesis;
- the [Performance](/en/performance/index) section separates baseline from method;
- the [Reference](/en/reference/index) pages define the contract surface;
- the [Research](/en/research/index) pages show nearby systems and source material;
- the [Status](/en/status/index) page defines the release and support boundary.

## Evidence posture

<FigureFrame title="Why the reading order matters" caption="BitCal expects readers to move from architecture, to evidence, to contract, to context, to status boundaries.">
  <ArchitectureDiagram lang="en" />
</FigureFrame>

BitCal prefers a narrower but more defensible story:

- **C++23 baseline** is a design decision, not a future aspiration.
- **x86-64-first posture** means optimization and benchmark interpretation are concentrated where the project retains active evidence.
- **Header-only delivery** does not remove the need for explicit verification, benchmark discipline, or support limits.
- **Whitepaper-first documentation** means architecture, methodology, and references stay connected instead of being split between marketing and source comments.

## Research footing

<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html',
      meta: 'Instruction reference',
      note: 'Grounds x86 SIMD discussion in an authoritative instruction-level source.'
    },
    {
      title: 'Agner Fog optimization manuals',
      href: 'https://www.agner.org/optimize/',
      meta: 'Microarchitecture manuals',
      note: 'Useful when explaining why latency, throughput, and dispatch policy matter to low-level claims.'
    },
    {
      title: 'Sebastiano Vigna, Broadword Implementation of Rank/Select Queries',
      href: 'https://arxiv.org/abs/0901.0166',
      meta: 'Broadword paper',
      note: 'Provides a conceptual bridge between bit-level semantics and word-parallel implementation techniques.'
    }
  ]"
/>
