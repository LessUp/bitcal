# Platform Support

## Operating Systems

| OS | Status | Notes |
|----|--------|-------|
| Linux | ✅ Primary target | Full GCC/Clang support |
| Windows | ✅ Supported | MSVC 2017+ |
| macOS | ✅ Supported | Apple Clang |
| FreeBSD/OpenBSD | ✅ Supported | GCC/Clang |

## Processor Architectures

| Architecture | Instruction Set | Status |
|--------------|-----------------|--------|
| x86-64 | SSE2 / AVX / AVX2 / AVX-512 | ✅ Full support |
| ARM64 (AArch64) | NEON | ✅ Full support |
| ARM32 (ARMv7-A) | NEON | ✅ Supported |
| Others | — | ⚠️ Scalar fallback |

## Compilers

| Compiler | Minimum Version | Recommended | Notes |
|----------|-----------------|-------------|-------|
| GCC | 7+ | 11+ | Best optimization |
| Clang | 6+ | 14+ | Good diagnostics |
| MSVC | 2017 (19.14+) | 2022 | v2.1 fixed SSE2 detection |
| Apple Clang | — | Latest Xcode | Bundled with Xcode |

## ARM Device Support

Tested and verified on:

- **Raspberry Pi**: 3, 4, 5 (ARM32/64)
- **NVIDIA Jetson**: Nano, TX1, TX2, Xavier, Orin
- **Apple Silicon**: M1, M2, M3, M4
- **AWS Graviton**: All generations
- **Mobile**: Various ARM Cortex-A processors

## x86 Feature Detection

### GCC/Clang

Macros automatically defined when corresponding flags are used:

```bash
g++ -mavx2 ...     # Defines __AVX2__
g++ -msse2 ...     # Defines __SSE2__
```

### MSVC

MSVC guarantees SSE2 availability on x64 but does not define `__SSE2__`. BitCal v2.1 detects this via `_M_X64`:

```cpp
#if defined(_M_X64)
    // SSE2 is guaranteed on x64
    #define BITCAL_HAS_SSE2 1
#endif
```

To enable AVX2 with MSVC, add `/arch:AVX2` at compile time. CMake automatically detects and adds this flag.

## Cross-Compilation

### ARM64 from x86 Linux

```bash
# Install cross-compiler
sudo apt-get install g++-aarch64-linux-gnu

# Configure
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm64-toolchain.cmake ..

# Build
make
```

Example toolchain file (`cmake/arm64-toolchain.cmake`):

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

### ARM32 from x86 Linux

```bash
sudo apt-get install g++-arm-linux-gnueabihf
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm32-toolchain.cmake ..
make
```

## CI/CD Verification

BitCal is automatically tested on GitHub Actions with the following configurations:

| Platform | Compiler | Architecture | SIMD |
|----------|----------|--------------|------|
| Ubuntu | GCC 11 | x86-64 | AVX2 |
| Ubuntu | Clang 14 | x86-64 | AVX2 |
| Ubuntu | GCC | ARM64 | NEON |
| Ubuntu | GCC | ARM32 | NEON |
| Windows | MSVC 2022 | x86-64 | AVX2 |
| macOS | Apple Clang | x86-64 | AVX2 |
| macOS | Apple Clang | ARM64 | NEON |

## Known Issues

### MSVC

- **Fixed in v2.1**: SSE2 macro detection now works correctly on x64
- AVX2 requires explicit `/arch:AVX2` flag

### ARM

- **Fixed in v2.1**: NEON `vmvnq_u64` changed to `veorq_u64` XOR with all-ones
- **Fixed in v2.1**: NEON variable shifts now use `vshlq_u64` instead of `vshlq_n_u64`

### General

- AVX-512: Falls back to AVX2 (full AVX-512 support planned)
- Very large bit widths (>8192): May cause stack overflow; consider heap allocation

## Reporting Platform Issues

If you encounter platform-specific issues:

1. Check [GitHub Issues](https://github.com/LessUp/bitcal/issues)
2. Include compiler version: `gcc --version` or `clang --version`
3. Include CPU info: `cat /proc/cpuinfo` (Linux) or system profiler
4. Provide minimal reproducible example

## See Also

- [SIMD Backend](../api/simd-backend.html) — Backend selection
- [SIMD Dispatch](simd-dispatch.html) — Dispatch mechanism
