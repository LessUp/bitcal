## ADDED Requirements

### Requirement: Internal backend tests SHALL cover x86 vector-tail behavior
BitCal SHALL keep internal tests for the retained x86 dispatch seam so vectorized chunks and scalar tails remain correct.

#### Scenario: Internal x86 helper tests run on any retained platform
- **WHEN** detail tests are compiled
- **THEN** tests for direct x86 helpers MUST remain portable through scalar fallback guards
- **AND** they MUST cover word counts below, equal to, and above vector chunk sizes

### Requirement: Unsupported accelerators SHALL not appear in retained validation
BitCal SHALL NOT include CUDA, GPU, or NPU targets in retained validation until those accelerators have their own OpenSpec change.

#### Scenario: Maintainers review retained CI and benchmarks
- **WHEN** the retained validation matrix is evaluated
- **THEN** CUDA, GPU, and NPU lanes MUST NOT be treated as missing failures
- **AND** they MUST NOT be described as supported behavior

