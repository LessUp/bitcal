# Status

Status is where BitCal drops all rhetorical excess and states the redesign in present tense.

## Release posture

- BitCal is in an active **vNext / 4.0.0-scale redesign**, not in a settled long-term maintenance phase.
- The public baseline is **C++23**.
- The primary optimization and measurement posture is **x86-64 first**.
- The public model being documented centers on `bit_block<Bits>`, `bit_view`, `const_bit_view`, and free algorithms.
- No code-level compatibility layer is promised for the earlier monolithic API shape.

## Support matrix

| Scope | Current posture | What readers may infer |
| --- | --- | --- |
| Linux x86-64 (GCC / Clang) | Primary | Main build, correctness, and benchmark attention. |
| Windows x86-64 (MSVC) | Primary | Main supported target alongside Linux x86-64. |
| ARM64 and macOS follow-up targets | Secondary | Buildability may exist, but performance rows stay blank until a retained benchmark path exists, and maturity claims stay narrower. |
| Anything beyond retained validation | Out of claim | Do not promote to support promises without new evidence. |

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
