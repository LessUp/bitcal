# BitCal Project Status

## Current posture

BitCal is no longer in a pure archive-readiness contraction phase. The repository is now in an active **vNext redesign** focused on:

- a C++23 public baseline
- an x86-64-first optimization and validation posture
- a new public model built around `bit_block`, `bit_view`, and free algorithms
- no code-level compatibility layer for the older `bitarray` API

## Single sources of truth

- Active requirements: `openspec/`
- AI execution rules: `AGENTS.md`
- Copilot project guidance: `.github/copilot-instructions.md`
- Version source: `include/bitcal/config.hpp`
- Documentation policy: `docs/README.md`

## What is primary vs secondary

### Primary

- Linux x86-64 (GCC / Clang)
- Windows x86-64 (MSVC)
- correctness and benchmark truth for the new public surface

### Secondary

- Linux ARM64
- macOS ARM64
- follow-up Pages whitepaper and published performance dashboards

Secondary targets must not receive stronger support or performance claims than the retained validation path supports.

## Current execution order

1. Freeze the vNext contract in OpenSpec
2. Land a compiling C++23 public skeleton
3. Rebuild correctness and benchmark truth around the new surface
4. Optimize x86-64 kernels on top of the rebuilt baseline
5. Publish whitepaper and metrics only after retained evidence exists

## Handoff notes

- `<bitcal/bitcal.hpp>` remains the only stable public include seam
- the repository is intentionally accepting breaking change for v4.0.0
- benchmark credibility matters more than premature performance claims
- documentation, specs, code, and version semantics must move together
