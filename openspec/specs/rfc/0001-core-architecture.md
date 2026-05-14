# RFC 0001: Core Architecture - BitCal vNext Block/View/Algorithm Model

## Status

**Accepted** - revised to describe the BitCal vNext redesign planned for v4.0.0.

## Context

BitCal v3.0.0 intentionally contracted the public seam around `bitarray`, but the next-generation direction has changed:
- the language baseline moves to C++23
- the primary optimization and validation target moves to x86-64
- the public model is no longer centered on a monolithic member-function type
- the project no longer wants to preserve code-level compatibility with the older public API

The vNext architecture therefore needs a new public/internal split that favors explicit ownership, views, and algorithm composition.

## Decision

BitCal vNext uses a block/view/algorithm architecture with a single stable public include:

```cpp
template<std::size_t Bits>
class bit_block;

class bit_view;
class const_bit_view;
```

### Architecture Layers

```text
┌─────────────────────────────────────────────────────────────┐
│ Stable Public Include Layer                                │
│  bitcal/bitcal.hpp                                         │
├─────────────────────────────────────────────────────────────┤
│ Public Owning Storage Layer                                │
│  bit_block<Bits>                                           │
├─────────────────────────────────────────────────────────────┤
│ Public Non-Owning View Layer                               │
│  bit_view / const_bit_view                                 │
├─────────────────────────────────────────────────────────────┤
│ Public Algorithm Layer                                     │
│  free algorithms operating on blocks / views               │
├─────────────────────────────────────────────────────────────┤
│ Internal Backend / Dispatch Layer                          │
│  detail/backend.hpp, detail/x64_dispatch.hpp               │
├─────────────────────────────────────────────────────────────┤
│ Hardware Instruction Layer                                 │
│  Scalar / SSE2 / AVX2 / AVX-512 on x86-64                  │
└─────────────────────────────────────────────────────────────┘
```

### File Structure

```text
include/bitcal/
├── bitcal.hpp             # only stable public include
├── bit_block.hpp          # owning block contract
├── bit_view.hpp           # non-owning views
├── algorithms.hpp         # public free algorithms
└── detail/
    ├── backend.hpp        # backend tags / shared contracts
    └── x64_dispatch.hpp   # x86-64-first dispatch
```

Notes:
- `bitcal.hpp` remains the only stable public include seam.
- `bit_block.hpp`, `bit_view.hpp`, and `algorithms.hpp` define the public model but are not separate include promises.
- `detail/` is internal and must not become part of the compatibility boundary.
- x86-64 is the primary backend target; other architectures are secondary and do not define the core optimization contract.

## Key Design Decisions

### 1. Separate ownership from algorithms

`bit_block<Bits>` owns contiguous words. Algorithms do not require callers to route everything through one member-heavy type.

**Rationale:**
- makes ownership and algorithm composition explicit
- makes non-owning interop possible without exposing internal helper APIs
- gives the implementation freedom to specialize algorithms independently from storage

### 2. Views are first-class public types

`bit_view` and `const_bit_view` are public, not merely internal helpers.

**Rationale:**
- callers need a stable way to apply public algorithms to existing storage
- views reduce copying and make interoperability clearer
- view semantics fit naturally with C++23 and contiguous-word processing

### 3. Public algorithms replace the older monolithic member surface

Bitwise operations, queries, shifts, and counting logic are modeled as public algorithms over public blocks/views.

**Rationale:**
- reduces coupling between API shape and backend implementation
- allows different storage and execution strategies without turning internals into public members
- keeps the contract centered on observable behavior

### 4. x86-64-first dispatch is an explicit architecture choice

The primary backend contract is `scalar`, `sse2`, `avx2`, and `avx512`.

**Rationale:**
- matches the stated product target of extreme x86-64 performance
- prevents documentation and CI from over-claiming uniform support
- keeps optimization work focused on the highest-value matrix

### 5. No public compatibility layer

The redesign does not preserve a code-level compatibility shim for older `bitarray` callers.

**Rationale:**
- avoids dual public models
- keeps the benchmark and optimization surface clean
- makes the new architecture the only target for follow-up work

## Consequences

### Positive
- clearer architectural boundaries between storage, views, algorithms, and backend dispatch
- better fit for C++23-era API design
- more focused x86-64 optimization and validation story

### Trade-offs
- migration cost increases because callers must move to a new model
- short-term spec/implementation drift will exist until the public skeleton lands
- non-primary platforms receive weaker promises than before

## Migration Impact

Consumers should plan for the following 3.x → 4.0.0 changes:
- continue including through `<bitcal/bitcal.hpp>` only
- replace `bitarray`-centered usage with the new block/view/algorithm model
- do not expect code-level compatibility shims for the older API

## Alternatives Considered

### Alternative 1: Keep extending the contracted `bitarray` public model
- **Rejected:** it preserves the older architecture as the center of gravity and makes the redesign less coherent.

### Alternative 2: Add a new API while preserving the old one indefinitely
- **Rejected:** dual public models would increase maintenance and blur benchmark correctness.

### Alternative 3: Keep a broad cross-platform optimization target
- **Rejected:** it conflicts with the explicit x86-64-first performance goal and dilutes retained validation effort.
