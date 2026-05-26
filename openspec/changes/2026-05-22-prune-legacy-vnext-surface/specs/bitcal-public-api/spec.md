## ADDED Requirements

### Requirement: The umbrella include SHALL ship only the retained vNext public model
BitCal SHALL keep `<bitcal/bitcal.hpp>` as the only stable include seam and SHALL expose only the retained vNext public model through that seam.

#### Scenario: Maintainers review the umbrella header
- **WHEN** `<bitcal/bitcal.hpp>` is updated
- **THEN** it MUST expose `bit_block`, `bit_view` / `const_bit_view`, `backend_kind`, and the retained free algorithms
- **AND** it MUST NOT export `bitarray`

### Requirement: The retained vNext algorithm surface SHALL replace the removed monolithic model
BitCal SHALL ship the retained vNext free algorithms as the supported replacement surface for the removed `bitarray` public model.

#### Scenario: Maintainers review the shipped vNext algorithm set
- **WHEN** the retained public algorithm surface is evaluated
- **THEN** `bit_and`, `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `is_zero`, `popcount`, `shift_left`, and `shift_right` MUST be implemented and tested
- **AND** the project MUST NOT treat `bitarray` member functions as part of the shipped vNext public contract

#### Scenario: Public equality compares views with different lengths
- **WHEN** `equals()` receives two views with different `word_count()` values
- **THEN** it MUST return `false`
- **AND** it MUST NOT treat the shorter view as a sufficient prefix match
