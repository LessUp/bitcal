# Installation Guide

Complete guide for installing BitCal on your system.

## Table of Contents

- [System Requirements](#system-requirements)
- [Installation Methods](#installation-methods)
- [Compiler Configuration](#compiler-configuration)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)

---

## System Requirements

### Minimum Requirements

| Component | Minimum Version | Notes |
|-----------|-----------------|-------|
| C++ Standard | C++17 | `if constexpr` and inline variables required |
| GCC | 7.0+ | GCC 11+ recommended for best optimization |
| Clang | 6.0+ | LLVM 14+ recommended |
| MSVC | 2017 (19.14+) | Visual Studio 2022 recommended |
| CMake | 3.16+ | Only required for building tests/examples |

### Recommended Development Environment

| Platform | Compiler | Architecture |
|----------|----------|--------------|
| Linux | GCC 11+ or Clang 14+ | x86-64, ARM64, ARM32 |
| macOS | Apple Clang 14+ | x86-64, ARM64 (Apple Silicon) |
| Windows | MSVC 2022 | x86-64 |

### SIMD Support Matrix

| Platform | Architecture | SSE2 | AVX2 | NEON | Auto-Detection |
|----------|--------------|------|------|------|----------------|
| Linux | x86-64 | ✅ | ✅ | N/A | `-march=native` |
| Linux | ARM64 | N/A | N/A | ✅ | `-march=armv8-a` |
| Linux | ARM32 | N/A | N/A | ✅ | `-mfpu=neon` |
| macOS | x86-64 | ✅ | ✅ | N/A | `-march=native` |
| macOS | ARM64 | N/A | N/A | ✅ | `-mcpu=apple-m1` |
| Windows | x86-64 | ✅ | ✅ | N/A | `/arch:AVX2` |

---

## Installation Methods

### Method 1: Header-Only Copy (Recommended)

The simplest and most common approach. No compilation, build system, or package manager required.

```bash
# Step 1: Clone or download
git clone https://github.com/LessUp/bitcal.git
cd bitcal

# Step 2: Copy headers to your project
cp -r include/bitcal /path/to/your/project/include/

# Or create a vendor directory
mkdir -p /path/to/your/project/vendor
cp -r include/bitcal /path/to/your/project/vendor/
```

**In your source code:**
```cpp
#include <bitcal/bitcal.hpp>  // If in system include path
// or
#include "bitcal/bitcal.hpp"  // If in local directory
```

**Compile:**
```bash
# Basic compilation (portable, scalar fallback)
g++ -std=c++17 -O2 your_program.cpp -o your_program

# Recommended: Enable SIMD for your CPU
g++ -std=c++17 -O3 -march=native your_program.cpp -o your_program
```

### Method 2: CMake find_package (System Install)

Install BitCal system-wide and use CMake's `find_package`.

```bash
# Step 1: Clone and install
git clone https://github.com/LessUp/bitcal.git
cd bitcal
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install  # May require sudo
```

**In your CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject)

find_package(bitcal REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

**Build your project:**
```bash
mkdir build && cd build
cmake ..
make
```

### Method 3: FetchContent (CMake 3.14+)

Automatically download and use BitCal as a dependency without manual installation.

**In your CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.14)
project(MyProject)

include(FetchContent)
FetchContent_Declare(
    bitcal
    GIT_REPOSITORY https://github.com/LessUp/bitcal.git
    GIT_TAG v2.1.0  # Pin to specific version
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(bitcal)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

**Benefits:**
- Automatic dependency management
- Version pinning for reproducible builds
- No manual installation steps
- Works well with CI/CD pipelines

### Method 4: Git Submodule

Include BitCal as a git submodule for version-controlled dependencies.

```bash
# Add as submodule
git submodule add https://github.com/LessUp/bitcal.git third_party/bitcal
git submodule update --init

# Pin to specific version
cd third_party/bitcal
git checkout v2.1.0
cd ../..
git add third_party/bitcal
git commit -m "Add bitcal v2.1.0 submodule"
```

**In your CMakeLists.txt:**
```cmake
add_subdirectory(third_party/bitcal EXCLUDE_FROM_ALL)
target_link_libraries(my_app PRIVATE bitcal::bitcal)
```

---

## Compiler Configuration

### Optimization Flags

#### GCC / Clang

| Flag | Effect | Use Case |
|------|--------|----------|
| `-O2` | Standard optimization | Production builds |
| `-O3` | Aggressive optimization | Production builds (recommended) |
| `-march=native` | Optimize for current CPU | Development, local deployment |
| `-mavx2` | Enable AVX2 manually | Known target hardware |
| `-msse2` | Enable SSE2 (safe fallback) | Maximum compatibility |

**Recommended combinations:**
```bash
# Development (best performance on your machine)
g++ -std=c++17 -O3 -march=native -DNDEBUG program.cpp

# Production deployment (target specific)
g++ -std=c++17 -O3 -mavx2 -mtune=generic program.cpp

# Maximum compatibility
g++ -std=c++17 -O2 -msse2 program.cpp
```

#### MSVC

| Flag | Effect |
|------|--------|
| `/O2` | Maximize speed |
| `/arch:AVX2` | Enable AVX2 |
| `/std:c++17` | C++17 standard (VS2017: `/std:c++17`) |

```cmd
cl /std:c++17 /O2 /arch:AVX2 /EHsc program.cpp
```

### CMake Options

When building tests, examples, or benchmarks:

| Option | Default | Description |
|--------|---------|-------------|
| `BITCAL_BUILD_TESTS` | `ON` | Build unit test suite |
| `BITCAL_BUILD_EXAMPLES` | `ON` | Build example programs |
| `BITCAL_BUILD_BENCHMARKS` | `OFF` | Build Google Benchmark suite |
| `BITCAL_NATIVE_ARCH` | `ON` | Enable `-march=native` optimizations |

**Example:**
```bash
cmake -DBITCAL_BUILD_TESTS=ON \
      -DBITCAL_BUILD_BENCHMARKS=ON \
      -DBITCAL_NATIVE_ARCH=ON \
      ..
make -j$(nproc)
```

---

## Verification

### Quick Verification Test

Create `test_install.cpp`:
```cpp
#include <bitcal/bitcal.hpp>
#include <iostream>

int main() {
    std::cout << "BitCal v2.1.0 Installation Test\n";
    std::cout << "================================\n\n";
    
    // Test basic operations
    bitcal::bit256 a(0xFFFFFFFF);
    bitcal::bit256 b(0xAAAAAAAA);
    
    auto result = a & b;
    std::cout << "AND operation: " << result.popcount() << " bits set\n";
    
    // Display detected backend
    std::cout << "SIMD Backend: ";
    switch (bitcal::get_default_backend()) {
        case bitcal::simd_backend::avx2:  std::cout << "AVX2"; break;
        case bitcal::simd_backend::sse2:  std::cout << "SSE2"; break;
        case bitcal::simd_backend::neon:  std::cout << "NEON"; break;
        default:                          std::cout << "Scalar";
    }
    std::cout << "\n";
    
    // Type information
    std::cout << "\nType Information:\n";
    std::cout << "  bit256 bits: " << bitcal::bit256::bits << "\n";
    std::cout << "  bit256 words: " << bitcal::bit256::u64_count << "\n";
    
    std::cout << "\n✓ All tests passed!\n";
    return 0;
}
```

**Compile and run:**
```bash
g++ -std=c++17 -O2 -march=native test_install.cpp -o test_install
./test_install
```

**Expected output:**
```
BitCal v2.1.0 Installation Test
================================

AND operation: 16 bits set
SIMD Backend: AVX2

Type Information:
  bit256 bits: 256
  bit256 words: 4

✓ All tests passed!
```

### Platform Detection Test

```cpp
#include <bitcal/bitcal.hpp>
#include <bitcal/config.hpp>
#include <iostream>

int main() {
    std::cout << "Platform Detection:\n";
    
    #if BITCAL_ARCH_X86
        std::cout << "  Architecture: x86/x86-64\n";
        #if BITCAL_HAS_AVX2
            std::cout << "  AVX2: Available\n";
        #endif
        #if BITCAL_HAS_SSE2
            std::cout << "  SSE2: Available\n";
        #endif
    #elif BITCAL_ARCH_ARM
        std::cout << "  Architecture: ARM\n";
        #if BITCAL_HAS_NEON
            std::cout << "  NEON: Available\n";
        #endif
    #endif
    
    return 0;
}
```

---

## Troubleshooting

### "SIMD intrinsics not found"

**Symptoms:**
```
fatal error: immintrin.h: No such file or directory
```

**Solution:**
1. Check compiler version: `gcc --version` (must be 7+)
2. Add correct flags:
   ```bash
   # GCC/Clang x86
   g++ -std=c++17 -mavx2 -O2 program.cpp
   
   # GCC ARM
   g++ -std=c++17 -march=armv8-a+simd -O2 program.cpp
   ```

### "Undefined reference" errors

**Cause:** BitCal is header-only. Linking errors suggest incorrect setup.

**Solution:**
- Ensure you're not trying to link against a library: `g++ -std=c++17 file.cpp` (no `-lbitcal`)
- Check that all headers are in the include path

### Performance lower than expected

**Diagnostic checklist:**
1. **Optimization enabled?**
   ```bash
   # Check: Should show -O2 or -O3
   g++ -std=c++17 -Q --help=optimizers program.cpp 2>&1 | grep -E '^\s+-O'
   ```

2. **SIMD active?**
   ```bash
   g++ -std=c++17 -march=native -dM -E - < /dev/null | grep -E '(AVX|SSE|NEON)'
   ```

3. **Compare backends:**
   ```cpp
   // Force scalar, then compare
   bitcal::bitarray<256, bitcal::simd_backend::scalar> scalar_arr;
   bitcal::bit256 simd_arr;  // Default auto-selection
   ```

### CMake can't find package

**Solution:**
```bash
# Option 1: Specify prefix
cmake -DCMAKE_PREFIX_PATH=/path/to/bitcal/install ..

# Option 2: Use FetchContent instead (see Method 3)
```

---

## Next Steps

- 🚀 [Quick Start](quickstart.md) — Write your first BitCal program
- 📚 [API Reference](../api/types.md) — Explore the complete API
- 🏗️ [Architecture](../architecture/overview.md) — Understand the design

---

**Need more help?** [Open an issue on GitHub](https://github.com/LessUp/bitcal/issues)
