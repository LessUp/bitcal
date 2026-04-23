# RFC 0001: Core Architecture - SIMD-Accelerated Bit Array

## Status

**Accepted** - Implemented in v2.0 and v2.1

## Context

BitCal v1.x used an object-oriented inheritance design with runtime dispatch:

```cpp
// Old design (v1.x)
class BitCalBase { virtual uint64_t bitAnd64(...) = 0; };
class BitCalGpr : public BitCalBase { ... };
class BitCalXmm : public BitCalBase { ... };

// Runtime dispatch with singleton
BitCal::getInstance().bitAnd64(a, b);  // Virtual function overhead
```

This design had several limitations:
- Virtual function call overhead on every operation
- Runtime backend selection
- Difficult to compile for specific SIMD widths
- Required compilation (not header-only)
- Complex inheritance hierarchy

## Decision

Redesign to use C++17 templates with compile-time dispatch:

```cpp
// New design (v2.x)
template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray { ... };

// Compile-time dispatch - zero overhead
bitcal::bit256 a, b;
auto c = a & b;  // Inlined to optimal SIMD instructions
```

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

### File Structure

```
include/bitcal/
├── config.hpp              # Platform detection macros, simd_backend enum
├── simd_traits.hpp         # SIMD register width, alignment requirements
├── scalar_ops.hpp          # Portable scalar implementation (baseline)
├── sse_ops.hpp             # x86 SSE2 128-bit operations
├── avx_ops.hpp             # x86 AVX2 256-bit operations
├── neon_ops.hpp            # ARM NEON 128-bit operations
└── bitcal.hpp              # Main header: aggregates all + bitarray class
```

### Key Design Decisions

#### 1. Template-Based Design

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
    alignas(64) uint64_t data_[Bits / 64];
    // ...
};
```

**Rationale:**
- Enables compile-time optimization
- Type safety through template parameters
- Allows static assertions for validation

#### 2. Compile-Time Backend Selection

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

**Rationale:**
- Zero runtime overhead
- Optimal code generation for target platform
- Graceful fallback when SIMD not available

#### 3. Memory Layout

```
bitarray<256, avx2> memory representation:
┌──────────────────────────────────────────────────────────────────────┐
│ Alignment  │  Word 0   │  Word 1   │  Word 2   │  Word 3   │ Padding │
│  64 bytes  │ bits 0-63 │bits 64-127│bits 128-191│bits 192-255│to 64B  │
└──────────────────────────────────────────────────────────────────────┘
                    Little-endian: word 0 holds LSB
```

**Rationale:**
- 64-byte alignment matches x86 cache line size
- Contiguous storage enables interoperability with C APIs
- Little-endian ordering for consistency across platforms

#### 4. Forced Inlining

```cpp
#define BITCAL_FORCEINLINE inline __attribute__((always_inline))

class bitarray {
    BITCAL_FORCEINLINE bitarray operator&(const bitarray& other) const {
        // ...
    }
};
```

**Rationale:**
- Eliminates function call overhead
- Enables compiler to optimize across abstraction boundaries

## Consequences

### Positive
- **Performance:** 5-6× speedup for 256-bit operations (AVX2)
- **Zero Overhead:** No runtime dispatch cost
- **Header-Only:** No compilation required for use
- **Type Safety:** Compile-time validation of bit widths
- **Simplicity:** Operator overloading provides intuitive API

### Trade-offs
- **Compile Time:** Template instantiation can increase compile time for large projects
- **Binary Size:** Multiple template instantiations may increase binary size
- **AVX-512:** Not yet natively supported (falls back to AVX2)

### Migration Impact

Users must update from v1.x singleton pattern:

```cpp
// Old code (v1.x)
uint64_t data[4];
BitCal::getInstance().bitAnd256(data, other, result);

// New code (v2.x)
bitcal::bit256 data, other;
auto result = data & other;
```

## Alternatives Considered

### Alternative 1: Runtime Feature Detection
- Detect CPU capabilities at runtime and select backend
- **Rejected:** Runtime overhead, complexity

### Alternative 2: Policy-Based Design
- Use policy templates for backend selection
- **Rejected:** More complex for users, marginal benefit

### Alternative 3: Separate Types Per Backend
- `bit256_scalar`, `bit256_sse`, `bit256_avx`
- **Rejected:** Poor user experience, combinatorial explosion

## References

- [Architecture Overview](../../docs/architecture/overview.md)
- [SIMD Dispatch](../../docs/architecture/simd-dispatch.md)
- [Platform Support](../../docs/architecture/platform-support.md)
