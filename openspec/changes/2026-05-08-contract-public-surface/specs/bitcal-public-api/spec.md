## ADDED Requirements

### Requirement: Stable public include SHALL contract to the umbrella header
BitCal SHALL treat `<bitcal/bitcal.hpp>` as the only stable public include path for the retained API in the 3.0.0 contract.

#### Scenario: A consumer includes BitCal headers
- **WHEN** user-facing documentation or samples show how to consume the library
- **THEN** they MUST include `<bitcal/bitcal.hpp>`
- **AND** no other header path may be described as a stable public include contract

### Requirement: Retained public API SHALL exclude helper namespaces and undocumented conveniences
BitCal SHALL retain only the `bitarray`-centric API that remains explicitly documented in OpenSpec, and SHALL remove helper namespaces, traits, and undocumented convenience methods from the public contract for 3.0.0.

#### Scenario: The 3.0.0 retained public surface is defined
- **WHEN** maintainers describe or test the supported public API
- **THEN** `bitcal::ops`, `is_bitarray`, `is_bitarray_v`, `bitarray_traits`, and any convenience method not re-listed in the API spec MUST be treated as removed from the retained public contract
