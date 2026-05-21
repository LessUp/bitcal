## ADDED Requirements

### Requirement: Retained validation SHALL not include legacy public-surface assets
BitCal SHALL remove legacy public-surface assets from the retained validation path once the vNext replacement surface exists.

#### Scenario: Maintainers review retained tests, examples, and benchmarks
- **WHEN** a retained target is built or documented as part of the validation path
- **THEN** it MUST compile and run without `bitarray`
- **AND** `benchmark_compare_legacy` or similar built-in legacy-lane executables MUST NOT remain part of the retained tree

### Requirement: Public TDD coverage SHALL drive vNext algorithm completion
BitCal SHALL use public contract tests through `<bitcal/bitcal.hpp>` to prove the missing free algorithms exist before claiming the legacy surface is fully removed.

#### Scenario: A vNext replacement algorithm is added
- **WHEN** a retained free algorithm such as `bit_or` or `shift_left` is implemented
- **THEN** a public test through `<bitcal/bitcal.hpp>` MUST fail first and then pass
- **AND** the algorithm MUST become part of the retained redesign verification path
