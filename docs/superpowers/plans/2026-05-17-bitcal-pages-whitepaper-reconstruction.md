# BitCal Pages Whitepaper Reconstruction Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Rebuild the BitCal GitHub Pages site into a whitepaper-grade, theme-safe, bilingual documentation system centered on architecture, algorithms, evidence, and references.

**Architecture:** Keep VitePress 1.5 and the current GitHub Pages deployment path, but replace the current guide-centric taxonomy and CSS-only theme overrides with a thesis-driven information architecture, reusable theme components, and rewritten whitepaper/performance/reference content. The site will use Markdown as canonical content, Vue theme components for high-value presentation, and shared CSS tokens so diagrams, SVG figures, and landing surfaces remain legible in both light and dark themes.

**Tech Stack:** VitePress 1.5, Vue 3 theme components, Markdown, Mermaid, CSS custom properties, npm scripts (`npm run build`)

---

## File Structure Lock

### Core theme and site shell

- Modify: `docs/.vitepress/config.ts`
- Modify: `docs/.vitepress/theme/index.ts`
- Modify: `docs/.vitepress/theme/style.css`
- Create: `docs/.vitepress/theme/components/BitcalHero.vue`
- Create: `docs/.vitepress/theme/components/ReadingPathGrid.vue`
- Create: `docs/.vitepress/theme/components/EvidenceStrip.vue`
- Create: `docs/.vitepress/theme/components/FigureFrame.vue`
- Create: `docs/.vitepress/theme/components/CitationList.vue`

### English docs

- Modify: `docs/en/index.md`
- Create: `docs/en/guide/index.md`
- Create: `docs/en/guide/verification.md`
- Create: `docs/en/guide/migration-posture.md`
- Create: `docs/en/whitepaper/index.md`
- Create: `docs/en/whitepaper/public-model.md`
- Create: `docs/en/whitepaper/algorithm-design.md`
- Create: `docs/en/whitepaper/dispatch-and-kernels.md`
- Create: `docs/en/performance/index.md`
- Create: `docs/en/performance/methodology.md`
- Create: `docs/en/reference/index.md`
- Create: `docs/en/reference/types-and-views.md`
- Create: `docs/en/reference/algorithms.md`
- Create: `docs/en/research/index.md`
- Create: `docs/en/research/evolution-and-comparisons.md`
- Create: `docs/en/status/index.md`

### Chinese docs

- Modify: `docs/zh/index.md`
- Create: `docs/zh/guide/index.md`
- Create: `docs/zh/guide/verification.md`
- Create: `docs/zh/guide/migration-posture.md`
- Create: `docs/zh/whitepaper/index.md`
- Create: `docs/zh/whitepaper/public-model.md`
- Create: `docs/zh/whitepaper/algorithm-design.md`
- Create: `docs/zh/whitepaper/dispatch-and-kernels.md`
- Create: `docs/zh/performance/index.md`
- Create: `docs/zh/performance/methodology.md`
- Create: `docs/zh/reference/index.md`
- Create: `docs/zh/reference/types-and-views.md`
- Create: `docs/zh/reference/algorithms.md`
- Create: `docs/zh/research/index.md`
- Create: `docs/zh/research/evolution-and-comparisons.md`
- Create: `docs/zh/status/index.md`

### Policy and design artifacts

- Modify: `docs/README.md`
- Modify: `docs/superpowers/specs/2026-05-15-bitcal-pages-whitepaper-design.md`
- Test: `cd docs && npm run build`
- Test: `git diff --check`

---

### Task 1: Rebuild the site map and route model

**Files:**
- Modify: `docs/.vitepress/config.ts`
- Modify: `docs/README.md`
- Create: `docs/en/guide/index.md`
- Create: `docs/en/whitepaper/index.md`
- Create: `docs/en/performance/index.md`
- Create: `docs/en/reference/index.md`
- Create: `docs/en/research/index.md`
- Create: `docs/en/status/index.md`
- Create: `docs/zh/guide/index.md`
- Create: `docs/zh/whitepaper/index.md`
- Create: `docs/zh/performance/index.md`
- Create: `docs/zh/reference/index.md`
- Create: `docs/zh/research/index.md`
- Create: `docs/zh/status/index.md`
- Test: `cd docs && npm run build`

- [ ] **Step 1: Capture the baseline route model and current docs build**

```bash
cd docs && npm run build
```

Expected: VitePress build succeeds and produces `docs/.vitepress/dist`.

- [ ] **Step 2: Rewrite locale nav and sidebar groups in `docs/.vitepress/config.ts`**

```ts
nav: [
  { text: 'Guide', link: '/en/guide/index', activeMatch: '/en/guide/' },
  { text: 'Whitepaper', link: '/en/whitepaper/index', activeMatch: '/en/whitepaper/' },
  { text: 'Performance', link: '/en/performance/index', activeMatch: '/en/performance/' },
  { text: 'Reference', link: '/en/reference/index', activeMatch: '/en/reference/' },
  { text: 'Research', link: '/en/research/index', activeMatch: '/en/research/' },
  { text: 'Status', link: '/en/status/index', activeMatch: '/en/status/' },
]
```

Repeat the same structure for `/zh/` with translated labels.

- [ ] **Step 3: Create section landing pages with minimal frontmatter and section summaries**

```md
---
title: Whitepaper
---

# Whitepaper

This section is the BitCal vNext thesis: public model, algorithm posture, dispatch structure, and support boundaries.
```

Mirror the same pattern for `guide`, `performance`, `reference`, `research`, and `status` in both locales.

- [ ] **Step 4: Update `docs/README.md` so the canonical IA matches the new domain model**

```md
### 3.3 Whitepaper

- Public model
- Algorithm design
- Dispatch and kernel boundaries
- Reproducibility and limits
```

Also replace old "Architecture / API" wording where it no longer matches the new domain split.

- [ ] **Step 5: Re-run the docs build**

```bash
cd docs && npm run build
```

Expected: PASS. Sidebars and routes resolve without dead-link or missing-page failures.

- [ ] **Step 6: Commit the routing and IA skeleton**

```bash
git add docs/.vitepress/config.ts docs/README.md docs/en docs/zh
git commit -m "docs: rebuild pages information architecture"
```

---

### Task 2: Replace the theme override soup with a reusable component system

**Files:**
- Modify: `docs/.vitepress/theme/index.ts`
- Modify: `docs/.vitepress/theme/style.css`
- Create: `docs/.vitepress/theme/components/BitcalHero.vue`
- Create: `docs/.vitepress/theme/components/ReadingPathGrid.vue`
- Create: `docs/.vitepress/theme/components/EvidenceStrip.vue`
- Create: `docs/.vitepress/theme/components/FigureFrame.vue`
- Create: `docs/.vitepress/theme/components/CitationList.vue`
- Test: `cd docs && npm run build`

- [ ] **Step 1: Register global theme components in `docs/.vitepress/theme/index.ts`**

```ts
import DefaultTheme from 'vitepress/theme'
import type { Theme } from 'vitepress'
import './style.css'

import BitcalHero from './components/BitcalHero.vue'
import CitationList from './components/CitationList.vue'
import EvidenceStrip from './components/EvidenceStrip.vue'
import FigureFrame from './components/FigureFrame.vue'
import ReadingPathGrid from './components/ReadingPathGrid.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('BitcalHero', BitcalHero)
    app.component('CitationList', CitationList)
    app.component('EvidenceStrip', EvidenceStrip)
    app.component('FigureFrame', FigureFrame)
    app.component('ReadingPathGrid', ReadingPathGrid)
  },
} satisfies Theme
```

- [ ] **Step 2: Add a dedicated design-token layer in `docs/.vitepress/theme/style.css`**

```css
:root {
  --bitcal-hero-bg: linear-gradient(180deg, #f7fbff 0%, #ffffff 100%);
  --bitcal-surface-1: rgba(255, 255, 255, 0.82);
  --bitcal-surface-2: #eef4fb;
  --bitcal-figure-stroke: #33506f;
  --bitcal-figure-muted: #6d86a0;
  --bitcal-figure-text: #16324d;
}

.dark {
  --bitcal-hero-bg: linear-gradient(180deg, #0b1320 0%, #0f1724 100%);
  --bitcal-surface-1: rgba(12, 18, 30, 0.82);
  --bitcal-surface-2: #111d2e;
  --bitcal-figure-stroke: #82a7d0;
  --bitcal-figure-muted: #5b7593;
  --bitcal-figure-text: #e4edf8;
}
```

- [ ] **Step 3: Create the hero and section primitives**

```vue
<script setup lang="ts">
defineProps<{
  eyebrow: string
  title: string
  lead: string
  stats: { label: string; value: string }[]
}>()
</script>

<template>
  <section class="bitcal-hero">
    <p class="bitcal-hero__eyebrow">{{ eyebrow }}</p>
    <h1>{{ title }}</h1>
    <p class="bitcal-hero__lead">{{ lead }}</p>
    <div class="bitcal-hero__stats">
      <div v-for="stat in stats" :key="stat.label" class="bitcal-stat">
        <span>{{ stat.label }}</span>
        <strong>{{ stat.value }}</strong>
      </div>
    </div>
  </section>
</template>
```

Add matching Vue single-file components for the reading-path grid, evidence strip, figure frame, and citations.

- [ ] **Step 4: Move homepage- and figure-specific styling out of the old ad hoc classes**

```css
.bitcal-figure {
  border: 1px solid var(--vp-c-border);
  background: var(--bitcal-surface-1);
  box-shadow: 0 20px 60px rgba(6, 17, 30, 0.08);
}

.bitcal-figure svg {
  color: var(--bitcal-figure-text);
}

.bitcal-figure [data-stroke='primary'] {
  stroke: var(--bitcal-figure-stroke);
}
```

- [ ] **Step 5: Re-run the docs build**

```bash
cd docs && npm run build
```

Expected: PASS. VitePress resolves the new Vue components and CSS without import errors.

- [ ] **Step 6: Commit the theme system rewrite**

```bash
git add docs/.vitepress/theme
git commit -m "docs: add component-driven pages theme"
```

---

### Task 3: Rebuild the English homepage and the top-level landing surfaces

**Files:**
- Modify: `docs/en/index.md`
- Create: `docs/en/guide/index.md`
- Create: `docs/en/whitepaper/index.md`
- Create: `docs/en/performance/index.md`
- Create: `docs/en/reference/index.md`
- Create: `docs/en/research/index.md`
- Create: `docs/en/status/index.md`
- Test: `cd docs && npm run build`

- [ ] **Step 1: Replace the English homepage feature grid with the thesis gateway**

```md
---
title: BitCal
---

<BitcalHero
  eyebrow="Systems-grade bit computation"
  title="BitCal vNext is a whitepaper-first SIMD bit library."
  lead="Read the public model, dispatch architecture, performance posture, and support boundaries the way you would evaluate an engineering design review."
  :stats="[
    { label: 'Delivery', value: 'Header-only' },
    { label: 'Language', value: 'C++23' },
    { label: 'Priority', value: 'x86-64 first' }
  ]"
/>
```

- [ ] **Step 2: Add a guided reading path and evidence strip to `docs/en/index.md`**

```md
<ReadingPathGrid
  :items="[
    { title: 'Whitepaper', href: '/en/whitepaper/index', summary: 'Architecture thesis, public model, algorithm posture.' },
    { title: 'Performance', href: '/en/performance/index', summary: 'Baseline numbers, methodology, interpretation guardrails.' },
    { title: 'Reference', href: '/en/reference/index', summary: 'Types, views, algorithms, and contract details.' }
  ]"
/>

<EvidenceStrip
  :items="[
    { label: 'Stable include seam', value: '<bitcal/bitcal.hpp>' },
    { label: 'Primary support posture', value: 'C++23 + x86-64' },
    { label: 'Narrative style', value: 'Proof over hype' }
  ]"
/>
```

- [ ] **Step 3: Turn each English section landing page into a real reading entrance**

```md
# Performance

Measure first, claim second. This section records the retained benchmark baseline, methodology constraints, and the difference between local evidence and public claims.

## Reading order

1. Baseline
2. Methodology
3. Interpretation guardrails
```

Do the same for `guide`, `whitepaper`, `reference`, `research`, and `status`.

- [ ] **Step 4: Re-run the docs build**

```bash
cd docs && npm run build
```

Expected: PASS. New landing pages render and the homepage no longer depends on the legacy card classes.

- [ ] **Step 5: Commit the English landing-page rewrite**

```bash
git add docs/en
git commit -m "docs: rebuild english pages landing surfaces"
```

---

### Task 4: Rewrite the English whitepaper, performance, reference, and research content

**Files:**
- Create: `docs/en/guide/verification.md`
- Create: `docs/en/guide/migration-posture.md`
- Create: `docs/en/whitepaper/public-model.md`
- Create: `docs/en/whitepaper/algorithm-design.md`
- Create: `docs/en/whitepaper/dispatch-and-kernels.md`
- Create: `docs/en/performance/methodology.md`
- Create: `docs/en/reference/types-and-views.md`
- Create: `docs/en/reference/algorithms.md`
- Create: `docs/en/research/evolution-and-comparisons.md`
- Modify: `docs/en/architecture/vnext-whitepaper.md`
- Modify: `docs/en/architecture/performance-baseline.md`
- Modify: `docs/en/architecture/platform-support.md`
- Test: `cd docs && npm run build`

- [ ] **Step 1: Move the old English whitepaper content into the new whitepaper chapter set**

```md
# Public Model

BitCal vNext splits its public model into three contracts:

1. `bit_block<Bits>` owns storage
2. `bit_view` / `const_bit_view` borrow storage
3. free algorithms operate on those surfaces without making ISA tags part of the public API
```

- [ ] **Step 2: Add an algorithm-design page that explains why free algorithms are the center**

```md
# Algorithm Design

The library is not presenting one monolithic class as the center of the API. It presents a fixed-width storage contract plus algorithms that are easier to optimize, test, and compose.
```

Include a small figure or Mermaid block showing storage, views, and algorithm flow.

- [ ] **Step 3: Add a dispatch-and-kernels page with a theme-safe figure**

```md
<FigureFrame title="Dispatch boundary" caption="Public contracts stay stable while x86-64 kernels evolve behind the detail layer.">
  <svg viewBox="0 0 720 220" role="img" aria-label="BitCal dispatch layers">
    <rect x="20" y="20" width="680" height="44" rx="12" fill="var(--bitcal-surface-2)" />
    <text x="48" y="48" fill="currentColor">Public API: bit_block, bit_view, algorithms</text>
  </svg>
</FigureFrame>
```

- [ ] **Step 4: Rewrite the performance pages so claims and limits are explicit**

```md
## Interpretation guardrails

- local smoke numbers are not cross-platform claims
- AVX2-first measurements are not evidence for every backend
- methodology quality matters more than isolated low-latency numbers
```

- [ ] **Step 5: Rewrite the reference pages around the vNext public model**

```md
## Stable include seam

All user-facing examples should include:

```cpp
#include <bitcal/bitcal.hpp>
```
```

Also remove or downplay stale `bitarray`-centric framing where it conflicts with vNext.

- [ ] **Step 6: Add research and references**

```md
<CitationList
  :items="[
    {
      title: 'Agner Fog instruction tables',
      href: 'https://www.agner.org/optimize/',
      note: 'Useful when interpreting x86 throughput and latency trade-offs.'
    }
  ]"
/>
```

Use the research page to compare BitCal with adjacent bitset or SIMD-oriented libraries without overstating equivalence.

- [ ] **Step 7: Re-run the docs build**

```bash
cd docs && npm run build
```

Expected: PASS. Whitepaper, performance, and reference pages render with the new components and no broken links.

- [ ] **Step 8: Commit the English deep-content rewrite**

```bash
git add docs/en
git commit -m "docs: rewrite english whitepaper content"
```

---

### Task 5: Mirror the new whitepaper system in Chinese

**Files:**
- Modify: `docs/zh/index.md`
- Create: `docs/zh/guide/verification.md`
- Create: `docs/zh/guide/migration-posture.md`
- Create: `docs/zh/whitepaper/public-model.md`
- Create: `docs/zh/whitepaper/algorithm-design.md`
- Create: `docs/zh/whitepaper/dispatch-and-kernels.md`
- Create: `docs/zh/performance/methodology.md`
- Create: `docs/zh/reference/types-and-views.md`
- Create: `docs/zh/reference/algorithms.md`
- Create: `docs/zh/research/evolution-and-comparisons.md`
- Modify: `docs/zh/architecture/vnext-whitepaper.md`
- Modify: `docs/zh/architecture/platform-support.md`
- Test: `cd docs && npm run build`

- [ ] **Step 1: Rebuild the Chinese homepage with the same components and reading path**

```md
<BitcalHero
  eyebrow="系统级位计算"
  title="BitCal vNext 是面向高级开发者的技术白皮书站点。"
  lead="从公开模型、分发架构、性能证据到参考资料，按设计评审的方式理解这个库。"
  :stats="[
    { label: '交付方式', value: 'Header-only' },
    { label: '语言基线', value: 'C++23' },
    { label: '优化重点', value: 'x86-64 优先' }
  ]"
/>
```

- [ ] **Step 2: Mirror the top-level Chinese landing pages**

```md
# 白皮书

本节给出 BitCal vNext 的系统设计主线：公开模型、算法组织、内部分发和支持边界。
```

- [ ] **Step 3: Rewrite the Chinese deep pages for natural language parity, not literal translation**

```md
## 公开模型

BitCal vNext 把 API 收敛为三种角色：

1. `bit_block<Bits>` 负责拥有型存储
2. `bit_view` / `const_bit_view` 负责借用型访问
3. 自由算法负责行为表达与优化边界
```

- [ ] **Step 4: Mirror the citation and research system in Chinese**

```md
<CitationList
  :items="[
    {
      title: 'Intel Intrinsics Guide',
      href: 'https://www.intel.com/content/www/us/en/docs/intrinsics-guide/',
      note: '用于解释 x86 SIMD 指令与实现边界。'
    }
  ]"
/>
```

- [ ] **Step 5: Re-run the docs build**

```bash
cd docs && npm run build
```

Expected: PASS. Chinese routes build cleanly and retain structural parity with English.

- [ ] **Step 6: Commit the Chinese rewrite**

```bash
git add docs/zh
git commit -m "docs: mirror whitepaper architecture in chinese"
```

---

### Task 6: Final verification, cleanup, and publication readiness

**Files:**
- Modify: `docs/.vitepress/config.ts`
- Modify: `docs/.vitepress/theme/style.css`
- Modify: `docs/en/**`
- Modify: `docs/zh/**`
- Test: `cd docs && npm run build`
- Test: `git diff --check`

- [ ] **Step 1: Audit for stale legacy claims**

```bash
rg -n "C\\+\\+17|bitarray|AVX-512.*full support|cross-platform" docs/en docs/zh
```

Expected: only deliberate historical or migration mentions remain.

- [ ] **Step 2: Run the production docs build**

```bash
cd docs && npm run build
```

Expected: PASS. No broken imports, no dead routes, no component registration failures.

- [ ] **Step 3: Run whitespace and patch hygiene checks**

```bash
git diff --check
```

Expected: no trailing-whitespace or malformed patch warnings.

- [ ] **Step 4: Manually spot-check theme-safe figures in local preview if needed**

```bash
cd docs && npm run preview -- --host 127.0.0.1 --port 4173
```

Expected: local preview serves successfully for manual light/dark inspection.

- [ ] **Step 5: Commit the final verification pass**

```bash
git add docs
git commit -m "docs: finalize bitcal pages whitepaper reconstruction"
```

---

## Self-Review Notes

### Spec coverage

- IA rebuild: Task 1
- theme-system rewrite and dark/light-safe figures: Task 2
- homepage and UX overhaul: Task 3
- deep whitepaper / performance / reference / research content: Task 4
- bilingual parity: Task 5
- validation and stale-claim audit: Task 6

### Placeholder scan

- No `TBD`, `TODO`, or "similar to Task N" placeholders remain.
- Every task includes exact files and commands.

### Type and naming consistency

- Section names are consistently `guide`, `whitepaper`, `performance`, `reference`, `research`, `status`.
- Theme component names are consistently `BitcalHero`, `ReadingPathGrid`, `EvidenceStrip`, `FigureFrame`, and `CitationList`.
