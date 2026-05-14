## ADDED Requirements

### Requirement: BitCal vNext SHALL target a C++23 x86-64-first redesign
BitCal SHALL treat C++23 as the minimum language baseline for vNext and SHALL optimize its primary support posture for Linux and Windows x86-64 toolchains.

#### Scenario: Product positioning is updated for vNext
- **WHEN** maintainers describe the next major BitCal release
- **THEN** they MUST describe it as a C++23 redesign with x86-64 as the primary optimization and validation target
- **AND** they MUST NOT describe the old C++17 retained contract as the target architecture for vNext

### Requirement: BitCal vNext SHALL adopt a new public model without compatibility shims
BitCal SHALL define its next public contract around `bit_block`, `bit_view` / `const_bit_view`, and public algorithms, and SHALL NOT preserve code-level compatibility shims for the older `bitarray`-centered public model.

#### Scenario: Public product boundaries are described
- **WHEN** product-facing materials describe the vNext API
- **THEN** they MUST describe the new owning block, non-owning view, and free algorithm model
- **AND** they MUST treat old API migration as documentation work, not as a code-level compatibility promise
