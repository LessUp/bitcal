## MODIFIED Requirements

### Requirement: Documentation SHALL have a single maintained source
BitCal SHALL define one maintained source for each documentation artifact, and any rendered HTML, search asset, or retained Pages release-notes entry MUST be derived from that source rather than manually mirrored.

#### Scenario: Updating release notes for GitHub Pages
- **WHEN** a maintainer updates the project changelog and keeps a changelog entry page under `docs/`
- **THEN** the root `CHANGELOG.md` / `CHANGELOG.zh-CN.md` files MUST remain the canonical sources
- **AND** the Pages changelog page MUST be treated as a synchronized or derived navigation entry, not as a separately maintained source

### Requirement: The public site SHALL separate promotion, academy, guidance, reference, research, and whitepaper content
BitCal SHALL structure its GitHub Pages experience as a layered entry flow: a root homepage for project promotion, a docs landing page for documentation navigation, an academy layer for explanatory/primer content, and distinct primary section trees for whitepaper, guides, API/reference material, research material, and project-status context. Academy content MUST introduce mental models and route readers into deeper whitepaper/reference material rather than becoming a second canonical home for deep technical body content.

#### Scenario: User lands on the site homepage
- **WHEN** a new user opens the BitCal Pages site
- **THEN** the homepage MUST communicate the vNext thesis and route readers into the docs landing page and the primary academy, whitepaper, guides, reference, research, and maintenance paths

#### Scenario: User enters the documentation landing page
- **WHEN** a reader opens the primary documentation entry page
- **THEN** that page MUST act as the landing layer for the documentation tree
- **AND** it MUST expose the primary academy, whitepaper, guides, reference, research, and project-status sections as distinct navigation paths

#### Scenario: Reader enters Academy
- **WHEN** a reader opens an Academy entry page
- **THEN** that page MUST explain the concepts, terminology, or reading path needed to approach the deeper material
- **AND** any deep technical body content introduced there MUST remain canonical Markdown in the appropriate whitepaper, guides, reference, or research section

### Requirement: High-value Pages surfaces SHALL use a reusable theme-component layer
BitCal SHALL provide a lightweight reusable VitePress theme-component layer for high-value Pages surfaces so shared presentation patterns can be implemented once while deep technical content remains canonical repository Markdown.

#### Scenario: Maintainers rebuild a high-value audience-facing surface
- **WHEN** maintainers implement or revise the homepage, docs landing page, whitepaper entry, research entry, or other high-value audience-facing Pages surface
- **THEN** shared presentation elements such as hero blocks, callouts, and figure chrome MUST come from the reusable theme-component layer
- **AND** the page's deep technical narrative MUST continue to be maintained in canonical repository Markdown rather than in separately maintained HTML or component-owned duplicates

### Requirement: Figures SHALL remain readable in both site themes
BitCal SHALL render whitepaper and other high-value documentation figures through a theme-aware figure path that preserves readability in both light and dark themes, with SVG as the default retained format for theme-sensitive diagrams.

#### Scenario: Reader switches between light and dark themes
- **WHEN** a reader views a figure or SVG and switches between the site's light and dark themes
- **THEN** labels, strokes, fills, emphasis, and figure chrome MUST remain legible against the active theme background
- **AND** readability MUST be preserved through the retained theme-aware figure or SVG styling path rather than by treating one theme as unsupported

#### Scenario: Maintainer adds a theme-sensitive diagram
- **WHEN** a maintainer adds or refreshes a whitepaper or research diagram whose colors or annotations must survive theme switching
- **THEN** SVG MUST be the default retained asset format
- **AND** its presentation MUST be driven by theme-aware styling or tokens rather than by maintaining separate manually synchronized light-only and dark-only figure copies

### Requirement: Bilingual maintenance SHALL support mirrored site structure
BitCal SHALL keep the English and Chinese site trees structurally aligned for all primary audience-facing sections, while allowing documented bilingual-policy exceptions for deep technical body content below that mirrored navigation layer.

#### Scenario: Maintainers plan bilingual primary navigation
- **WHEN** maintainers define the Home, Academy, Whitepaper, Guides, Reference, Research, or Project Status reading paths
- **THEN** the English and Chinese trees MUST expose the same primary audience-facing section structure unless an explicit policy exception is documented before publication

#### Scenario: A maintainer adds or removes a primary audience-facing section
- **WHEN** a maintainer adds, removes, or renames a primary audience-facing section in one language tree
- **THEN** the corresponding section in the other language tree MUST be added, removed, or renamed to preserve mirrored structure
- **AND** any intentional mismatch MUST be documented in the bilingual policy before the change is published

#### Scenario: Maintainers keep a deep technical body page as an exception
- **WHEN** maintainers choose not to mirror a deep whitepaper, research, guides, or reference body page across both languages
- **THEN** the mirrored primary navigation structure MUST still be preserved for the audience-facing section that contains it
- **AND** the exception MUST be documented in the bilingual policy before publication
