# SIMD Backend

## Backend Enumeration

```cpp
enum class simd_backend {
    scalar,   // Portable scalar fallback (all platforms)
    sse2,     // SSE2 (x86)
    avx,      // AVX (x86)
    avx2,     // AVX2 (x86)
    avx512,   // AVX-512 (x86) — falls back to AVX2
    neon      // NEON (ARM)
};
```

## Automatic Selection

```cpp
constexpr simd_backend get_default_backend();
```

Automatically selects the optimal backend at compile time based on compiler flags.

**Selection Priority**:
1. AVX-512 → AVX2 (detected but falls back to AVX2)
2. AVX2
3. AVX
4. SSE2
5. NEON
6. Scalar

## Manual Specification

Force a specific backend:

```cpp
// Force AVX2
bitcal::bitarray<256, bitcal::simd_backend::avx2> a;

// Force NEON
bitcal::bitarray<128, bitcal::simd_backend::neon> b;

// Force scalar (maximum portability)
bitcal::bitarray<256, bitcal::simd_backend::scalar> c;
```

## Runtime Query

```cpp
#include <iostream>

std::string backend_name(bitcal::simd_backend b) {
    switch (b) {
        case bitcal::simd_backend::avx2:   return "AVX2";
        case bitcal::simd_backend::avx:    return "AVX";
        case bitcal::simd_backend::sse2:   return "SSE2";
        case bitcal::simd_backend::neon:   return "NEON";
        case bitcal::simd_backend::avx512: return "AVX-512";
        default: return "scalar";
    }
}

int main() {
    std::cout << "Backend: " << backend_name(bitcal::get_default_backend()) << std::endl;
}
```

## Backend Support Matrix

| Operation | Scalar | SSE2 | AVX2 | NEON |
|-----------|--------|------|------|------|
| AND/OR/XOR | All widths | 128/256 | 256/512 | 128/256/512 |
| NOT | All widths | 128/256 | 256/512 | 128/256/512 |
| ANDNOT | All widths | 128/256 | 256/512 | 128/256/512 |
| Shift Left | All widths | 128/256 | 256/512 | 128/256/512 |
| Shift Right | All widths | 128/256 | 256/512 | 128/256/512 |
| is_zero | All widths | 128/256 | 256/512 | 128/256/512 |
| popcount | All widths | — | — | — |

> Unmatched width/backend combinations automatically fall back to scalar implementation.

## Compiler Flags

Enable specific SIMD instruction sets:

### GCC/Clang (x86)

```bash
# Auto-detect CPU capabilities
-march=native

# Manual specification
-msse2
-mavx
-mavx2
-mavx512f
```

### GCC/Clang (ARM)

```bash
# ARM64 with NEON
-march=armv8-a+simd

# ARM32 with NEON
-mfpu=neon
```

### MSVC

```cmd
/arch:AVX
/arch:AVX2
```

## AVX-512 Support Status

Currently, when AVX-512 is detected, the library falls back to AVX2 implementation. Full AVX-512 support is planned for future releases.

To explicitly use AVX2 even when AVX-512 is available:

```cpp
bitcal::bitarray<256, bitcal::simd_backend::avx2> arr;
```

## Cross-Platform Code

For maximum portability:

```cpp
// Let the compiler choose automatically
bitcal::bit256 automatic;

// Force scalar for guaranteed compatibility
bitcal::bitarray<256, bitcal::simd_backend::scalar> portable;
```

## Performance Comparisons

Run benchmarks with different backends:

```bash
# Build with different flags and compare
g++ -std=c++17 -O3 -march=native benchmark.cpp -o bench_native
g++ -std=c++17 -O3 -msse2 benchmark.cpp -o bench_sse2
g++ -std=c++17 -O3 -mavx2 benchmark.cpp -o bench_avx2

./bench_native
./bench_sse2
./bench_avx2
```

## See Also

- [Architecture](../architecture/simd-dispatch.html) — SIMD dispatch implementation
- [Platform Support](../architecture/platform-support.html) — Supported platforms
