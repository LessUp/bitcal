## Why
BitCal Pages already uses the retained VitePress pipeline, but the public site still reads like a transitional docs portal instead of a coherent vNext whitepaper.

## What Changes
- Rebuild the GitHub Pages IA around Home / Academy / Whitepaper / Guides / Reference / Research / Project Status
- Upgrade the site to near-full bilingual mirrored structure
- Add a theme-aware figure and SVG policy for light/dark readability
- Align all public Pages narrative with the active C++23 x86-64-first vNext redesign

## Impact
- Affects openspec/specs/project/project-documentation-site.md
- Affects docs/README.md, docs/.vitepress/*, and the bilingual docs tree
- Requires validation of VitePress build, routes, Mermaid rendering, search, and theme switching
