# BitCal Copilot Instructions

## Project posture

BitCal is in a final stabilization phase. Prefer cleanup, consolidation, and contract fidelity over feature expansion.

## Mandatory workflow

1. Read the active OpenSpec change before implementing
2. Treat `openspec/` as the only active specification source
3. Never add new active content under legacy `specs/`
4. Keep commits and edits narrow, reversible, and reviewable

## Repository-specific rules

- Core library code lives in `include/bitcal/`
- Tests live in `tests/test_bitcal.cpp` and use the existing lightweight macro test style
- Build and verify with a clean CMake directory such as `build-test/`
- BitCal is header-only: release, CI, and packaging changes must reflect that

## Documentation rules

- Do not create or maintain duplicate Markdown/HTML mirrors for the same deep content
- Use `docs/README.md` as the documentation architecture policy
- Keep entry pages attractive, but keep deep technical reference in canonical repository Markdown

## Workflow and platform rules

- Minimize GitHub Actions sprawl
- Only keep platform or performance claims that still have a retained verification path
- Any public API or behavior change must update OpenSpec, documentation, and version semantics together

## AI tooling rules

- `AGENTS.md` is the cross-agent execution manual
- `CLAUDE.md` is the Claude-specific session supplement
- Prefer CLI skills for process discipline and small context cost
- Reserve MCP/external integrations for live GitHub or web state, not for internal repository reasoning
