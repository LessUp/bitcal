# Aggressive Docs Pruning Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remove BitCal's compatibility-only docs surfaces and Pages changelog mirror so the repository keeps only the maintained primary IA plus the root changelog text files.

**Architecture:** This cleanup treats the primary docs IA as the only retained Pages surface and deletes compatibility trees instead of redirecting them. The implementation first tightens OpenSpec and IA validation so the old surfaces become invalid, then removes the duplicate routes/tooling, and finally rewrites maintained docs copy to describe the new single-truth model.

**Tech Stack:** Markdown, VitePress config/scripts, Node.js docs validation scripts, OpenSpec, git

---

### Task 1: Tighten the contract before deleting files

**Files:**
- Modify: `openspec/changes/2026-05-15-pages-whitepaper-overhaul/proposal.md`
- Modify: `openspec/changes/2026-05-15-pages-whitepaper-overhaul/design.md`
- Modify: `openspec/changes/2026-05-15-pages-whitepaper-overhaul/tasks.md`
- Modify: `openspec/changes/2026-05-15-pages-whitepaper-overhaul/specs/project-documentation-site/spec.md`
- Modify: `docs/README.md`
- Test: `docs/scripts/check-ia.mjs`

- [ ] **Step 1: Rewrite the active spec and policy from “compatibility aliases may remain” to “compatibility-only docs are cleanup targets”**

Update the text so it explicitly says:

```md
- Root `CHANGELOG.md` / `CHANGELOG.zh-CN.md` are the only changelog surface to maintain.
- compatibility-only doc trees are removal candidates, not retained aliases.
- `docs/` is the source for the primary IA only, not a storage area for legacy route shims.
```

- [ ] **Step 2: Make IA validation fail on the old structure**

In `docs/scripts/check-ia.mjs`, replace compatibility-presence checks with absence checks for these paths:

```js
const deletedLegacyPaths = [
  'project-status.md',
  'en/academy',
  'zh/academy',
  'en/api',
  'zh/api',
  'en/architecture',
  'zh/architecture',
  'en/getting-started',
  'zh/getting-started',
  'en/project-status',
  'zh/project-status',
  'en/release-notes',
  'zh/release-notes',
]
```

- [ ] **Step 3: Run the IA check before production cleanup**

Run:

```bash
cd docs && npm run check:ia
```

Expected: **FAIL** because the compatibility trees and changelog mirror still exist.

- [ ] **Step 4: Commit the contract-tightening slice**

Run:

```bash
git add openspec/changes/2026-05-15-pages-whitepaper-overhaul docs/README.md docs/scripts/check-ia.mjs
git commit -m "docs: tighten pruning contract"
```

### Task 2: Delete compatibility-only docs and changelog mirror content

**Files:**
- Delete: `docs/en/academy/**`
- Delete: `docs/zh/academy/**`
- Delete: `docs/en/api/**`
- Delete: `docs/zh/api/**`
- Delete: `docs/en/architecture/**`
- Delete: `docs/zh/architecture/**`
- Delete: `docs/en/getting-started/**`
- Delete: `docs/zh/getting-started/**`
- Delete: `docs/en/project-status/**`
- Delete: `docs/zh/project-status/**`
- Delete: `docs/en/release-notes/**`
- Delete: `docs/zh/release-notes/**`
- Delete: `docs/en/performance/methodology.md`
- Delete: `docs/zh/performance/methodology.md`
- Delete: `docs/en/whitepaper/performance.md`
- Delete: `docs/zh/whitepaper/performance.md`
- Delete: `docs/project-status.md`
- Delete: `docs/design/backend-unified-interface.md`
- Delete: `docs/~/**`

- [ ] **Step 1: Delete the duplicate route trees in one focused patch**

Use one deletion patch covering:

```text
academy/
api/
architecture/
getting-started/
project-status/
release-notes/
performance/methodology.md
whitepaper/performance.md
docs/project-status.md
docs/design/backend-unified-interface.md
docs/~/
```

- [ ] **Step 2: Verify the files are actually gone**

Run:

```bash
cd /home/shane/lessup/bitcal
find docs/en docs/zh -maxdepth 2 \( -name academy -o -name api -o -name architecture -o -name getting-started -o -name project-status -o -name release-notes \)
```

Expected: **no output**.

- [ ] **Step 3: Commit the structural prune**

Run:

```bash
git add -A docs/en docs/zh docs/project-status.md docs/design docs/~
git commit -m "docs: remove compatibility-only routes"
```

### Task 3: Remove tooling that exists only for deleted compatibility content

**Files:**
- Modify: `docs/package.json`
- Delete: `docs/scripts/sync-changelog.mjs`
- Modify: `docs/.vitepress/config.ts`
- Modify: `docs/.vitepress/page-exclusions.js`
- Modify: `docs/scripts/check-ia.mjs`

- [ ] **Step 1: Remove changelog-sync from the docs toolchain**

Change `docs/package.json` from:

```json
"sync": "node scripts/sync-changelog.mjs",
"dev": "npm run sync && vitepress dev",
"build": "npm run sync && vitepress build && npm run check:clean-url-artifacts"
```

to:

```json
"dev": "vitepress dev",
"build": "vitepress build && npm run check:clean-url-artifacts"
```

and delete `docs/scripts/sync-changelog.mjs`.

- [ ] **Step 2: Simplify VitePress exclusions to match the retained tree**

In `docs/.vitepress/config.ts` and `docs/.vitepress/page-exclusions.js`, remove:

```ts
compatPageExcludes
llmsDerivedPageExcludes
```

so the config keeps only retained exclusions like:

```ts
srcExclude: [
  'README.md',
  'superpowers/**',
]
```

unless another deleted artifact still requires exclusion.

- [ ] **Step 3: Rewrite IA validation around absence, not compatibility bookkeeping**

Delete validation blocks that require:

```js
compatPageExcludes.includes(...)
llmsIgnoreFilesBlock?.includes('...compatPageExcludes')
llmsIgnoreFilesBlock?.includes('...llmsDerivedPageExcludes')
```

and replace them with checks like:

```js
for (const relativePath of deletedLegacyPaths) {
  expect(!exists(relativePath), `Legacy docs path must be removed: ${relativePath}`)
}
```

- [ ] **Step 4: Run IA validation again**

Run:

```bash
cd docs && npm run check:ia
```

Expected: **PASS** on the new primary-only model.

- [ ] **Step 5: Commit the tooling simplification**

Run:

```bash
git add docs/package.json docs/.vitepress/config.ts docs/.vitepress/page-exclusions.js docs/scripts/check-ia.mjs
git commit -m "docs: remove legacy docs tooling"
```

### Task 4: Rewrite maintained references to the new single-truth model

**Files:**
- Modify: `README.md`
- Modify: `README.zh-CN.md`
- Modify: `docs/index.md`
- Modify: `docs/README.md`
- Modify: `docs/en/index.md`
- Modify: `docs/zh/index.md`
- Modify: any retained docs page that still mentions compatibility-only routes after deletion

- [ ] **Step 1: Fix root README references**

Replace:

```md
- Project status lives in `docs/project-status.md`
```

with wording that points to the retained status section and root changelog:

```md
- Project status lives in `docs/en/status/` and `docs/zh/status/`
- Release history lives in `CHANGELOG.md` / `CHANGELOG.zh-CN.md`
```

- [ ] **Step 2: Remove “legacy aliases remain” language from docs landing and policy**

Delete wording like:

```md
Legacy academy paths remain compatibility-only aliases
```

and replace it with:

```md
The docs tree now retains only the maintained primary IA.
```

- [ ] **Step 3: Search for stale route references and patch the retained pages**

Run:

```bash
cd /home/shane/lessup/bitcal
rg "project-status|release-notes|academy/|api/|architecture/|getting-started/" README.md README.zh-CN.md docs --glob '*.md'
```

Expected: results should be limited to historical design notes under `docs/superpowers/` or OpenSpec context, not retained user-facing docs.

- [ ] **Step 4: Commit the maintained-copy rewrite**

Run:

```bash
git add README.md README.zh-CN.md docs/index.md docs/README.md docs/en docs/zh
git commit -m "docs: align retained docs references"
```

### Task 5: Rebuild and verify the pruned site

**Files:**
- Verify: `docs/`
- Verify: `openspec/changes/2026-05-15-pages-whitepaper-overhaul/**`

- [ ] **Step 1: Run the retained docs validation suite**

Run:

```bash
cd docs && npm run check:content-contract && npm run check:theme-contract && npm run check:ia && npm run build
```

Expected: all commands pass.

- [ ] **Step 2: Verify diff hygiene**

Run:

```bash
cd /home/shane/lessup/bitcal && git --no-pager diff --check
```

Expected: no output.

- [ ] **Step 3: Inspect the final file layout**

Run:

```bash
cd /home/shane/lessup/bitcal
find docs -maxdepth 2 -type d | sort
```

Expected: only retained primary sections plus `.vitepress`, `public`, `scripts`, `superpowers`, and other intentional infrastructure directories remain.

- [ ] **Step 4: Commit the final verification pass**

Run:

```bash
git add -A
git commit -m "docs: prune legacy Pages surfaces"
```

## Self-review

- **Spec coverage:** Task 1 covers OpenSpec/policy alignment, Task 2 covers file deletion, Task 3 covers tooling removal, Task 4 covers maintained-copy rewrites, and Task 5 covers validation. No spec section is left without a task.
- **Placeholder scan:** All tasks name exact files, commands, and deleted path groups; no TBD/TODO markers remain.
- **Type consistency:** The plan uses one retained model throughout: root changelog as the only changelog truth, primary IA only, no compatibility-route retention.
