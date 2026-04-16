# SIMD Dispatch Mechanism

## Compile-time Dispatch

BitCal uses C++17 `if constexpr` to select the optimal implementation path at compile time. This dispatch mechanism has **zero runtime overhead**.

## Binary Operation Dispatch

v2.1 unified `bit_and`/`bit_or`/`bit_xor`/`bit_andnot` into a single template dispatch:

```cpp
enum class binop { op_and, op_or, op_xor, op_andnot };

template<binop Op>
static void dispatch_binop(const bitarray& a, const bitarray& b, bitarray& out) {
    if constexpr (Bits == 64) {
        // Scalar 64-bit operation
        if constexpr (Op == binop::op_and) {
            out.data_[0] = a.data_[0] & b.data_[0];
        }
        // ... other operations
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        // AVX2 256-bit operation
        if constexpr (Op == binop::op_and) {
            avx::bit_and_256(a.data_, b.data_, out.data_);
        }
        // ... other operations
    }
    // ... other backends
    else {
        // Scalar array fallback
        for (size_t i = 0; i < u64_count; ++i) {
            if constexpr (Op == binop::op_and) {
                out.data_[i] = a.data_[i] & b.data_[i];
            }
            // ... other operations
        }
    }
}
```

## Shift Operation Dispatch

Shift operations dispatch internally within `shift_left()` / `shift_right()`:

```cpp
void shift_left(int count) {
    if constexpr (Bits == 64) {
        // Scalar
        data_[0] <<= count;
    }
    else if constexpr (Backend == simd_backend::avx2 && Bits == 256) {
        avx::shift_left_256(data_, count);
    }
    // ...
}
```

## Shift Implementation Strategy

All SIMD shift operations execute in two phases:

1. **Word-shift** (count ≥ 64): Scalar whole-word movement
2. **Bit-shift** (count < 64): SIMD parallel shift + carry propagation

### AVX2 256-bit Left Shift Example

```
Phase 1: word-shift (scalar)
  data[3] = data[2], data[2] = data[1], data[1] = data[0], data[0] = 0

Phase 2: bit-shift (AVX2)
  shifted = _mm256_slli_epi64(v, count)           # Each qword shifts independently
  carry   = _mm256_permute4x64_epi64(v, 0x93)    # Rotate: [q3,q2,q1,q0] -> [q0,q3,q2,q1]
  carry   = _mm256_srli_epi64(carry, 64-count)   # Extract carry bits
  carry   = _mm256_blend_epi32(carry, zero, 0x03) # Clear carry in lowest qword
  result  = shifted | carry
```

## Why Not `_mm256_slli_si256`?

`_mm256_slli_si256` operates **independently on 128-bit lanes** and does not shift across lanes. This is a common AVX2 pitfall.

```cpp
// WRONG: Doesn't work as expected
__m256i shifted = _mm256_slli_si256(v, count);  // Lane-boundary issue

// CORRECT: BitCal's approach
// Phase 1: Move whole words across lanes (scalar)
// Phase 2: Shift within lanes + carry propagation (SIMD)
```

BitCal v2.1 fixed this bug by using the two-phase approach.

## Backend-Specific Implementation Files

### scalar_ops.hpp

Portable fallback using standard C++:

```cpp
namespace bitcal::scalar {
    inline void bit_and_256(uint64_t* out, const uint64_t* a, const uint64_t* b) {
        for (size_t i = 0; i < 4; ++i) {
            out[i] = a[i] & b[i];
        }
    }
    // ... other operations
}
```

### avx_ops.hpp

AVX2 implementations:

```cpp
namespace bitcal::avx {
    inline void bit_and_256(uint64_t* out, const uint64_t* a, const uint64_t* b) {
        __m256i va = _mm256_load_si256((__m256i*)a);
        __m256i vb = _mm256_load_si256((__m256i*)b);
        __m256i vr = _mm256_and_si256(va, vb);
        _mm256_store_si256((__m256i*)out, vr);
    }
    // ... other operations
}
```

### neon_ops.hpp

ARM NEON implementations:

```cpp
namespace bitcal::neon {
    inline void bit_and_256(uint64_t* out, const uint64_t* a, const uint64_t* b) {
        uint64x2_t va0 = vld1q_u64(a);
        uint64x2_t va1 = vld1q_u64(a + 2);
        uint64x2_t vb0 = vld1q_u64(b);
        uint64x2_t vb1 = vld1q_u64(b + 2);
        vst1q_u64(out, vandq_u64(va0, vb0));
        vst1q_u64(out + 2, vandq_u64(va1, vb1));
    }
    // ... other operations
}
```

## Register Pressure

The dispatch mechanism is designed to minimize register pressure:
- Small widths (64-bit): Use scalar registers
- Medium widths (128/256-bit): Use single SIMD registers
- Large widths (512/1024-bit): Split into multiple SIMD operations or use scalar loops

## Compilation Flow

1. **Template instantiation**: `bitarray<256, avx2>` instantiates with known `Bits` and `Backend`
2. **Dispatch elimination**: `if constexpr` selects the active branch, others are discarded
3. **Inlining**: `BITCAL_FORCEINLINE` propagates through call chain
4. **Code generation**: Compiler generates optimal machine code for specific width/backend

## Debugging Dispatch

Check which backend is being used:

```cpp
#include <iostream>

int main() {
    // Check default backend
    std::cout << "Default backend: " 
              << static_cast<int>(bitcal::get_default_backend()) << std::endl;
    
    // Check bitarray's backend
    bitcal::bit256 arr;
    std::cout << "Array backend: " 
              << static_cast<int>(decltype(arr)::backend) << std::endl;
    
    return 0;
}
```

## Future Improvements

- AVX-512 support for 512-bit operations
- SVE/SVE2 support for ARM
- Dynamic dispatch for runtime CPU feature detection
