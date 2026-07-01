## ADDED Requirements

### Requirement: Platform claims SHALL use explicit evidence levels
BitCal SHALL distinguish buildability, correctness, SIMD acceleration, and retained benchmark evidence when describing platform support.

#### Scenario: Maintainers describe x86-64 support
- **WHEN** x86-64 support is documented
- **THEN** the documentation MUST state whether the claim refers to buildability, correctness, SIMD acceleration, or retained benchmark evidence
- **AND** compile-target ISA optimization MUST NOT be described as runtime CPU-feature dispatch

### Requirement: Accelerator support SHALL remain out of scope until separately specified
BitCal SHALL NOT claim CUDA, GPU, or NPU acceleration unless a future OpenSpec change defines the API, implementation, and retained validation path.

#### Scenario: Readers evaluate accelerator support
- **WHEN** CUDA, GPU, or NPU acceleration is discussed
- **THEN** the current product contract MUST describe it as unsupported in the retained core library
- **AND** NPU acceleration MUST NOT be implied by CPU SIMD support

