## ADDED Requirements

### Requirement: Product positioning SHALL prioritize archive-ready stability
BitCal SHALL define itself as a stable, archive-ready SIMD bit-manipulation library whose documentation, support matrix, and engineering process reflect maintainable long-term stewardship rather than open-ended feature growth.

#### Scenario: Project materials describe BitCal
- **WHEN** maintainers update README, Pages, or product-facing specs
- **THEN** those materials MUST describe the project as a stable library with explicit scope, support expectations, and maintenance boundaries

### Requirement: Platform and performance claims SHALL reflect maintained reality
BitCal SHALL only publish platform, compiler, and performance claims that are backed by retained workflows, tests, or documented validation procedure.

#### Scenario: A support claim appears in documentation
- **WHEN** documentation states that a platform, compiler, or performance target is supported
- **THEN** the repository MUST retain a corresponding validation path or clearly mark the claim as non-guaranteed historical information
