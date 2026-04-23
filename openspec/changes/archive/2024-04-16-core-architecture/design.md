# Design: Core Architecture - SIMD-Accelerated Bit Array

## Context

BitCal v1.x used runtime dispatch with virtual functions, incurring overhead on every operation. Modern C++17 provides template-based compile-time dispatch that eliminates this overhead.

## Decision

### Architecture Layers

```
┌─────────────────────────────────────────────────────────────┐
│                    User API Layer                           │
│  bitarray<N>, Operators, Type Aliases (bit64, bit256, etc.) │
├─────────────────────────────────────────────────────────────┤
│                 Compile-Time Dispatch Layer                 │
│  if constexpr, Traits, Backend Selection                    │
├─────────────────────────────────────────────────────────────┤
│                   SIMD Implementation Layer                 │
│  ├── scalar_ops.hpp  (Portable fallback)                    │
│  ├── sse_ops.hpp     (x86 SSE2 128-bit)                     │
│  ├── avx_ops.hpp     (x86 AVX2 256-bit)                     │
│  └── neon_ops.hpp    (ARM NEON 128-bit)                     │
├─────────────────────────────────────────────────────────────┤
│                   Hardware Instruction Layer                │
│  SSE2 / AVX / AVX2 / AVX-512 / NEON / Scalar                │
└─────────────────────────────────────────────────────────────┘
```

### Template-Based Design

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
    alignas(64) uint64_t data_[Bits / 64];
    // ...
};
```

### Compile-Time Backend Selection

```cpp
template<size_t Bits>
constexpr simd_backend select_best_backend() {
    #if defined(BITCAL_HAS_AVX2) && Bits >= 256
        return simd_backend::avx2;
    #elif defined(BITCAL_HAS_SSE2) && Bits >= 128
        return simd_backend::sse2;
    #elif defined(BITCAL_HAS_NEON) && Bits >= 128
        return simd_backend::neon;
    #else
        return simd_backend::scalar;
    #endif
}
```

### Memory Layout

- 64-byte alignment (cache-line aligned)
- Contiguous `uint64_t` storage
- Little-endian ordering

### Forced Inlining

All critical paths use `BITCAL_FORCEINLINE` to eliminate function call overhead.

## Implementation

### File Structure

```
include/bitcal/
├── config.hpp              # Platform detection macros
├── simd_traits.hpp         # SIMD type traits
├── scalar_ops.hpp          # Portable scalar implementation
├── sse_ops.hpp             # x86 SSE2 operations
├── avx_ops.hpp             # x86 AVX2 operations
├── neon_ops.hpp            # ARM NEON operations
└── bitcal.hpp              # Main header
```

## Alternatives Considered

### Runtime Feature Detection
- **Rejected:** Adds runtime overhead

### Policy-Based Design
- **Rejected:** More complex for users

### Separate Types Per Backend
- **Rejected:** Poor user experience, combinatorial explosion

## Risks

- Template instantiation may increase compile time
- Multiple template instantiations may increase binary size
- Users need to migrate from v1.x API
