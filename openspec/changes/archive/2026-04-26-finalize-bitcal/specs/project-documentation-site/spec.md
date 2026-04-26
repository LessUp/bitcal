## ADDED Requirements

### Requirement: Documentation SHALL have a single maintained source
BitCal SHALL define one maintained source for each documentation artifact, and any rendered HTML, index, or search asset MUST be derived from that source rather than manually mirrored.

#### Scenario: Updating a reference page
- **WHEN** a maintainer updates API or architecture documentation
- **THEN** the change MUST be made in the canonical source file and not in a parallel manually maintained duplicate

### Requirement: The public site SHALL separate promotion, guidance, and reference
BitCal SHALL structure its GitHub Pages experience so that project promotion, quick-start guidance, deep reference material, and archival/maintenance context are clearly separated.

#### Scenario: User lands on the site homepage
- **WHEN** a new user opens the BitCal Pages site
- **THEN** the homepage MUST communicate the library value proposition and provide clear navigation into installation, API reference, architecture, and maintenance context

### Requirement: Bilingual maintenance SHALL be policy-driven
BitCal SHALL define which pages are bilingual and which pages are single-language, and this policy MUST be documented before large-scale documentation deletion or consolidation begins.

#### Scenario: Documentation consolidation is planned
- **WHEN** maintainers decide to remove or merge duplicated documents
- **THEN** the bilingual policy MUST identify which audience-facing pages require Chinese, English, or bilingual coverage
