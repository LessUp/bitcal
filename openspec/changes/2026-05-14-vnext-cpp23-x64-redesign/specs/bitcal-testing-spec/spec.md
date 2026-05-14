## ADDED Requirements

### Requirement: vNext testing SHALL validate the new public contract first
BitCal SHALL validate the next-generation redesign through contract tests that compile through `<bitcal/bitcal.hpp>` and exercise the new owning block, non-owning view, and public algorithm seams.

#### Scenario: Maintainers define the first vNext test layer
- **WHEN** the next-generation test strategy is written
- **THEN** it MUST include public smoke coverage for `bit_block`, `bit_view` / `const_bit_view`, and public algorithms
- **AND** it MUST NOT use the old `bitarray` retained contract as the definition of vNext public correctness

### Requirement: vNext support claims SHALL prioritize x86-64 validation
BitCal SHALL align its next-generation validation posture around x86-64 retained test and build paths before publishing broader support or performance claims.

#### Scenario: A vNext support or performance claim is published
- **WHEN** documentation or specs describe a supported platform or benchmark result
- **THEN** the repository MUST retain a corresponding x86-64 validation path
- **AND** non-primary platforms MUST NOT receive stronger guarantees than the retained validation path supports
