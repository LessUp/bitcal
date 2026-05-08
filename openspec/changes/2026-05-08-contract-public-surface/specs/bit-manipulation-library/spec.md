## ADDED Requirements

### Requirement: Public surface contraction SHALL ship as a major release
BitCal SHALL publish the retained public-surface contraction as version 3.0.0 and SHALL describe it as a breaking release.

#### Scenario: Release planning is updated for the contracted API
- **WHEN** maintainers align specs, docs, tests, and version semantics for the retained public surface
- **THEN** the release target MUST be a major version increment
- **AND** migration-facing materials MUST explain the public APIs that were removed or re-scoped

### Requirement: Product-facing materials SHALL describe the contracted public boundary
BitCal SHALL present itself as a header-only library with a single stable public include seam and a `bitarray`-centered retained API.

#### Scenario: Product materials describe how BitCal is consumed
- **WHEN** README, Pages, or product specs summarize the library surface
- **THEN** they MUST describe `<bitcal/bitcal.hpp>` as the stable include seam
- **AND** they MUST NOT present helper namespaces, traits, or undocumented convenience methods as retained public features
