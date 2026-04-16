# Build Options

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BITCAL_BUILD_TESTS` | ON | Build unit tests |
| `BITCAL_BUILD_EXAMPLES` | ON | Build example programs |
| `BITCAL_BUILD_BENCHMARKS` | OFF | Build performance benchmarks |
| `BITCAL_NATIVE_ARCH` | ON | Enable `-march=native` or `/arch:AVX2` |

## Compiler Flags

### x86/x64 (GCC/Clang)

```bash
# Auto-detect optimal instruction set
g++ -std=c++17 -O3 -march=native main.cpp

# Specify instruction set explicitly
g++ -std=c++17 -O3 -mavx2 main.cpp   # AVX2
g++ -std=c++17 -O3 -msse2 main.cpp   # SSE2
```

### x86/x64 (MSVC)

```cmd
cl /std:c++17 /O2 /arch:AVX2 main.cpp
```

### ARM (GCC/Clang)

```bash
# ARM64 with NEON
g++ -std=c++17 -O3 -march=armv8-a+simd main.cpp

# ARM32 with NEON
g++ -std=c++17 -O3 -mfpu=neon main.cpp
```

## Macro Definitions

The following macros are automatically defined by the library:

| Macro | Description |
|-------|-------------|
| `BITCAL_ARCH_X86` | x86/x64 architecture |
| `BITCAL_ARCH_ARM` | ARM architecture |
| `BITCAL_HAS_SSE2` | SSE2 available |
| `BITCAL_HAS_AVX` | AVX available |
| `BITCAL_HAS_AVX2` | AVX2 available |
| `BITCAL_HAS_AVX512` | AVX-512 available |
| `BITCAL_HAS_NEON` | NEON available |

You can check these at compile time:

```cpp
#ifdef BITCAL_HAS_AVX2
    // AVX2 code path
#endif
```

## Alignment Control

```cpp
// Default 64-byte alignment (cache line)
// Can be overridden before including:
#define BITCAL_ALIGNMENT 32
#include <bitcal/bitcal.hpp>
```

## Build Examples

```bash
# Full build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBITCAL_NATIVE_ARCH=ON ..
make

# Tests only
cmake -DBITCAL_BUILD_EXAMPLES=OFF -DBITCAL_BUILD_BENCHMARKS=OFF ..
make
./tests/test_bitcal

# Benchmarks
ccmake -DBITCAL_BUILD_BENCHMARKS=ON ..
make
./benchmarks/benchmark_bitcal
```

## Debug vs Release

**Release (recommended for production)**:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

**Debug**:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..  # Enables assertions
```

## Cross-Compilation

### ARM64 from x86

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm64-toolchain.cmake ..
```

### ARM32 from x86

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm32-toolchain.cmake ..
```

## Link-Time Optimization (LTO)

For maximum performance, enable LTO:

```bash
cmake -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
```

Note: LTO may increase compile time but can improve runtime performance by 5-15%.
