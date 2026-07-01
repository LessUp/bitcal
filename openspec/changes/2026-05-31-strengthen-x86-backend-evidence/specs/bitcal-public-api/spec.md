## ADDED Requirements

### Requirement: Public query algorithms SHALL preserve scalar constexpr behavior
BitCal SHALL keep public query and counting algorithms usable through the retained public model while allowing runtime implementations to use internal x86 backend helpers.

#### Scenario: Public algorithms run at compile time
- **WHEN** `popcount`, `is_zero`, or `equals` is evaluated in a constant-evaluation context
- **THEN** the result MUST be computed by portable scalar word logic
- **AND** no x86 intrinsic backend is required for constant evaluation

#### Scenario: Public algorithms run at normal runtime
- **WHEN** `popcount`, `is_zero`, or `equals` is called at runtime
- **THEN** the implementation MAY route through the internal x86 dispatch seam
- **AND** this routing MUST NOT add new public API surface

