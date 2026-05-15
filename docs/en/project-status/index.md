# Project Status

BitCal is in an explicit vNext redesign phase for a breaking v4.0.0 line, not in a "feature-complete stable library" phase.

## This section defines

- support posture
- maintenance boundaries
- migration expectations
- versioning and breaking-change policy

## Current posture

- C++23 is the public design baseline
- optimization and validation are x86-64-first
- the new public model centers on `bit_block`, `bit_view`, and free algorithms
- vNext does not promise a code-level compatibility layer for the retained `bitarray` API

## Support and maintenance boundaries

### Primary

- Linux x86-64 (GCC / Clang)
- Windows x86-64 (MSVC)
- correctness and benchmark truth for the new public surface

### Secondary

- Linux ARM64
- macOS ARM64
- follow-up Pages whitepaper polish and published performance dashboards

Secondary targets must not receive stronger support or performance claims than the retained validation path supports.

## Repository rules that stay in force

- Active requirements live in `openspec/`
- `<bitcal/bitcal.hpp>` remains the only stable public include seam
- docs, specs, code, benchmarks, and version semantics must move together
- benchmark credibility matters more than premature platform or performance claims

## Detailed status ledger

This localized page is the primary navigation destination. A retained detailed status ledger remains available at [/project-status](/project-status) for the full support and handoff notes.
