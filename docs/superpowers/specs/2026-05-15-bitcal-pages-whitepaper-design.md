# BitCal GitHub Pages Whitepaper Site Redesign

## Summary

BitCal will rebuild its GitHub Pages experience on top of the existing kimi-cli-style VitePress foundation, but with a new information architecture, visual system, and content model tailored to a C++23, x86-64-first, architecture-centric project.

The target is not a conventional product docs site. It is a bilingual, production-grade **technical whitepaper / architecture showcase / project academy** for advanced developers, interviewers, and contributors.

## Problem Statement

The current Pages stack has already converged on the same core framework choices as `/home/shane/dev/kimi-cli`:

- VitePress 1.5
- Mermaid integration
- LLM documentation plugin
- locale-aware routing
- custom CSS theme layer
- GitHub Pages deployment via `docs/.vitepress/dist`

However, the current site is still a transitional artifact:

- its structure remains close to a conventional guide/reference split
- its homepage is still card-oriented and product-like rather than whitepaper-like
- some narrative still drifts between pre-vNext and vNext positioning
- light/dark image and diagram handling is not governed by a unified visual policy
- the current bilingual policy is weaker than the intended target for this redesign

As a result, the site does not yet deliver a strong technical reading experience for advanced audiences.

## Goals

1. Preserve the mature engineering base already aligned with kimi-cli docs.
2. Rebuild the site into a whitepaper-style reading system for BitCal vNext.
3. Make the homepage function as a project thesis and reading gateway, not just a feature list.
4. Establish a theme-aware diagram and SVG strategy that works in both light and dark modes.
5. Align all public narrative with the active vNext OpenSpec direction.
6. Deliver a near-full bilingual structure with mirrored navigation and core content flow.

## Non-Goals

1. This redesign does not attempt to preserve the current information architecture for compatibility.
2. This redesign does not commit to keeping low-value mirrored pages that violate the canonical-source policy.
3. This redesign does not use kimi-cli's content taxonomy as-is; it only inherits the engineering skeleton and proven interaction patterns.

## Current-State Findings

### Foundation

BitCal docs already use nearly the same underlying framework and deployment pattern as kimi-cli docs. This means the redesign should focus on **structure, content, and systemization**, not on changing the site generator.

### Narrative Drift

Public documentation currently mixes multiple eras of the project:

- C++17 vs C++23
- broad cross-platform retained positioning vs x86-64-first vNext posture
- previous `bitarray`-centric framing vs `bit_block` / `bit_view` / free-algorithm framing

The redesigned site must remove this ambiguity and make vNext the dominant reading path.

### Documentation Architecture Tension

BitCal's documentation policy prefers canonical Markdown and discourages long-lived Markdown/HTML duplication. The redesign must therefore keep deep technical content in canonical Markdown while allowing the homepage and selected landing surfaces to be highly designed within the VitePress presentation layer.

## Design Decisions

### 1. Keep the current VitePress-based deployment model

The site will continue to build and deploy through `docs/.vitepress/dist`, keeping the existing GitHub Pages workflow shape and `VITEPRESS_BASE` behavior.

**Why**

- the current foundation is already stable and aligned with the reference baseline
- it avoids unnecessary framework churn
- it keeps the work focused on user-facing quality and architectural clarity

### 2. Rebuild the information architecture around whitepaper reading flows

The new top-level content domains will be:

- **Home**
- **Academy**
- **Whitepaper**
- **Guides**
- **Reference**
- **Research**
- **Project Status**

These sections will be mirrored across Chinese and English.

**Intent**

- `Home` frames BitCal and routes readers
- `Academy` teaches prerequisites and mental models
- `Whitepaper` explains the vNext architecture as a cohesive thesis
- `Guides` handle onboarding and migration
- `Reference` covers API and contract details
- `Research` adds citations, related projects, and evolution notes
- `Project Status` defines support and maintenance boundaries

### 3. Treat the homepage as a thesis gateway

The homepage will be rewritten around:

- a precise project thesis
- proof-oriented summary blocks
- role-based reading entrances
- explicit links into architecture, support boundaries, and reproducibility

It should feel like the abstract and reading guide of a technical paper, not a marketing landing page.

### 4. Introduce a BitCal-specific visual language on top of the kimi-cli skeleton

The site will reuse the proven structural bones of the kimi-cli theme, but the visual identity will shift toward a more restrained, academic, and systems-oriented tone:

- lower-saturation cool palette
- stronger typographic hierarchy
- more whitespace
- evidence cards instead of promotional cards
- diagrams as first-class explanatory assets

### 5. Establish a theme-aware diagram and SVG policy

Diagrams and illustrations will follow a unified rendering strategy:

- Mermaid for maintainable structural diagrams
- curated SVG/HTML illustrations for high-value architecture visuals
- shared semantic color tokens for light and dark themes
- no hard-coded text/background combinations that break across themes
- one canonical source per figure

This directly addresses the current light/dark SVG readability weakness.

### 6. Align content with the active vNext OpenSpec

The whitepaper narrative will center on the active redesign direction:

- C++23 baseline
- x86-64-first support posture
- `bit_block`, `bit_view`, `const_bit_view`, and free algorithms
- explicit migration posture
- reproducible benchmark and correctness narratives

The site should no longer read like a mixed archive of pre-vNext and vNext messaging.

### 7. Upgrade bilingual scope to near-full mirrored structure

This redesign assumes a much stronger bilingual stance than the current lightweight policy. Chinese and English should share the same site structure and major reading path, even if some lower-level phrasing is adapted for fluency.

## Proposed Content Structure

### Home

- project thesis
- key claims with evidence hooks
- audience-specific reading entrances
- direct paths to whitepaper, guides, reference, and GitHub

### Academy

- why BitCal exists
- bit-level mental model
- SIMD and dispatch primer
- performance methodology
- terminology and conceptual background

### Whitepaper

- vNext architecture overview
- public model and layering
- support matrix and platform posture
- correctness and benchmarking stance
- migration posture

### Guides

- installation
- quick start
- verification flow
- migration guide

### Reference

- public API model
- concepts and contracts
- backend boundary explanations where user-visible

### Research

- related open source projects
- reference papers or technical articles
- evolution notes and design trade-off discussions
- methodology notes for interpretation of benchmark claims

### Project Status

- support matrix
- maintenance posture
- versioning and breaking-change policy
- retained links to changelog and release notes

## Visual and UX System

### Principles

- clarity over decoration
- proof over hype
- visual hierarchy over feature-card repetition
- consistent contrast in both themes
- scannable entry points for advanced readers

### Components to Rework

- homepage hero
- reading-path blocks
- evidence cards
- architecture figure blocks
- research / citation blocks
- status and support tables

### Dark/Light Strategy

- use semantic color tokens rather than page-local hacks
- keep backgrounds and stroke colors theme-aware
- treat diagrams as designed surfaces, not pasted images
- verify diagram typography against both themes

## Deployment and Validation Boundaries

The redesign keeps the existing GitHub Pages publishing path, but validation must explicitly cover:

- VitePress build success
- locale routing
- base-path correctness
- local search availability
- Mermaid rendering
- theme switching
- SVG/diagram readability in both themes
- navigation parity across Chinese and English

## Risks

### Content Volume

A near-full bilingual whitepaper site is expensive to maintain.

**Mitigation:** keep structure mirrored, share figure systems and layout primitives, and avoid redundant content forms that create double maintenance.

### Policy Mismatch

Current documentation policy is more conservative than the target site ambition.

**Mitigation:** update the relevant specification and documentation policy before or alongside the implementation work so the new site has an explicit governance basis.

### Framework Mimicry Without Identity

A shallow kimi-cli imitation would make the site feel derivative.

**Mitigation:** preserve the engineering skeleton but make BitCal's IA, proof structure, terminology, and visuals project-specific.

## Implementation Shape

The implementation should proceed in staged workstreams:

1. specification and policy alignment
2. site audit and keep/rewrite/merge/delete matrix
3. information architecture rebuild
4. theme and design-system rewrite
5. diagram and SVG system rebuild
6. bilingual content rewrite
7. deployment and validation pass

## Acceptance Criteria

This design is considered satisfied when:

1. the published Pages experience reads as a coherent technical whitepaper site
2. the homepage acts as a strong technical reading gateway
3. vNext narrative drift is removed
4. diagrams remain legible in light and dark themes
5. Chinese and English navigation structures are aligned
6. the site is clearly more rigorous and better organized than the current implementation
