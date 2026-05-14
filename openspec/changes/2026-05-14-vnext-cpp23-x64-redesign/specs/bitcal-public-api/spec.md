## ADDED Requirements

### Requirement: Stable public include SHALL remain the umbrella header in vNext
BitCal SHALL keep `<bitcal/bitcal.hpp>` as the only stable public include seam for the next-generation API.

#### Scenario: A consumer includes BitCal in vNext examples
- **WHEN** user-facing samples or documentation show how to consume the library
- **THEN** they MUST include `<bitcal/bitcal.hpp>`
- **AND** they MUST NOT present internal headers as separate stable include contracts

### Requirement: vNext public API SHALL center on blocks, views, and free algorithms
BitCal SHALL define its next public API around owning `bit_block<Bits>`, non-owning `bit_view` / `const_bit_view`, and free algorithms operating on those public types.

#### Scenario: Maintainers describe the vNext public surface
- **WHEN** the next-generation API is specified
- **THEN** `bit_block`, `bit_view`, `const_bit_view`, and public algorithms MUST be treated as the core public model
- **AND** the older `bitarray` retained contract MUST NOT be treated as the architectural center for vNext
