# BitCal - AI Agent Development Guide

This document provides essential information for AI coding agents working on the BitCal project. BitCal is a modern, high-performance C++17 header-only library for bit manipulation operations with automatic SIMD acceleration.

**Current Version:** v2.1.0  
**Minimum C++ Standard:** C++17  
**Project Philosophy:** Spec-Driven Development (SDD)

---

## Table of Contents

- [Project Overview](#project-overview)
- [Technology Stack](#technology-stack)
- [Project Structure](#project-structure)
- [Development Workflow](#development-workflow)
- [Build and Test Commands](#build-and-test-commands)
- [Code Style Guidelines](#code-style-guidelines)
- [Architecture Overview](#architecture-overview)
- [Testing Strategy](#testing-strategy)
- [Security Considerations](#security-considerations)

---

## Project Overview

BitCal is a header-only C++ library that provides SIMD-accelerated bit manipulation operations. The library automatically selects the optimal SIMD backend at compile time based on the target architecture and compiler flags.

### Supported SIMD Backends

| Backend | Architecture | Register Width | Detection Macro |
|---------|-------------|----------------|-----------------|
| `scalar` | Any | 64-bit | Always available |
| `sse2` | x86-64 | 128-bit | `BITCAL_HAS_SSE2` |
| `avx2` | x86-64 | 256-bit | `BITCAL_HAS_AVX2` |
| `neon` | ARM (ARMv7-A+/ARM64) | 128-bit | `BITCAL_HAS_NEON` |
| `avx512` | x86-64 | 512-bit | `BITCAL_HAS_AVX512` (partial) |

### Predefined Types

```cpp
using bit64   = bitarray<64>;     // 1 x uint64_t
using bit128  = bitarray<128>;    // 2 x uint64_t  
using bit256  = bitarray<256>;    // 4 x uint64_t
using bit512  = bitarray<512>;    // 8 x uint64_t
using bit1024 = bitarray<1024>;   // 16 x uint64_t
```

Custom widths are supported (must be multiple of 64):
```cpp
bitcal::bitarray<2048> custom;
```

---

## Technology Stack

- **Language:** C++17 (minimum)
- **Build System:** CMake 3.16+
- **Supported Compilers:**
  - GCC 7+
  - Clang 6+
  - MSVC 2017+
  - Apple Clang
- **Testing:** Custom lightweight test framework (single-file)
- **Benchmarking:** Google Benchmark (optional)
- **Code Formatting:** clang-format
- **CI/CD:** GitHub Actions

---

## Project Structure

```
bitcal/
├── include/bitcal/         # Header files (header-only library)
│   ├── bitcal.hpp          # Main header - include this
│   ├── config.hpp          # Platform detection, version macros, simd_backend enum
│   ├── simd_traits.hpp     # SIMD type traits for each backend
│   ├── scalar_ops.hpp      # Portable scalar implementation (baseline)
│   ├── sse_ops.hpp         # SSE2 (128-bit) operations for x86
│   ├── avx_ops.hpp         # AVX2 (256-bit) operations for x86
│   ├── avx512_ops.hpp      # AVX-512 (512-bit) operations for x86
│   └── neon_ops.hpp        # NEON (128-bit) operations for ARM
├── specs/                  # Specification documents (SDD workflow)
│   ├── api/                # API specifications
│   ├── product/            # Product requirements and acceptance criteria
│   ├── rfc/                # Technical design documents
│   └── testing/            # Test specifications
├── docs/                   # User documentation
│   ├── en/                 # English documentation
│   └── zh/                 # Chinese documentation
├── tests/                  # Unit tests (test_bitcal.cpp)
├── benchmarks/             # Performance benchmarks (benchmark_bitcal.cpp)
├── examples/               # Example programs
├── cmake/                  # CMake configuration templates
└── .github/workflows/      # CI/CD configuration
```

---

## Development Workflow

**CRITICAL: This project follows Spec-Driven Development (SDD).** All changes must follow this workflow:

### Spec-Driven Development Workflow

```
Step 1: Review Specs
    ↓ Read /specs/product/, /specs/rfc/, /specs/api/
Step 2: Identify Conflicts
    ↓ If user request conflicts with specs, STOP and alert user
Step 3: Propose Spec Updates (if needed)
    ↓ Update /specs/product/ for features, /specs/api/ for API changes
Step 4: Wait for Approval
    ↓ Do not write code until specs are approved
Step 5: Implement According to Spec
    ↓ Follow naming conventions and API contracts exactly
Step 6: Test Against Spec
    ↓ Verify all acceptance criteria are met
```

### Spec Directory Structure

| Directory | Purpose | When to Update |
|-----------|---------|----------------|
| `/specs/product/` | Product requirements and acceptance criteria | Adding new features |
| `/specs/rfc/` | Technical design documents and architecture proposals | Design decisions, optimizations |
| `/specs/api/` | Public API specifications | Interface changes |
| `/specs/testing/` | Test coverage requirements | New test scenarios |

### Conflict Detection Examples

- **User wants to add bit2048 type**
  - Check `/specs/product/bit-manipulation-library.md` for allowed types
  - If not listed, propose spec update first
  
- **User wants to change API naming**
  - Check `/specs/api/bitcal-public-api.md` for current naming
  - Any API changes require spec update first

---

## Build and Test Commands

### Quick Start

```bash
# Build tests and examples
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests
./tests/test_bitcal

# Run benchmarks (if built)
./benchmarks/bench_bitcal
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BITCAL_BUILD_TESTS` | ON | Build unit tests |
| `BITCAL_BUILD_EXAMPLES` | ON | Build example programs |
| `BITCAL_BUILD_BENCHMARKS` | OFF | Build performance benchmarks |
| `BITCAL_NATIVE_ARCH` | ON | Use `-march=native` for tests/examples |
| `BITCAL_ENABLE_LTO` | ON | Enable Link Time Optimization |
| `BITCAL_ENABLE_HARDENING` | OFF | Enable security hardening flags |

### Example: Build with Benchmarks

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DBITCAL_BUILD_BENCHMARKS=ON \
         -DBITCAL_NATIVE_ARCH=ON
make -j$(nproc)
```

### Cross-Compilation (ARM)

```bash
# ARM64
cmake .. -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
         -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
         -DBITCAL_NATIVE_ARCH=OFF

# ARM32
cmake .. -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
         -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
         -DBITCAL_NATIVE_ARCH=OFF
```

### Code Formatting

```bash
# Check formatting
clang-format --dry-run --Werror include/bitcal/*.hpp

# Apply formatting
clang-format -i include/bitcal/*.hpp
```

---

## Code Style Guidelines

### File Naming

- **Headers:** `lowercase_underscore.hpp` (`scalar_ops.hpp`, `avx_ops.hpp`)
- **Tests:** `test_<module>.cpp` (`test_bitcal.cpp`)
- **Benchmarks:** `benchmark_<module>.cpp` (`benchmark_bitcal.cpp`)
- **Examples:** `descriptive_name.cpp` (`basic_usage.cpp`)

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Namespace | lowercase | `bitcal` |
| Class/Struct | lowercase + underscore | `bitarray` |
| Function | lowercase + underscore | `popcount()`, `shift_left()` |
| Variable | lowercase + underscore | `bit_index`, `word_count` |
| Constant/Macro | SCREAMING_SNAKE_CASE | `BITCAL_HAS_AVX2`, `BITCAL_FORCEINLINE` |
| Template parameter | PascalCase | `Bits`, `Backend` |
| Private member | trailing underscore | `data_` |

### Code Quality Requirements

- **No compiler warnings** with `-Wall -Wextra -Wpedantic`
- Use `constexpr` and `noexcept` where appropriate
- Prefer `if constexpr` over runtime dispatch
- Force-inline critical paths with `BITCAL_FORCEINLINE`
- Include documentation comments for all public APIs

### Include Order

Headers must be included in this order (enforced by `.clang-format`):

1. Project config headers (`"config.hpp"`)
2. Other project headers (`"*.hpp"`)
3. C standard library (`<cstdint>`, `<cstring>`, etc.)
4. C++ standard library (`<type_traits>`, `<limits>`, etc.)
5. System headers (`<immintrin.h>`, `<arm_neon.h>`)

**CRITICAL:** SIMD intrinsics headers (`<immintrin.h>`, `<arm_neon.h>`) must be included **outside** of any namespace scope, after standard headers. This is handled in `config.hpp`.

---

## Architecture Overview

### Four-Layer Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    User API Layer                               │
│  bitarray<N>, Operators, Type Aliases (bit64, bit256, etc.)    │
├─────────────────────────────────────────────────────────────────┤
│                 Compile-Time Dispatch Layer                     │
│  if constexpr, Traits, Backend Selection                        │
├─────────────────────────────────────────────────────────────────┤
│                   SIMD Implementation Layer                     │
│  ├── scalar_ops.hpp  (Portable fallback)                        │
│  ├── sse_ops.hpp     (x86 SSE2 128-bit)                         │
│  ├── avx_ops.hpp     (x86 AVX2 256-bit)                         │
│  ├── avx512_ops.hpp  (x86 AVX-512 512-bit)                      │
│  └── neon_ops.hpp    (ARM NEON 128-bit)                         │
├─────────────────────────────────────────────────────────────────┤
│                   Hardware Instruction Layer                    │
│  SSE2 / AVX / AVX2 / AVX-512 / NEON / Scalar                    │
└─────────────────────────────────────────────────────────────────┘
```

### Dispatch Pattern

The library uses `if constexpr` for compile-time backend selection:

```cpp
template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray {
    // ...
    BITCAL_FORCEINLINE bitarray operator&(const bitarray& other) const noexcept {
        bitarray result;
        dispatch_binop<binop::op_and>(*this, other, result);
        return result;
    }
};
```

The `dispatch_binop` template uses `if constexpr` chains to select the optimal implementation based on `Bits` and `Backend`.

### Memory Layout

```
bitarray<256> memory representation:
┌──────────────────────────────────────────────────────────────────────┐
│ Alignment  │  Word 0   │  Word 1   │  Word 2   │  Word 3   │ Padding │
│  64 bytes  │ bits 0-63 │bits 64-127│bits 128-191│bits 192-255│to 64B  │
└──────────────────────────────────────────────────────────────────────┘
                    Little-endian: word 0 holds LSB
```

- **Alignment:** 64 bytes (cache-line aligned for AVX-512)
- **Storage:** Contiguous `uint64_t` array
- **Endianness:** Little-endian (LSB in `data_[0]`)

### Backend Selection Priority

```cpp
// Priority order (highest to lowest)
#if BITCAL_HAS_AVX512
    return simd_backend::avx512;
#elif BITCAL_HAS_AVX2
    return simd_backend::avx2;
#elif BITCAL_HAS_AVX
    return simd_backend::avx;
#elif BITCAL_HAS_SSE2
    return simd_backend::sse2;
#elif BITCAL_HAS_NEON
    return simd_backend::neon;
#else
    return simd_backend::scalar;
#endif
```

---

## Testing Strategy

### Test Organization

Tests in `/tests/test_bitcal.cpp` are organized by bit width:

1. **64-bit tests** - Basic operations, scalar backend
2. **128-bit tests** - SSE2/NEON, cross-word carry
3. **256-bit tests** - AVX2, complex shift operations
4. **512-bit tests** - AVX2/AVX-512, cross-256-bit boundary
5. **1024-bit tests** - Full coverage, cross-512-bit boundary

### Test Coverage Requirements

Per `/specs/testing/bitcal-testing-spec.md`:

- **Construction:** Default, value, copy, move
- **Bitwise operations:** AND, OR, XOR, NOT, ANDNOT
- **Shift operations:** Left/right, boundaries, cross-word carry
- **Comparison:** Equality, inequality
- **Bit counting:** popcount, CLZ, CTZ
- **Single bit operations:** get_bit, set_bit, flip_bit
- **State detection:** is_zero, clear
- **Manipulation:** reverse
- **Edge cases:** Shift by 0, shift by width, all zeros, all ones

### Running Tests

```bash
# Run all tests
./tests/test_bitcal

# Expected output format:
# === BitCal Unit Tests ===
#
# [64-bit]
#   test_bit64_basic ... PASS
#   test_bit64_shift_left ... PASS
#   ...
#
# ==============================
# Total: 55  Pass: 55  Fail: 0
# ALL TESTS PASSED!
```

### Adding New Tests

When adding a new feature:

1. Update `/specs/testing/bitcal-testing-spec.md` with test requirements
2. Add test functions in `/tests/test_bitcal.cpp`
3. Register tests in `main()` using `RUN_TEST(func_name)`
4. Maintain coverage for all predefined types (bit64, bit128, bit256, bit512, bit1024)

---

## Security Considerations

### Bounds Checking

- `operator[]` uses `assert()` for bounds checking (debug builds only)
- Production builds with `NDEBUG` have no bounds checking for performance
- Single bit operations (get_bit, set_bit, flip_bit) use `assert()` for bit_index validation

### Integer Overflow/Underflow

- Shift counts are validated: negative shift is undefined behavior
- Shift by >= width results in all zeros (defined behavior)
- All arithmetic uses fixed-width types (`uint64_t`, `size_t`)

### Hardening Options

```bash
# Enable security hardening flags
cmake .. -DBITCAL_ENABLE_HARDENING=ON
```

This enables:
- `-D_FORTIFY_SOURCE=2`
- `-fstack-protector-strong`
- `-Wformat -Wformat-security`
- `-Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack`

### Thread Safety

- **Different instances in different threads:** ✅ Thread-safe
- **Read-only access to shared instance:** ✅ Thread-safe
- **Concurrent read/write on same instance:** ❌ Requires external synchronization

---

## Quick Reference

### Default Backend Macro Detection

| Macro | x86 Detection | ARM Detection |
|-------|--------------|---------------|
| `BITCAL_HAS_SSE2` | `__SSE2__` or `_M_X64` | Always 0 |
| `BITCAL_HAS_AVX` | `__AVX__` | Always 0 |
| `BITCAL_HAS_AVX2` | `__AVX2__` | Always 0 |
| `BITCAL_HAS_AVX512` | `__AVX512F__` | Always 0 |
| `BITCAL_HAS_NEON` | Always 0 | `__ARM_NEON` or `__ARM_NEON__` |

### Compiler-Specific Attributes

| Macro | GCC/Clang | MSVC |
|-------|-----------|------|
| `BITCAL_INLINE` | `__attribute__((always_inline)) inline` | `__forceinline` |
| `BITCAL_FORCEINLINE` | `__attribute__((always_inline)) inline` | `__forceinline` |
| `BITCAL_LIKELY(x)` | `__builtin_expect(!!(x), 1)` | `(x)` |
| `BITCAL_UNLIKELY(x)` | `__builtin_expect(!!(x), 0)` | `(x)` |

### Commit Message Convention

| Prefix | Description |
|--------|-------------|
| `feat:` | New feature |
| `fix:` | Bug fix |
| `docs:` | Documentation |
| `perf:` | Performance optimization |
| `refactor:` | Code refactoring |
| `test:` | Test-related |
| `chore:` | Build/tooling |
| `spec:` | Specification changes |

---

## Getting Help

- **Full Documentation:** https://lessup.github.io/bitcal/
- **Issue Tracker:** https://github.com/LessUp/bitcal/issues
- **Contributing Guide:** See `CONTRIBUTING.md`
- **Changelog:** See `CHANGELOG.md`

---

*This document is maintained for AI coding agents. For human contributors, see `CONTRIBUTING.md` and `README.md`.*
