<!-- From: /home/shane/dev/bitcal/AGENTS.md -->
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
| `avx` | x86-64 | 256-bit | `BITCAL_HAS_AVX` |
| `avx2` | x86-64 | 256-bit | `BITCAL_HAS_AVX2` |
| `avx512` | x86-64 | 512-bit | `BITCAL_HAS_AVX512` (partial) |
| `neon` | ARM (ARMv7-A+/ARM64) | 128-bit | `BITCAL_HAS_NEON` |

### Predefined Types

```cpp
using bit64   = bitarray<64>;     // 1 x uint64_t (specialized implementation)
using bit128  = bitarray<128>;    // 2 x uint64_t
using bit256  = bitarray<256>;    // 4 x uint64_t
using bit512  = bitarray<512>;    // 8 x uint64_t
using bit1024 = bitarray<1024>;   // 16 x uint64_t
```

Custom widths are supported (must be multiple of 64):
```cpp
bitcal::bitarray<2048> custom;
```

### Template Parameters

```cpp
template<size_t Bits, simd_backend Backend = get_default_backend()>
class bitarray;
```

- `Bits`: Total number of bits (must be >= 64 and multiple of 64)
- `Backend`: SIMD implementation to use (default: auto-selected)

---

## Technology Stack

- **Language:** C++17 (minimum)
- **Build System:** CMake 3.16+
- **Supported Compilers:**
  - GCC 7+
  - Clang 6+
  - MSVC 2017+
  - Apple Clang
- **Testing:** Custom lightweight test framework (single-file, macro-based)
- **Benchmarking:** Google Benchmark (optional) or built-in simple timer
- **Code Formatting:** clang-format (Google style based)
- **Code Analysis:** clang-tidy
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
│   ├── basic_usage.cpp
│   └── performance_comparison.cpp
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
./benchmarks/bitcal_benchmark
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BITCAL_BUILD_TESTS` | ON | Build unit tests |
| `BITCAL_BUILD_EXAMPLES` | ON | Build example programs |
| `BITCAL_BUILD_BENCHMARKS` | OFF | Build performance benchmarks |
| `BITCAL_NATIVE_ARCH` | ON | Use `-march=native` or equivalent for tests/examples |
| `BITCAL_ENABLE_LTO` | ON | Enable Link Time Optimization |
| `BITCAL_ENABLE_HARDENING` | OFF | Enable security hardening flags |

**Important:** The INTERFACE library (`bitcal`) itself does NOT carry SIMD flags. Tests and examples use `BITCAL_SIMD_FLAGS` which is set based on `BITCAL_NATIVE_ARCH`.

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

### bit64 Specialization

The `bit64` type (64-bit) is fully specialized with a more efficient implementation using a single `uint64_t` value instead of an array:

```cpp
template<>
class bitarray<64> {
    // Direct uint64_t storage
    uint64_t value_;
    // Optimized operations
};
```

---

## Public API Reference

### Construction

```cpp
bitarray<N> a;                    // Default constructor (all zeros)
bitarray<N> b(0xDEADBEEF);        // Value constructor (lower 64 bits)
bitarray<N> c = b;                // Copy constructor
bitarray<N> d = std::move(b);     // Move constructor
```

### Data Access

```cpp
uint64_t* data() noexcept;                    // Direct pointer to words
const uint64_t* data() const noexcept;
uint64_t operator[](size_t idx) const;        // Read word at index
uint64_t& operator[](size_t idx);             // Read/write word at index
static constexpr size_t bits;                 // Total bit count
static constexpr size_t u64_count;            // Number of 64-bit words
static constexpr simd_backend backend;        // Backend used
```

### Bitwise Operations

```cpp
bitarray operator&(const bitarray& other) const;  // AND
bitarray operator|(const bitarray& other) const;  // OR
bitarray operator^(const bitarray& other) const;  // XOR
bitarray operator~() const;                       // NOT
bitarray andnot(const bitarray& mask) const;      // Optimized: a & ~mask

bitarray& operator&=(const bitarray& other);      // AND-assign
bitarray& operator|=(const bitarray& other);      // OR-assign
bitarray& operator^=(const bitarray& other);      // XOR-assign
```

### Shift Operations

```cpp
void shift_left(int count) noexcept;          // In-place left shift
void shift_right(int count) noexcept;         // In-place right shift
bitarray operator<<(int count) const;         // Return shifted copy (left)
bitarray operator>>(int count) const;         // Return shifted copy (right)
bitarray& operator<<=(int count) noexcept;    // Left shift-assign
bitarray& operator>>=(int count) noexcept;    // Right shift-assign
```

**Shift Behavior:**
- `count == 0`: No operation
- `count >= Bits`: All bits cleared
- `count < 0`: Undefined behavior

### Bit Counting

```cpp
uint64_t popcount() const;                    // Count set bits (HW accelerated)
uint64_t count() const;                       // Alias for popcount()
int count_leading_zeros() const;              // CLZ: zeros from MSB
int count_trailing_zeros() const;             // CTZ: zeros from LSB
```

### Single Bit Operations

```cpp
bool get_bit(size_t idx) const;               // Read bit at idx
bool test(size_t idx) const;                  // Alias for get_bit
void set_bit(size_t idx, bool val = true);    // Set/clear bit at idx
void flip_bit(size_t idx);                    // Toggle bit at idx
```

### Range Operations

```cpp
void set_range(size_t start, size_t end);     // Set bits [start, end)
void clear_range(size_t start, size_t end);   // Clear bits [start, end)
void flip_range(size_t start, size_t end);    // Flip bits [start, end)
```

### State Detection & Queries

```cpp
bool is_zero() const;                         // Check if all bits are zero
void clear();                                 // Set all bits to zero
bool all() const;                             // Check if all bits are set
bool any() const;                             // Check if any bit is set
bool none() const;                            // Check if no bits are set
static constexpr size_t size() noexcept;      // Return Bits
```

### Find Operations

```cpp
int find_first_set() const;                   // Index of first set bit, -1 if zero
int find_last_set() const;                    // Index of last set bit, -1 if zero
```

### Comparison

```cpp
bool operator==(const bitarray& other) const; // Equality
bool operator!=(const bitarray& other) const; // Inequality
```

### Bit Manipulation

```cpp
void reverse();                               // Reverse bit order
```

### Type Traits

```cpp
// Check if T is a bitarray type
template<typename T>
struct is_bitarray : std::false_type {};

template<size_t Bits, simd_backend Backend>
struct is_bitarray<bitarray<Bits, Backend>> : std::true_type {};

template<typename T>
inline constexpr bool is_bitarray_v = is_bitarray<T>::value;

// Type traits for bitarray
template<size_t Bits, simd_backend Backend>
struct bitarray_traits<bitarray<Bits, Backend>> {
    static constexpr size_t bits = Bits;
    static constexpr size_t u64_count = Bits / 64;
    static constexpr simd_backend backend = Backend;
    using word_type = uint64_t;
    using type = bitarray<Bits, Backend>;
};
```

### Low-Level API (ops namespace)

```cpp
namespace bitcal::ops {
    template<size_t Bits>
    uint64_t popcount(const uint64_t* data);

    template<size_t Bits>
    int count_leading_zeros(const uint64_t* data);

    template<size_t Bits>
    int count_trailing_zeros(const uint64_t* data);

    uint64_t reverse_bits_64(uint64_t x);
    uint64_t byte_swap_64(uint64_t x);
}
```

---

## Testing Strategy

### Test Framework

BitCal uses a **custom lightweight test framework** defined in `tests/test_bitcal.cpp`:

```cpp
#define ASSERT_EQ(a, b)     // Check equality
#define ASSERT_TRUE(expr)   // Check true condition
#define RUN_TEST(func)      // Register and run test function
```

**Output format:**
```
=== BitCal Unit Tests ===

[64-bit]
  test_bit64_basic ... PASS
  test_bit64_shift_left ... PASS
  ...

==============================
Total: 55  Pass: 55  Fail: 0
ALL TESTS PASSED!
```

### Test Organization

Tests are organized by bit width:

1. **64-bit tests** - Basic operations, scalar backend, bit64 specialization
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
- **Range operations:** set_range, clear_range, flip_range
- **State detection:** is_zero, clear, all, any, none
- **Find operations:** find_first_set, find_last_set
- **Manipulation:** reverse
- **Edge cases:** Shift by 0, shift by width, all zeros, all ones

### Running Tests

```bash
# Run all tests
./tests/test_bitcal
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
| `BITCAL_CONSTEXPR` | `constexpr` (C++14+) | `constexpr` |

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
