## Why
BitCal Pages still reads like a transitional docs portal, and the active OpenSpec package, `docs/README.md`, and the VitePress IA are no longer describing the same site model.

## What Changes
- Rebuild the GitHub Pages IA around `Home / Docs Landing / Guide / Whitepaper / Performance / Reference / Research / Status`
- Mirror the primary audience-facing navigation across English and Chinese, with `docs/README.md` documenting any deep-content bilingual exceptions
- Remove primary-path dependence on retired academy naming, legacy `project-status` naming, and old architecture routes
- Keep the retained theme-aware figure and SVG policy for light/dark readability
- Align the public Pages reading path with the active vNext redesign contract

## Impact
- Affects the `project-documentation-site` change spec under `openspec/changes/2026-05-15-pages-whitepaper-overhaul/specs/`
- Affects `docs/README.md`, `docs/.vitepress/*`, and the bilingual docs tree
- Requires validation of the executable IA check, the VitePress build, routes, Mermaid rendering, search, and theme switching
