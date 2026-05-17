# BitCal Pages Whitepaper Reconstruction Design

## Summary

BitCal will keep the current VitePress 1.5 and GitHub Pages deployment skeleton, but everything above that foundation will be rebuilt around a different goal: a production-grade **technical whitepaper site** for senior developers evaluating the library's architecture, algorithm model, performance posture, and engineering credibility.

This redesign is intentionally incompatible with the current docs taxonomy, page tone, and visual language. The new site will privilege **thesis-quality narrative, evidence-first layout, theme-safe diagrams, and research-style references** over conventional product-doc patterns.

## Assumptions

Because the user is offline during this design pass, this document locks the following assumptions so implementation can proceed autonomously:

1. The existing VitePress stack is retained because the current foundation is already modern and proven.
2. The current guide / architecture / API split is not preserved for compatibility.
3. English and Chinese will keep mirrored top-level navigation and critical whitepaper content.
4. Public narrative will align with the active vNext direction instead of preserving the older `bitarray` era messaging.
5. Visual polish is not decorative work; it is part of the technical reading experience and must be implemented at the theme-system level.

## Problem Statement

The current GitHub Pages implementation has three structural weaknesses:

1. **Narrative drift**: core pages still mix older BitCal positioning with the active C++23, x86-64-first, `bit_block` / `bit_view` / free-algorithm direction.
2. **Weak information architecture**: the current site still feels like a traditional feature-and-reference docs portal instead of a guided whitepaper for advanced readers.
3. **Insufficient visual system discipline**: theme handling, SVG contrast, diagrams, and landing-page composition are not yet operating as one coherent design system.

The result is technically serviceable but not category-defining.

## Goals

1. Reframe the site as a **systems-and-algorithms whitepaper** instead of a generic documentation portal.
2. Rebuild the reading path so advanced developers can move from thesis to architecture to evidence to API contract without friction.
3. Replace the current CSS-only polish with a reusable VitePress theme layer and shared components.
4. Eliminate light/dark SVG readability failures by introducing a figure system that is theme-aware by construction.
5. Add research-grade sections for references, related systems, design trade-offs, and evolution notes.
6. Remove stale platform, compatibility, and API claims that are weaker than the retained verification path.

## Non-Goals

1. Preserve the current docs directory structure for compatibility.
2. Maintain low-value mirrored content that duplicates the same deep material in multiple formats.
3. Turn the site into a beginner tutorial portal.
4. Expand product claims beyond what the retained benchmark and test posture can support.

## Current-State Findings

### Foundation strengths

- VitePress 1.5 is already in place.
- Mermaid integration already exists.
- The current site already builds through `docs/.vitepress/dist`.
- Locale routing and local search are already available.

These are good enough and should not be replaced.

### Foundation weaknesses

- `docs/.vitepress/theme/index.ts` only re-exports the default theme, so the site has no real component layer.
- `docs/.vitepress/theme/style.css` is mostly a monolithic override sheet with homepage-specific classes mixed into global theme styling.
- Current home pages (`docs/en/index.md`, `docs/zh/index.md`) are still feature-card landing pages, not whitepaper gateways.
- The current platform and API pages still retain legacy-era claims inconsistent with the active vNext posture.
- The current architecture pages are long-form prose without a strong figure grammar or evidence framing.

## Approaches Considered

### Approach A — Visual reskin over the existing information architecture

Keep the current page tree and mostly rewrite CSS, hero copy, and diagrams.

**Pros**

- Fastest path
- Lowest content churn

**Cons**

- Preserves the wrong reading model
- Cannot fully remove narrative drift
- Leaves homepage and sidebars structurally conventional

### Approach B — Thesis-driven docs atlas on the existing VitePress core (**recommended**)

Keep the VitePress deployment skeleton, but rebuild navigation, sidebars, page taxonomy, components, and key content domains around a whitepaper-style reading flow.

**Pros**

- Maximizes reader clarity without incurring framework churn
- Supports aggressive visual and IA redesign
- Keeps content in canonical Markdown while still allowing high-value presentation components
- Fits repository policy better than a custom app shell

**Cons**

- Requires broad content movement and rewrite
- Needs a real component system rather than CSS-only overrides

### Approach C — Single long-form monograph with minimal navigation

Collapse most content into one giant whitepaper and keep only a tiny reference appendix.

**Pros**

- Strong academic identity
- Highly opinionated reading path

**Cons**

- Poor skimmability
- Weak discoverability for API and operational content
- Harder to maintain bilingually

### Recommendation

Choose **Approach B**. It is aggressive enough to deliver the intended "technical whitepaper" effect, but still operationally sound for GitHub Pages, VitePress, bilingual maintenance, and canonical Markdown governance.

## Target Experience

The redesigned site should feel like a cross between:

- an architecture whitepaper
- a systems handbook
- a benchmark methodology note
- a compact API reference

It should not feel like marketing, a generic starter-docs template, or a raw README mirror.

## Information Architecture

### Top-level navigation

The site will move to the following domain model in both locales:

1. **Guide**
2. **Whitepaper**
3. **Performance**
4. **Reference**
5. **Research**
6. **Status**

### Reading model

#### Guide

Minimal onboarding material only:

- installation
- quick start
- verification path
- migration posture

#### Whitepaper

The main architecture narrative:

- system thesis
- public model
- algorithm surface
- internal dispatch and layering
- support posture
- reproducibility and limitations

#### Performance

Evidence-oriented material:

- benchmark baseline
- methodology
- interpretation guardrails
- future benchmark roadmap

#### Reference

Contract-oriented material:

- types and ownership model
- views
- algorithm reference
- backend boundary notes where user-visible

#### Research

Academic and comparative material:

- related projects
- reference papers and articles
- evolution notes
- design trade-offs

#### Status

Operational truth:

- project posture
- supported targets
- breaking-change stance
- release and changelog links

## Visual System

### Design principles

1. **Scholarly, not corporate**
2. **Proof-oriented, not slogan-oriented**
3. **Dense where valuable, quiet where decorative**
4. **Theme-safe by default**
5. **Figures are explanatory assets, not ornaments**

### Visual tone

- dark mode should feel like a high-end engineering notebook, not neon marketing
- light mode should feel like a research memo with crisp contrast and generous whitespace
- color should be used for semantic emphasis, depth separation, and data cues rather than saturation

### Component set

The implementation should add reusable theme components for:

- thesis hero
- reading-path grid
- evidence strip
- figure frame
- comparison matrix
- citation list
- metric card
- support-status panel
- aside-style "why this matters" callout

The homepage and major section landings should be built from these components instead of page-local ad hoc HTML.

## Diagram and SVG Policy

### Requirements

1. Every first-class diagram must remain legible in both light and dark themes.
2. Diagrams must use shared semantic tokens instead of hard-coded black text on transparent or dark backgrounds.
3. Mermaid diagrams must inherit an explicit site-level theme variable set.
4. High-value architecture figures should be inline SVG or Vue-rendered figure components so they can respond to CSS variables.
5. One figure source should generate one semantic diagram; no duplicated light/dark assets unless strictly necessary.

### Figure classes

- **Mermaid** for maintainable graphs and flow diagrams
- **Inline SVG** for polished system schematics and algorithm visuals
- **HTML/CSS figure blocks** for callouts, memory layouts, or comparative cards where text must remain selectable and responsive

### Theme-safe figure tokens

The theme layer should expose dedicated tokens for:

- canvas background
- inset background
- primary stroke
- muted stroke
- text strong
- text muted
- positive / caution / accent

This must be separate from generic brand tokens so figures remain coherent even if the accent palette evolves.

## Content Strategy

### Narrative alignment

All primary content must align with the active vNext direction:

- C++23 baseline
- x86-64-first optimization posture
- `bit_block`, `bit_view`, `const_bit_view`, and free algorithms as the public model
- `<bitcal/bitcal.hpp>` as the stable include seam
- reproducible evidence instead of broad unsupported claims

### Mandatory rewrites

The following existing areas require rewrite or aggressive pruning:

- home pages in both locales
- architecture overview
- platform support
- SIMD dispatch
- quick start examples still centered on legacy `bitarray`
- reference pages that document removed or soon-to-be-obsolete public shapes

### New deep-content material

The redesign must add:

1. a public-model page explaining owner/view/algorithm separation
2. an algorithm page introducing the free-algorithm design
3. a performance methodology page that separates evidence from aspiration
4. a research page covering related libraries, papers, or adjacent systems
5. an evolution page explaining why BitCal moved away from the older model

## Academic and Reference Layer

The site should explicitly include a scholarly apparatus:

### Reference blocks

Each major whitepaper page may end with:

- references
- further reading
- related systems
- design notes

### Acceptable reference sources

- upstream CPU / ISA documentation where relevant
- papers or canonical technical articles on bitset/vectorized operations
- comparable open source libraries or runtime systems
- benchmark methodology references where they clarify interpretation

### Tone rule

References are there to increase rigor and context, not to fake academia. Every citation should help the reader reason better about BitCal.

## Theme Architecture

### Structural decision

The current theme layer should evolve from a CSS override file into a small component system:

- extend the default VitePress theme
- register BitCal-specific components globally
- keep global CSS focused on tokens, typography, layout primitives, and component skins

### Expected theme files

The implementation should likely introduce:

- a richer `docs/.vitepress/theme/index.ts`
- one or more Vue components under `docs/.vitepress/theme/components/`
- a restructured stylesheet split or a significantly cleaned single stylesheet

The exact file split can be adjusted during implementation, but the goal is to remove homepage-specific structure from the global override soup.

## Bilingual Strategy

### Scope

Both locales should share:

- identical top-level section structure
- equivalent landing-page affordances
- mirrored whitepaper reading paths

### Translation rule

Literal sentence-by-sentence mirroring is not required. Structural parity is required. Each locale should read naturally while keeping the same technical claims and navigation depth.

## Validation Requirements

The redesign is only acceptable if validation covers:

1. VitePress production build
2. locale routing
3. navigation integrity
4. local search presence
5. Mermaid rendering
6. dark/light visual contrast
7. figure readability in both themes
8. no broken links introduced by taxonomy changes

## Risks and Mitigations

### Risk: content scope explodes

**Mitigation:** prioritize the public reading path first, then trim or defer secondary pages instead of carrying forward weak content.

### Risk: beautiful but unmaintainable figures

**Mitigation:** keep complex figures in shared SVG or Vue components and keep Mermaid for diagrams whose structure is likely to evolve.

### Risk: divergence from repository documentation policy

**Mitigation:** keep deep content canonical in Markdown, use components only for presentation, and update docs policy if the final IA materially changes governance.

### Risk: stale claims survive the rewrite

**Mitigation:** explicitly audit all pages for C++17-era, broad cross-platform, and legacy-API wording before publication.

## Implementation Workstreams

1. rewrite docs IA and VitePress config
2. replace the theme layer with a component-driven design system
3. rebuild home and section landing pages
4. rewrite whitepaper and performance narratives around vNext
5. add research and citation infrastructure
6. prune or rewrite stale reference and platform pages
7. validate production build and theme behavior

## Acceptance Criteria

This design is satisfied when:

1. the home page reads like a thesis gateway rather than a generic feature grid
2. the site exposes a clear reading path from architecture to evidence to reference
3. legacy messaging no longer dominates any primary page
4. diagrams and SVG figures remain clear in both themes
5. the visual system feels cohesive, premium, and engineer-facing
6. research and citation sections materially deepen the whitepaper value
7. the final Pages experience is recognizably stronger than the current baseline in structure, rigor, and craft
