# Guide

The guide is not a beginner tutorial. It is the orientation layer for advanced readers who need to decide whether BitCal's redesign is coherent enough to keep reading.

## Audience and scope

This section is written for readers who want to evaluate one or more of the following:

- whether the **C++23 baseline** is a real project decision;
- whether the **x86-64-first** posture is documented with honest limits;
- whether the new owner / view / algorithm split improves the public model;
- whether the repository still exposes a stable include seam through `<bitcal/bitcal.hpp>`.

The guide intentionally stops short of full reference detail. It exists to prepare the reader for the deeper whitepaper and evidence sections.

## Reading chain

The canonical path is **Guide → Whitepaper → Performance → Reference → Research → Status**.

<ReadingPathGrid
  :items="[
    {
      title: 'Verification path',
      href: '/en/guide/verification',
      badge: 'Execution',
      summary: 'See the retained build, test, example, and benchmark command path.',
      detail: 'A header-only library still needs a reproducible local validation loop.'
    },
    {
      title: 'Migration posture',
      href: '/en/guide/migration-posture',
      badge: 'Direction',
      summary: 'Understand why BitCal is willing to leave the earlier monolithic public model.',
      detail: 'Migration is explained as a design choice, not hidden behind ambiguous compatibility wording.'
    },
    {
      title: 'Whitepaper',
      href: '/en/whitepaper/',
      badge: 'Architecture',
      summary: 'Move into the system architecture spine once the execution posture is clear.',
      detail: 'This is where the owner / view / algorithm model and dispatch boundary are defined.'
    },
    {
      title: 'Performance',
      href: '/en/performance/',
      badge: 'Evidence',
      summary: 'Inspect the baseline snapshot and methodology as separate artifacts.',
      detail: 'Benchmark numbers are only meaningful when scope and reproduction stay attached.'
    },
    {
      title: 'Reference',
      href: '/en/reference/',
      badge: 'Contract',
      summary: 'Read types, views, and algorithms as contractual roles, not as a source tour.',
      detail: 'Reference comes after architecture because semantics depend on the role model.'
    },
    {
      title: 'Research and Status',
      href: '/en/research/',
      badge: 'Context',
      summary: 'Use research for comparisons and status for boundaries before drawing conclusions.',
      detail: 'Research widens context; Status narrows the final claims again.'
    }
  ]"
/>

## What you should know before continuing

- BitCal is **not** presenting itself as a settled general-purpose bitset library.
- The redesign is **v4.0.0-scale** and is willing to break earlier expectations.
- Current Pages content is organized to help you review architecture and evidence, not to maximize feature-count impression.

Continue with the [verification path](/en/guide/verification) if you want to anchor the documentation in executable evidence first, or go straight to the [whitepaper](/en/whitepaper/) if you already know the repository context.
