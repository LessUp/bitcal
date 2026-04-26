## 1. Governance and repository truth

- [x] 1.1 Audit `openspec/`, legacy `specs/`, root docs, AI control files, and tracked auxiliary directories to produce the final keep/remove/migrate matrix
- [x] 1.2 Convert `openspec/` into the sole active specification source and decide whether legacy `specs/` becomes a migration shell or is removed entirely
- [x] 1.3 Rewrite repository control documents (`AGENTS.md`, `CLAUDE.md`, and supporting governance text) to encode final-state OpenSpec, review, and archive rules

## 2. Documentation and Pages consolidation

- [x] 2.1 Define the final documentation information architecture, bilingual policy, and canonical source policy for rendered site artifacts
- [x] 2.2 Remove duplicated or obsolete documentation assets from `docs/`, including low-value changelog mirrors and manually mirrored content
- [x] 2.3 Rebuild the GitHub Pages homepage, docs landing page, and supporting site assets so the site promotes BitCal clearly and reflects the final maintenance posture

## 3. Core library, tests, and version alignment

- [x] 3.1 Audit `include/bitcal/`, `tests/`, `examples/`, `benchmarks/`, and CMake files for correctness issues, drifted contracts, and cleanup opportunities that affect the retained public surface
- [x] 3.2 Apply the required code and test refactors, including any necessary breaking API or behavior cleanup, and update the public API documentation/specs to match
- [x] 3.3 Establish a single version source and align README, changelog, release references, CMake metadata, and dependency/toolchain policy around it

## 4. Engineering workflows and GitHub integration

- [x] 4.1 Review every GitHub Actions workflow and classify it as retained, merged, converted to manual trigger, or removed
- [x] 4.2 Implement the simplified workflow set, including path filters, release policy changes, and final CI/CD responsibility boundaries
- [x] 4.3 Use `gh` to update repository description, homepage, topics, and Pages settings so GitHub metadata matches the final project positioning
- [x] 4.4 Document the lightweight development flow that uses OpenSpec and `/review` without accumulating long-lived unmerged branches

## 5. AI toolchain finalization and handoff

- [x] 5.1 Generate and rewrite `copilot-instructions.md` so it becomes BitCal-specific rather than template-driven
- [x] 5.2 Prune and reorganize `.claude/`, `_bmad/`, and related AI assets so only high-value entry points remain
- [x] 5.3 Define the final LSP/editor setup plus the MCP-versus-CLI-skills decision record and evaluate whether any Copilot plugin is justified
- [x] 5.4 Create the GLM handoff package and archive-readiness gate covering specs, docs, Pages, CI/CD, metadata, AI tooling, and verification expectations
