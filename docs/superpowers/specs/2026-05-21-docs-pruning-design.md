# BitCal aggressive docs pruning design

Date: 2026-05-21

## Context

BitCal's current docs tree still carries a second layer of compatibility-only surfaces:

- derived Pages changelog entries under `docs/en|zh/release-notes/`
- compatibility-only route trees under `docs/en|zh/{academy,api,architecture,getting-started,project-status}`
- isolated compatibility pages such as `performance/methodology` and `whitepaper/performance`
- deprecated root docs artifacts such as `docs/project-status.md`, `docs/design/`, and stray junk like `docs/~`

This conflicts with the repository's own policy in `docs/README.md`, which already says the canonical change log lives at the root, low-value changelog mirrors should be deleted, and compatibility-only paths must not keep competing with the primary IA.

The user explicitly asked for aggressive cleanup to reduce future interference. This design therefore treats backward-compatible route retention as optional and chooses deletion over compatibility.

## Assumption locked for autonomous execution

Because the user is offline after requesting aggressive cleanup, this design locks one operational assumption:

- **Assumption:** old Pages links may break if that is the cost of removing duplicate or compatibility-only documentation layers.

If the user later wants redirects restored, that should be a separate follow-up change, not a reason to keep the duplicate tree alive now.

## Approaches considered

### Approach A — minimal cleanup

- Remove only the Pages changelog mirror.
- Keep compatibility alias trees and redirect pages.

**Pros:** lowest breakage risk.  
**Cons:** most interference remains; docs tree still has multiple obsolete vocabularies and duplicated route surfaces.

### Approach B — selective cleanup

- Remove the Pages changelog mirror and a few obvious aliases such as `project-status`.
- Keep most other compatibility trees.

**Pros:** lower risk than a full prune.  
**Cons:** still leaves the repo with multiple low-value doc namespaces that future maintainers must reason about.

### Approach C — aggressive prune (**recommended**)

- Keep only the maintained IA: `Guide / Whitepaper / Performance / Reference / Research / Status`.
- Keep only root `CHANGELOG.md` / `CHANGELOG.zh-CN.md` as changelog truth.
- Delete compatibility-only doc trees, compatibility-only single pages, and deprecated root docs artifacts.
- Simplify docs config, exclusions, IA checks, and README/OpenSpec wording around the new single-truth model.

**Pros:** strongest reduction in maintenance noise; lowest future ambiguity; aligns with repo policy and the user's request.  
**Cons:** older bookmarked Pages links will stop resolving.

## Chosen design

BitCal will adopt **Approach C**.

### 1. Changelog policy

- Root `CHANGELOG.md` and `CHANGELOG.zh-CN.md` remain the only changelog source.
- `docs/en/release-notes/changelog.md` and `docs/zh/release-notes/changelog.md` are removed.
- The `docs/scripts/sync-changelog.mjs` pipeline and `npm run sync` dependency are removed from the docs toolchain.
- Any maintained docs copy that talks about release history will point readers to the root changelog files instead of a Pages changelog route.

### 2. Compatibility-surface pruning

Delete the following compatibility-only trees:

- `docs/en/academy`, `docs/zh/academy`
- `docs/en/api`, `docs/zh/api`
- `docs/en/architecture`, `docs/zh/architecture`
- `docs/en/getting-started`, `docs/zh/getting-started`
- `docs/en/project-status`, `docs/zh/project-status`
- `docs/en/release-notes`, `docs/zh/release-notes`

Delete the following compatibility-only single files or deprecated root artifacts:

- `docs/en/performance/methodology.md`, `docs/zh/performance/methodology.md`
- `docs/en/whitepaper/performance.md`, `docs/zh/whitepaper/performance.md`
- `docs/project-status.md`
- `docs/design/backend-unified-interface.md`
- `docs/~` and any contents beneath it

This cleanup intentionally favors one readable documentation tree over legacy route continuity.

### 3. Config and validation simplification

- Remove compatibility-page exclusions and derived changelog exclusions from the VitePress configuration layer.
- Rewrite IA validation so it asserts the absence of compatibility-only route trees instead of merely excluding them from some checks.
- Keep existing primary-IA checks for `Guide / Whitepaper / Performance / Reference / Research / Status`.

### 4. Documentation truth updates

Update maintained docs so they no longer describe compatibility aliases as retained:

- `docs/README.md`
- `README.md`
- `README.zh-CN.md`
- `docs/index.md`
- any maintained entry pages or validation docs that still mention compatibility-only routes

The maintained narrative after cleanup is:

- OpenSpec = active requirement truth
- root changelog files = release-history truth
- `docs/` = Pages source for the primary IA only

### 5. OpenSpec alignment

The active change package `2026-05-15-pages-whitepaper-overhaul` currently permits compatibility aliases to remain. This cleanup narrows that posture:

- compatibility aliases are no longer expected to remain;
- derived Pages changelog entries are no longer part of the retained site model.

Proposal/design/tasks/spec wording in that active change must be updated so the spec matches the cleanup.

## Error handling and migration posture

- The project will not add replacement redirect pages in this cleanup.
- Broken external links to deleted compatibility routes are an accepted consequence of the aggressive prune.
- If redirect recovery becomes necessary later, it must be introduced deliberately with a small, explicit scope.

## Validation

The cleanup is complete only when all of the following hold:

1. Deleted compatibility trees no longer exist in `docs/`.
2. `docs/.vitepress/config.ts` and docs validation scripts no longer model compatibility exclusions or sync-derived changelog pages.
3. `docs/README.md`, root READMEs, and active OpenSpec all describe the same single-truth docs model.
4. `cd docs && npm run check:content-contract && npm run check:theme-contract && npm run check:ia && npm run build` passes.
5. `git --no-pager diff --check` passes.

## Out of scope

- Rewriting the primary IA again
- Reintroducing redirects for deleted legacy routes
- Reworking the technical content inside the retained primary sections unless a reference must change as part of the cleanup
