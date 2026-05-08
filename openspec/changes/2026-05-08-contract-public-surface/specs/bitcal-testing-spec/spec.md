## ADDED Requirements

### Requirement: Public contract tests SHALL use the stable umbrella include
BitCal SHALL validate the retained public API through `<bitcal/bitcal.hpp>` so contract tests exercise the supported public seam rather than internal header structure.

#### Scenario: Maintainers write or update public API tests
- **WHEN** a test claims to validate retained public behavior
- **THEN** it MUST compile against `<bitcal/bitcal.hpp>`
- **AND** it MUST NOT rely on direct inclusion of internal or non-stable headers as part of the public contract

### Requirement: Removed public helpers SHALL leave the retained test matrix
BitCal SHALL stop treating removed helper namespaces, traits, and undocumented convenience methods as part of the retained automated contract before the 3.0.0 release is declared complete.

#### Scenario: The contract suite is pruned for 3.0.0
- **WHEN** tests are aligned with the retained public surface
- **THEN** coverage expectations for removed helper APIs MUST be deleted or reclassified as non-contract implementation tests
