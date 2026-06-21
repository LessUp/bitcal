# BitCal Copilot Instructions

## Project posture

BitCal is a compact C++23 header-only SIMD practice repo.
Prefer simplification and consistency over process-heavy expansion.

## Mandatory workflow

1. Keep edits narrow and reversible.
2. Sync code, tests, and README together on public behavior changes.
3. Do not keep dead code or speculative backend layers.

## Repository-specific rules

- Core code: `include/bitcal/`
- Tests: `tests/test_bitcal.cpp` (existing lightweight macros)
- Build with a clean CMake dir (`build-test/` etc.)
- Header-only constraints apply to release/CI/package changes

## Documentation rules

- No duplicate Markdown/HTML mirror tracks
- Keep docs focused on maintained paths
- Keep claims aligned with retained verification paths

## Workflow and platform rules

- Minimize GitHub Actions sprawl
- Public platform/perf claims must match retained tests/benchmarks
- Keep backend contract truthful (`scalar` + `avx2`)

## AI tooling rules

- `AGENTS.md` defines cross-agent execution policy
- `CLAUDE.md` is Claude-session supplement
- Avoid unnecessary external integrations for local repo reasoning
