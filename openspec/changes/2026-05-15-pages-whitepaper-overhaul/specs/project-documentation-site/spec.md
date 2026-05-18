## MODIFIED Requirements

### Requirement: Documentation SHALL have a single maintained source
BitCal SHALL define one maintained source for each documentation artifact, and any rendered HTML, search asset, or retained Pages release-notes entry MUST be derived from that source rather than manually mirrored.

#### Scenario: Updating release notes for GitHub Pages
- **WHEN** a maintainer updates the project changelog and keeps a changelog entry page under `docs/`
- **THEN** the root `CHANGELOG.md` / `CHANGELOG.zh-CN.md` files MUST remain the canonical sources
- **AND** the Pages changelog page MUST be treated as a synchronized or derived navigation entry, not as a separately maintained source

### Requirement: The public site SHALL expose one mirrored primary IA
BitCal SHALL treat `Home / Docs Landing / Guide / Whitepaper / Performance / Reference / Research / Status` as the mirrored primary site model for both English and Chinese readers.

#### Scenario: User lands on the site homepage
- **WHEN** a new user opens the BitCal Pages site
- **THEN** the retained root entry and localized homepage or landing surface MUST communicate the vNext thesis
- **AND** they MUST route readers into the `Guide / Whitepaper / Performance / Reference / Research / Status` paths without depending on retired academy naming, old architecture paths, or `project-status` as a primary section name

#### Scenario: Maintainers adjust the primary navigation
- **WHEN** maintainers change a primary section in one language tree
- **THEN** the corresponding section in the other language tree MUST be updated to preserve the same primary IA
- **AND** any retained compatibility alias MUST remain outside the primary nav/sidebar contract

### Requirement: Performance SHALL remain a standalone evidence section
BitCal SHALL expose performance evidence through a standalone `/performance/` section so readers can evaluate baseline data, methodology, and claim boundaries independently from the whitepaper narrative.

#### Scenario: Reader enters the performance section
- **WHEN** a reader opens the performance entry path
- **THEN** the page MUST describe current evidence posture, retained methodology, and claim guardrails
- **AND** it MUST remain reachable from primary navigation in both languages as `Performance` / `性能`

### Requirement: Status SHALL be the only primary maintenance section name
BitCal SHALL use `Status` / `状态` as the only primary section name for project posture, support boundaries, and breaking-change policy.

#### Scenario: Maintainer keeps a compatibility alias for older links
- **WHEN** a compatibility alias such as `project-status` is retained for older links
- **THEN** that alias MUST redirect or route readers to `Status` / `状态`
- **AND** it MUST NOT appear as the preferred naming in policy, nav, sidebar, or entry-page copy

### Requirement: Bilingual maintenance SHALL support mirrored site structure
BitCal SHALL keep the English and Chinese site trees structurally aligned for all primary audience-facing sections while allowing documented bilingual-policy exceptions for deep technical body content below that mirrored navigation layer.

#### Scenario: Maintainers publish a localized entry surface
- **WHEN** maintainers update a localized homepage, landing page, or section index
- **THEN** both languages MUST expose the same `Guide / Whitepaper / Performance / Reference / Research / Status` entry structure
- **AND** any deep-content exception MUST be documented in `docs/README.md` before publication
