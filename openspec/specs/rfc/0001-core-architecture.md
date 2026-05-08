# RFC 0001: Core Architecture - SIMD-Accelerated Bit Array

## Status

**Accepted** - revised to describe the retained public-surface contraction planned for v3.0.0.

## Context

BitCal v2.x evolved into a header-only template library, but the documented architecture drifted away from the repository:
- the implementation no longer uses `simd_traits.hpp`
- backend dispatch is centralized in `backend_ops.hpp`
- the documented backend list still mentions a public `AVX` transitional backend that the implementation no longer exposes
- `bitcal.hpp` currently mixes retained public API and removable helper surface in a single header

The v3.0.0 contraction keeps the compile-time SIMD architecture, but narrows the public seam and clarifies the internal layering.

## Decision

BitCal retains a compile-time `bitarray` architecture with a contracted public seam:

```cpp
template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray;
```

### Architecture Layers

```text
┌─────────────────────────────────────────────────────────────┐
│ Stable Public Include Layer                                │
│  bitcal/bitcal.hpp                                         │
├─────────────────────────────────────────────────────────────┤
│ Retained Public Type Layer                                 │
│  bitarray<Bits, Backend>, stable aliases, retained methods │
│  (planned physical home: include/bitcal/bitarray.hpp)      │
├─────────────────────────────────────────────────────────────┤
│ Backend Selection / Dispatch Layer                         │
│  config.hpp, backend_ops.hpp, scalar_ops.hpp               │
├─────────────────────────────────────────────────────────────┤
│ Hardware Instruction Layer                                 │
│  Scalar / SSE2 / AVX2 / AVX-512 / NEON                     │
└─────────────────────────────────────────────────────────────┘
```

### File Structure (contract target)

```text
include/bitcal/
├── bitcal.hpp        # only stable public include
├── bitarray.hpp      # retained bitarray contract definition
├── config.hpp        # backend enum, default selection, alignment helpers
├── backend_ops.hpp   # internal backend dispatch implementations
└── scalar_ops.hpp    # scalar helpers and fallbacks
```

Notes:
- `bitarray.hpp` is part of the retained implementation layering, but the stable include seam remains `bitcal.hpp`.
- `backend_ops.hpp`, `scalar_ops.hpp`, and `config.hpp` may be described architecturally without becoming separate stable public include promises.
- `simd_traits.hpp` is not part of the retained architecture description.

## Key Design Decisions

### 1. Template-based fixed-width type

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
    // fixed-width contiguous word storage
};
```

**Rationale:**
- preserves compile-time optimization and zero-overhead dispatch
- keeps width and backend encoded in the type
- supports a small, explicit retained public API surface

### 2. Compile-time backend selection

The retained backend selection order is:

`avx512 -> avx2 -> sse2 -> neon -> scalar`

There is no separate retained `simd_backend::avx` contract.

**Rationale:**
- matches the actual enum exposed by `config.hpp`
- avoids documenting non-existent or transitional backend states
- keeps public claims aligned with implementation and tests

### 3. Contracted public seam

- `bitcal/bitcal.hpp` remains the only stable public include.
- The retained `bitarray` API is separated from removable helper APIs.
- Helper namespaces, traits, and undocumented convenience methods no longer define the architectural contract.

**Rationale:**
- narrows the compatibility boundary
- allows future internal refactors without expanding the public include promise
- keeps API docs consistent with the retained product posture

### 4. Storage contract

The retained architecture promises contiguous `uint64_t` word storage with backend-appropriate alignment, not universal 64-byte alignment for every instantiation.

**Rationale:**
- reflects the current width-sensitive alignment logic
- avoids over-promising memory layout details the implementation does not guarantee uniformly

## Consequences

### Positive
- Smaller and clearer retained public surface
- Architecture docs match current backend naming and file responsibilities
- Easier to keep API, tests, and docs synchronized around one public seam

### Trade-offs
- 3.0.0 becomes an explicit breaking release
- Existing consumers of helper APIs or direct internal-header includes need migration
- The temporary implementation may lag behind the contracted spec until follow-up tasks complete

## Migration Impact

Consumers should plan for the following 2.x → 3.0.0 changes:
- include through `<bitcal/bitcal.hpp>` only
- rely on retained `bitarray` methods and stable aliases
- stop depending on `bitcal::ops`, public type traits, or undocumented convenience helpers unless a future spec re-adds them

## Alternatives Considered

### Alternative 1: Keep the current umbrella header as a broad compatibility bucket
- **Rejected:** it preserves drift and forces BitCal to support helper APIs that were never intentionally stabilized.

### Alternative 2: Make every header under `include/bitcal/` a stable public seam
- **Rejected:** that would increase compatibility burden and block internal cleanup in a header-only library.

### Alternative 3: Delay spec updates until after code refactors
- **Rejected:** later tasks need a frozen contract first, otherwise tests, docs, and implementation would continue to drift.
