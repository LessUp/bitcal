# Status

BitCal is in an explicit vNext redesign phase for a breaking v4.0.0 line, not in a “feature-complete stable library” phase.

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

## Single sources of truth

- Active requirements: `openspec/`
- Documentation policy: `docs/README.md`
- Stable public include seam: `<bitcal/bitcal.hpp>`
- Version source: `include/bitcal/config.hpp`

## Current execution order

1. Freeze the vNext contract in OpenSpec
2. Land a compiling C++23 public skeleton
3. Rebuild correctness and benchmark truth around the new surface
4. Optimize x86-64 kernels on top of the rebuilt baseline
5. Publish whitepaper and metrics only after retained evidence exists

## Next steps

- Use the [Guide](/en/guide/index) for the current reading path and verification posture
- Read the [Whitepaper](/en/whitepaper/index) for the intended public model
- Review [Performance](/en/performance/index) before treating any benchmark number as a product promise
