# Architecture Overview

This document describes the retained BitCal 3.0 architecture and public-surface boundaries.

> **Public contract note:** BitCal 3.0 exposes `bitarray`, the predefined aliases, the backend enum, and the documented member/operator API. Removed raw-pointer helpers and trait shims are implementation details or no longer supported.

## Table of Contents

- [Layered Design](#layered-design)
- [File Structure](#file-structure)
- [Design Principles](#design-principles)
- [SIMD Dispatch](#simd-dispatch-mechanism)
- [Performance Characteristics](#performance-characteristics)
- [Thread Safety](#thread-safety)

---

## Layered Design

BitCal keeps a small public surface over backend-specific implementation code:

```
┌─────────────────────────────────────────────────────────────┐
│                    Public API Layer                        │
│  bitarray<N>, predefined aliases, documented operators     │
├─────────────────────────────────────────────────────────────┤
│                 Compile-Time Selection Layer               │
│  Backend tags, width constraints, get_default_backend()    │
├─────────────────────────────────────────────────────────────┤
│                 Backend Dispatch Layer                     │
│  backend_ops.hpp maps operations onto scalar/SSE/AVX/...   │
├─────────────────────────────────────────────────────────────┤
│               ISA-Specific Implementation Layer            │
│  scalar_ops.hpp / sse_ops.hpp / avx_ops.hpp / ...          │
└─────────────────────────────────────────────────────────────┘
```

---

## File Structure

```
include/bitcal/
├── bitcal.hpp              # Public umbrella header
├── bitarray.hpp            # Public bitarray template
├── config.hpp              # Version macros, backend enum, platform detection
├── backend_ops.hpp         # Width/backend dispatch glue
├── scalar_ops.hpp          # Scalar fallback implementation
├── sse_ops.hpp             # x86 SSE2 implementation
├── avx_ops.hpp             # x86 AVX2 implementation
├── avx512_ops.hpp          # x86 AVX-512 implementation
└── neon_ops.hpp            # ARM NEON implementation
```

**Public-facing headers:** `bitcal.hpp`, `bitarray.hpp`, and `config.hpp`.

---

## Design Principles

### 1. Header-Only Delivery

Users consume BitCal through a normal include path:

```cpp
#include <bitcal/bitcal.hpp>
```

### 2. Narrow Public Contract

The supported API is centered on fixed-width value types:

- `bitarray<Bits, Backend>`
- predefined aliases such as `bit256`
- operators, counting functions, and bit helpers documented in `docs/en/api/`

Removed surfaces such as `bitcal::ops` and public traits are intentionally absent from the 3.0 contract.

### 3. Compile-Time Backend Selection

The default template parameter selects a backend at compile time:

```cpp
template<size_t Bits,
         simd_backend Backend = (Bits == 64 ? simd_backend::scalar : get_default_backend())>
class bitarray;
```

### 4. Backend Specialization Behind One Type

`backend_ops.hpp` keeps the public `bitarray` API stable while routing work to backend-specific implementations.

### 5. Width-Aware Alignment

Alignment scales with bit width:

- 64-bit values use 8-byte alignment
- 128-bit values use 16-byte alignment
- 256-bit values use 32-byte alignment
- 512-bit and larger values use 64-byte alignment

---

## SIMD Dispatch Mechanism

### Backend Selection Flow

```
Compiler flags / target ISA
         │
         ▼
config.hpp defines BITCAL_HAS_* macros
         │
         ▼
get_default_backend() chooses avx512 / avx2 / sse2 / neon / scalar
         │
         ▼
bitarray<Bits, Backend> binds that backend at compile time
         │
         ▼
backend_ops.hpp dispatches each operation to the matching implementation
```

### Backend Priority

```cpp
constexpr simd_backend get_default_backend() noexcept {
#if BITCAL_HAS_AVX512
    return simd_backend::avx512;
#elif BITCAL_HAS_AVX2
    return simd_backend::avx2;
#elif BITCAL_HAS_SSE2
    return simd_backend::sse2;
#elif BITCAL_HAS_NEON
    return simd_backend::neon;
#else
    return simd_backend::scalar;
#endif
}
```

---

## Performance Characteristics

| Width | Typical Backend | Notes |
|-------|-----------------|-------|
| 64-bit | Scalar | Single-word fast path |
| 128-bit | SSE2 / NEON / AVX-512 VL | Native 128-bit vector width |
| 256-bit | AVX2 / AVX-512 VL | Common high-throughput path |
| 512-bit | AVX-512 or looped AVX2 | Larger fixed-width workloads |
| 1024-bit | Loop over selected backend | Throughput scales with word count |

---

## Thread Safety

### Safe Without Extra Synchronization

- Different threads operating on different `bitarray` instances
- Read-only access to a shared `bitarray`

### Requires External Synchronization

- Concurrent mutation of the same `bitarray`
- Mixed read/write access to the same instance
