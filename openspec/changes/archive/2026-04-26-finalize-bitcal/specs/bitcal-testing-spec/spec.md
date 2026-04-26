## ADDED Requirements

### Requirement: Testing scope SHALL match the retained public contract
BitCal SHALL focus automated testing on the public behaviors, platforms, and invariants that remain part of the final maintained contract after cleanup.

#### Scenario: A behavior is retained as supported
- **WHEN** the project keeps a public operation, platform claim, or backend guarantee
- **THEN** the automated test strategy MUST include validation for that retained contract

### Requirement: Validation workflows SHALL match the documented support policy
BitCal SHALL ensure that CI validation, manual verification instructions, and testing specs describe the same maintained support matrix.

#### Scenario: Maintainers review the support matrix
- **WHEN** support claims are listed in testing specs or documentation
- **THEN** the workflow configuration and verification instructions MUST align with those claims and omit unsupported combinations
