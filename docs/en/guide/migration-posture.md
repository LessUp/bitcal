# Migration Posture

BitCal vNext is not trying to preserve the earlier public model with a cosmetic refresh. It is replacing the center of the contract.

## Why vNext breaks old assumptions

The earlier `bitarray`-centric shape bundled ownership, borrowing, and behavior too tightly. That makes it harder to document the contract clearly and harder to change dispatch or kernels without creating public API drag.

The redesign therefore chooses:

- `bit_block<Bits>` for explicit owning storage;
- `bit_view` and `const_bit_view` for borrowing existing storage;
- free algorithms for the behavioral center.

## What is still stable

Some things are intentionally held steady even during the redesign:

- `<bitcal/bitcal.hpp>` remains the only stable include seam;
- CMake-based local verification remains the retained review path;
- documentation, tests, benchmarks, and OpenSpec are expected to move together.

## What is intentionally unstable

Readers should not assume that the library currently promises:

- a code-level compatibility layer for the old API;
- equal optimization maturity across every architecture;
- performance claims beyond the retained x86-64 baseline and benchmark method.

## How to review migration risk

Review BitCal as a forward-looking contract, not as a compatibility story. Ask three questions:

1. Is the owner / view / algorithm split easier to reason about than the old center?
2. Does the dispatch boundary protect the public contract from backend churn?
3. Are the benchmark and status pages careful enough to keep migration promises honest?

If the answer to those questions is yes, the redesign is doing useful work even before every kernel family is finalized.
