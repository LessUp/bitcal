## Why
BitCal Pages already uses the retained VitePress pipeline, but the public site still reads like a transitional docs portal instead of a coherent vNext whitepaper.

## What Changes
- Rebuild the GitHub Pages IA around Home / Academy / Whitepaper / Guides / Reference / Research / Project Status
- Mirror the primary audience-facing navigation structure across English and Chinese, with documented policy exceptions for deep technical body content
- Define Academy as the explanatory/primer layer that introduces mental models and routes readers into whitepaper/reference material
- Add a theme-aware figure and SVG policy for light/dark readability
- Align all public Pages narrative with the active vNext redesign

## Impact
- Affects `project-documentation-site` change spec under `openspec/changes/2026-05-15-pages-whitepaper-overhaul/specs/`
- Affects docs/README.md, docs/.vitepress/*, and the bilingual docs tree
- Requires validation of VitePress build, routes, Mermaid rendering, search, and theme switching
