# Architecture Overview

This document describes the design principles and implementation details of BitCal.

## Table of Contents

- [Layered Design](#layered-design)
- [File Structure](#file-structure)
- [Design Principles](#design-principles)
- [SIMD Dispatch](#simd-dispatch-mechanism)
- [Performance Characteristics](#performance-characteristics)
- [Thread Safety](#thread-safety)
- [Memory Usage](#memory-usage)

---

## Layered Design

BitCal follows a layered architecture that separates user interface from implementation details:

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

---

## File Structure

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

**Key Files:**

| File | Purpose | Lines (Approx) |
|------|---------|----------------|
| `config.hpp` | Compiler/platform detection | ~150 |
| `simd_traits.hpp` | Type traits for SIMD backends | ~200 |
| `scalar_ops.hpp` | Fallback scalar implementations | ~300 |
| `sse_ops.hpp` | SSE2 optimized operations | ~400 |
| `avx_ops.hpp` | AVX2 optimized operations | ~450 |
| `neon_ops.hpp` | NEON optimized operations | ~400 |
| `bitcal.hpp` | Main API (bitarray class) | ~600 |

---

## Design Principles

### 1. Header-Only Library

No compilation required. Users simply:

```cpp
#include <bitcal/bitcal.hpp>
```

**Benefits:**
- Zero link-time dependencies
- Easy integration (copy headers)
- Optimal inlining across translation units

### 2. Compile-Time Dispatch

All backend selection happens at compile time:

```cpp
template<binop Op, size_t Bits, simd_backend Backend>
static void dispatch_binop(const uint64_t* a, const uint64_t* b, uint64_t* out) {
    if constexpr (Bits == 64) {
        // Scalar 64-bit operation (most efficient)
        out[0] = apply_op<Op>(a[0], b[0]);
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        // AVX2 256-bit operation
        avx2::binop<Op>(a, b, out);
    }
    else if constexpr (Backend == simd_backend::neon && Bits >= 128) {
        // NEON 128-bit operations (loop for larger widths)
        neon::binop_loop<Op, Bits>(a, b, out);
    }
    else {
        // Scalar fallback for all other cases
        scalar::binop_loop<Op, Bits>(a, b, out);
    }
}
```

### 3. Zero-Overhead Abstraction

Critical paths use forced inlining:

```cpp
#define BITCAL_FORCEINLINE inline __attribute__((always_inline))

class bitarray {
    BITCAL_FORCEINLINE bitarray operator&(const bitarray& other) const {
        bitarray result;
        dispatch_binop<binop::AND>(*this, other, result);
        return result;
    }
};
```

After optimization, `a & b` compiles to:
```asm
; With AVX2, bit256
vmovdqu ymm0, [rdi]
vmovdqu ymm1, [rdi+32]
vpand   ymm0, ymm0, [rsi]
vpand   ymm1, ymm1, [rsi+32]
vmovdqu [rdx], ymm0
vmovdqu [rdx+32], ymm1
ret
```

### 4. Graceful Degradation

Unsupported combinations automatically fall back to scalar:

```cpp
// On a system without AVX2, this still works:
bitcal::bitarray<256, bitcal::simd_backend::avx2> arr;
// Falls back to scalar with no runtime error
```

### 5. Memory Alignment

All internal storage is 64-byte aligned:

```cpp
template<size_t Bits, simd_backend Backend>
class bitarray {
    alignas(64) uint64_t data_[Bits / 64];
};
```

**Why 64 bytes?**
- Matches x86 cache line size
- Required for AVX-512 (if supported)
- Optimal for DMA and GPU interop

---

## SIMD Dispatch Mechanism

### Backend Selection Flow

```
Compile with -march=native
         │
         ▼
┌─────────────────────────────────┐
│  Compiler defines __AVX2__ etc  │
└─────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│ config.hpp sets BITCAL_HAS_*    │
└─────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│ select_best_backend<Bits>()     │
│ picks optimal for bit width     │
└─────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────┐
│ bitarray uses selected backend  │
│ for all operations              │
└─────────────────────────────────┘
```

### Backend Priority

```cpp
constexpr simd_backend get_default_backend() {
    #if defined(BITCAL_HAS_AVX2)
        return simd_backend::avx2;      // Highest performance on x86
    #elif defined(BITCAL_HAS_SSE2)
        return simd_backend::sse2;      // Universal x86 support
    #elif defined(BITCAL_HAS_NEON)
        return simd_backend::neon;      // ARM support
    #else
        return simd_backend::scalar;    // Universal fallback
    #endif
}
```

---

## Performance Characteristics

### Operation Speedups

| Operation | Scalar | SSE2 | AVX2 | NEON | Notes |
|-----------|--------|------|------|------|-------|
| AND/OR/XOR | 1.0× | ~2.0× | ~5.9× | ~2.6× | Memory-bound at large widths |
| NOT | 1.0× | ~1.9× | ~1.7× | ~2.0× | XOR with all-ones |
| Shift | 1.0× | ~2.3× | ~3.6× | ~1.8× | Cross-lane carry handling |
| is_zero | 1.0× | ~2.1× | ~2.5× | ~2.2× | Movemask/testz optimization |
| popcount | 1.0× | 1.0× | 2.0× | 1.0× | Uses `popcnt` instruction |

### Scaling Behavior

| Width | Optimal Backend | Relative Performance |
|-------|-----------------|---------------------|
| 64-bit | Scalar | Baseline |
| 128-bit | SSE2/NEON | ~2× |
| 256-bit | AVX2 | ~6× |
| 512-bit | AVX2 (2 ops) | ~6× |
| 1024-bit | AVX2 (4 ops) | ~6× |

---

## Thread Safety

### Thread-Safe Operations

- Different threads operating on **different** `bitarray` instances
- **Read-only** operations on shared instances

### Not Thread-Safe

- Concurrent read/write on the **same** instance without synchronization

**Example:**
```cpp
bitcal::bit256 shared;

// Thread 1 (unsafe without mutex)
shared |= other;

// Thread 2 (unsafe without mutex)
shared.set_bit(42, true);
```

**Solution:**
```cpp
std::mutex mtx;
bitcal::bit256 shared;

// Thread 1
{
    std::lock_guard<std::mutex> lock(mtx);
    shared |= other;
}

// Thread 2
{
    std::lock_guard<std::mutex> lock(mtx);
    shared.set_bit(42, true);
}
```

---

## Memory Usage

### Storage Requirements

| Type | User Data | Alignment Padding | Total |
|------|-----------|-------------------|-------|
| bit64 | 8 bytes | 56 bytes | 64 bytes |
| bit128 | 16 bytes | 48 bytes | 64 bytes |
| bit256 | 32 bytes | 32 bytes | 64 bytes |
| bit512 | 64 bytes | 0 bytes | 64 bytes |
| bit1024 | 128 bytes | 0 bytes | 128 bytes |

### Cache Efficiency

```cpp
// Fits in one cache line (optimal)
bitcal::bit512 a, b, c;
c = a & b;  // 3 × 64-byte = 192 bytes (3 cache lines)

// Slightly larger
bitcal::bit1024 x, y, z;
z = x & y;  // 3 × 128-byte = 384 bytes (6 cache lines)
```

### Stack Usage Warning

Very large bit widths may cause stack overflow:

```cpp
// Risk of stack overflow in deeply recursive functions
void recursive_function() {
    bitcal::bitarray<65536> huge;  // 8KB on stack
    // ...
    recursive_function();  // Stack grows
}

// Solution: Use heap allocation for very large widths
auto huge = std::make_unique<bitcal::bitarray<65536>>();
```

---

## See Also

- [SIMD Dispatch](simd-dispatch.html) — Detailed dispatch mechanism
- [Platform Support](platform-support.html) — Supported platforms and compilers
