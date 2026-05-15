---
layout: home
hero:
  name: BitCal vNext
  text: Header-only C++23 bit library
  tagline: Rebuilt around block/view separation, x86-64-first validation, and observable algorithm contracts.
  actions:
    - theme: brand
      text: Read the whitepaper
      link: ./architecture/vnext-whitepaper
    - theme: alt
      text: Project status
      link: ./project-status/
    - theme: alt
      text: 中文首页
      link: ../zh/
---

<script setup>
import { withBase } from 'vitepress'
</script>

## Start with the path that matches your job

<ReadingPathGrid
  :items="[
    {
      title: 'Interviewers',
      href: './architecture/overview',
      badge: 'Fast brief',
      summary: 'Use the retained architecture overview to frame what BitCal was, then jump into the whitepaper and status pages for the vNext thesis.'
    },
    {
      title: 'Systems developers',
      href: './academy/overview',
      badge: 'Deep model',
      summary: 'Start with the Academy route to understand block/view separation, dispatch boundaries, and how performance claims are kept observable.'
    },
    {
      title: 'Adopters',
      href: './getting-started/installation',
      badge: 'Adoption',
      summary: 'Go straight to installation if you need the include seam, migration posture, and the minimum route into the maintained guides.'
    }
  ]"
/>

## Evidence snapshot

<FigureFrame
  eyebrow="Architecture evidence"
  title="The homepage now matches the BitCal vNext layering thesis."
  caption="BitCal vNext separates block and view responsibilities, keeps algorithms visible at the contract layer, and ties performance claims back to an x86-64-first validation path."
>

<img :src="withBase('/figures/bitcal-vnext-layers.svg')" alt="BitCal vNext layers diagram">

</FigureFrame>

## Why this site exists

This site is the retained reading layer for BitCal vNext. It explains the redesign thesis, shows where the evidence lives, and routes readers into the maintained sections instead of leaving the project split across legacy-era pages.

- [Academy](./academy/overview) teaches the mental model.
- [Whitepaper](./architecture/vnext-whitepaper) states the architecture thesis and validation posture.
- [Guides](./getting-started/installation) handle installation, quick starts, and migration.
- [Reference](./api/overview) defines the public surface and contracts.
- [Research](./research/overview) collects comparisons, citations, and methodology notes.
- [Project Status](./project-status/) states support boundaries and maintenance reality.
