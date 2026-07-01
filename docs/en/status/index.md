# Status

Status is where BitCal drops all rhetorical excess and states the redesign in present tense.

## Release posture

- BitCal is in an active **vNext / 4.0.0-scale redesign**, not in a settled long-term maintenance phase.
- The public baseline is **C++23**.
- The primary optimization and measurement posture is **x86-64 first**.
- The public model being documented centers on `bit_block<Bits>`, `bit_view`, `const_bit_view`, and free algorithms.
- The legacy monolithic API shape is no longer shipped as part of the retained vNext public surface.
- No code-level compatibility layer is promised for the earlier monolithic API shape.

## Support matrix

| Scope | Current posture | What readers may infer |
| --- | --- | --- |
| Linux x86-64 (GCC / Clang) | Primary | Main build, correctness, and benchmark attention. |
| Windows x86-64 (MSVC) | Primary | Main supported target alongside Linux x86-64. |
| ARM64 and macOS follow-up targets | Secondary | Buildability may exist, but performance rows stay blank until a retained benchmark path exists, and maturity claims stay narrower. |
| Domestic CPU SIMD extensions | Research-only | LoongArch LSX/LASX, Zhaoxin-specific, Kunpeng-specific, and Phytium-specific acceleration need separate OpenSpec, tests, and benchmark evidence before they become support claims. |
| CUDA, GPU, and NPU acceleration | Unsupported in core | These are not part of the retained header-only core library. CUDA/GPU would require an opt-in extension design, and NPU is not a fit for the current bitset-style public algorithms. |
| Anything beyond retained validation | Out of claim | Do not promote to support promises without new evidence. |

## Evidence levels

BitCal uses separate words for separate evidence:

| Level | Meaning |
| --- | --- |
| Buildable | A retained CI lane or documented command compiles the code for the target. |
| Correct | Retained tests execute on that target or an equivalent runner. |
| SIMD-accelerated | The retained public algorithm path uses a target-specific SIMD implementation. |
| Retained benchmark evidence | Structured benchmark artifacts exist for the target, backend, and commit. |

The current x86 SIMD backend is selected by compile-target feature macros such as AVX2 and AVX-512. It is not runtime CPU-feature dispatch for one universal binary.

## Documentation truth

The main sources of truth remain:

- active requirements and deltas in `openspec/`;
- documentation architecture policy in `docs/README.md`;
- the stable public include seam `<bitcal/bitcal.hpp>`;
- version semantics defined from the repository source of truth.

## Change discipline

BitCal expects these surfaces to move together:

- OpenSpec and status language;
- reference wording and public examples;
- benchmark methodology and benchmark claims;
- implementation work and the support matrix.

## What to read next

- Go back to the [Guide](/en/guide/) if you need the review path.
- Revisit the [Whitepaper](/en/whitepaper/) for architecture rationale.
- Use [Performance](/en/performance/) and [Research](/en/research/) before turning isolated evidence into a broader conclusion.
