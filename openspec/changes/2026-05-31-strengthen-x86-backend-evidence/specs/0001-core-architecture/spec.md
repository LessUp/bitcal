## ADDED Requirements

### Requirement: The x86 backend seam SHALL be compile-target ISA based in this phase
BitCal SHALL strengthen the x86 backend seam without claiming runtime CPU-feature dispatch.

#### Scenario: Maintainers review x86 backend architecture
- **WHEN** AVX2 or AVX-512 paths are enabled
- **THEN** they MUST be guarded by compiler feature macros for the compile target
- **AND** the architecture documentation MUST NOT imply that one binary automatically selects among scalar, AVX2, and AVX-512 at runtime

### Requirement: Deferred backends SHALL stay outside the retained core contract
BitCal SHALL keep ARM SIMD, domestic CPU SIMD, CUDA/GPU, and NPU acceleration outside the retained core contract until future changes add evidence paths.

#### Scenario: A future backend is proposed
- **WHEN** maintainers want to add ARM SVE/SVE2, LoongArch LSX/LASX, CUDA, GPU, or NPU acceleration
- **THEN** they MUST create a separate OpenSpec change
- **AND** that change MUST define implementation, tests, benchmarks, and documentation before support claims are upgraded

