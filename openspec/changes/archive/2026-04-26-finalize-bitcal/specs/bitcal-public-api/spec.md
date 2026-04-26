## ADDED Requirements

### Requirement: Public API changes SHALL be explicitly versioned and documented
BitCal SHALL record any breaking public API or behavioral change in its API specification, migration-facing documentation, and canonical versioning source.

#### Scenario: A public API contract changes
- **WHEN** a method signature, type alias, semantic behavior, or supported contract changes in a breaking way
- **THEN** the API specification MUST describe the new contract and the project documentation MUST identify the migration implication

### Requirement: Public API documentation SHALL stay synchronized with implementation
BitCal SHALL keep user-visible API documentation aligned with the implementation and tests that define retained behavior.

#### Scenario: A documented API remains in the project
- **WHEN** an API surface is documented as supported
- **THEN** the implementation and test suite MUST still cover that documented behavior or the documentation MUST be removed or corrected
