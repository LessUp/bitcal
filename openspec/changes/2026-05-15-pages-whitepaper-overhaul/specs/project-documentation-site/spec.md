## MODIFIED Requirements

### Requirement: Documentation SHALL have a single maintained source
BitCal SHALL define one maintained source for each documentation artifact, and any rendered HTML, search asset, or retained Pages release-notes entry MUST be derived from that source rather than manually mirrored.

#### Scenario: Updating release notes for GitHub Pages
- **WHEN** a maintainer updates the project changelog and keeps a changelog entry page under `docs/`
- **THEN** the root `CHANGELOG.md` / `CHANGELOG.zh-CN.md` files MUST remain the canonical sources
- **AND** the Pages changelog page MUST be treated as a synchronized or derived navigation entry, not as a separately maintained source

### Requirement: The public site SHALL separate promotion, guidance, reference, research, and whitepaper content
BitCal SHALL structure its GitHub Pages experience as a layered entry flow: a root homepage for project promotion, a docs landing page for documentation navigation, and distinct primary section trees for academy, whitepaper, guides, API/reference material, research material, and project-status context.

#### Scenario: User lands on the site homepage
- **WHEN** a new user opens the BitCal Pages site
- **THEN** the homepage MUST communicate the vNext thesis and route readers into the docs landing page and the primary academy, whitepaper, guides, reference, research, and maintenance paths

#### Scenario: User enters the documentation landing page
- **WHEN** a reader opens the primary documentation entry page
- **THEN** that page MUST act as the landing layer for the documentation tree
- **AND** it MUST expose the primary academy, whitepaper, guides, reference, research, and project-status sections as distinct navigation paths

### Requirement: Bilingual maintenance SHALL support mirrored site structure
BitCal SHALL keep the English and Chinese site trees structurally aligned for all primary audience-facing sections.

#### Scenario: Maintainers plan bilingual primary navigation
- **WHEN** maintainers define the Home, Academy, Whitepaper, Guides, Reference, Research, or Project Status reading paths
- **THEN** the English and Chinese trees MUST expose the same primary audience-facing section structure unless an explicit policy exception is documented before publication

#### Scenario: A maintainer adds or removes a primary audience-facing section
- **WHEN** a maintainer adds, removes, or renames a primary audience-facing section in one language tree
- **THEN** the corresponding section in the other language tree MUST be added, removed, or renamed to preserve mirrored structure
- **AND** any intentional mismatch MUST be documented in the bilingual policy before the change is published
