# Contributing to BitCal

Thank you for your interest in contributing to BitCal! This document explains how to get involved and follows our Spec-Driven Development methodology.

---

## Table of Contents

- [Spec-Driven Development](#spec-driven-development)
- [Development Environment](#development-environment)
- [Code Style](#code-style)
- [Commit Convention](#commit-convention)
- [Adding a New SIMD Backend](#adding-a-new-simd-backend)
- [Documentation](#documentation)
- [Release Process](#release-process)

---

## Spec-Driven Development

BitCal follows **Spec-Driven Development (SDD)**. All new features and changes must be defined in the `/specs` directory before implementation.

### Workflow

```
┌─────────────────────┐
│  1. Review Specs    │  ← Read /specs/ first
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│  2. Propose Update  │  ← Create/modify spec documents
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│  3. Get Approval    │  ← Wait for maintainer sign-off
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│  4. Implement       │  ← Write code per spec
└─────────┬───────────┘
          │
          ▼
┌─────────────────────┐
│  5. Test & Verify   │  ← Verify against acceptance criteria
└─────────────────────┘
```

### Spec Directory Structure

| Directory | Purpose | When to Update |
|-----------|---------|----------------|
| `/specs/product/` | Product feature definitions and acceptance criteria | Adding new features |
| `/specs/rfc/` | Technical design documents and architecture proposals | Design decisions, optimizations |
| `/specs/api/` | Public API specifications | Interface changes |
| `/specs/testing/` | Test coverage requirements | New test scenarios |

### Spec-First Contribution Process

1. **Review existing specs** in `/specs/` to understand current requirements
2. **Open an issue** to discuss the proposed change
3. **Submit spec update PR** before any code changes:
   - New features: Update `/specs/product/`
   - API changes: Update `/specs/api/`
   - Architecture changes: Create RFC in `/specs/rfc/`
4. **Get spec approval** from maintainers
5. **Implement** according to the approved spec
6. **Write tests** that verify against spec acceptance criteria

> **Important:** Pull requests that modify code without corresponding spec updates will be asked to update specs first.

---

## Development Environment

### Requirements

- **C++17** compiler (GCC 7+, Clang 6+, MSVC 2017+)
- **CMake 3.16+** (for tests and benchmarks)
- **Git** for version control

### Building

```bash
# Clone the repository
git clone https://github.com/LessUp/bitcal.git
cd bitcal

# Create build directory
mkdir build && cd build

# Configure (Release mode for performance testing)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make

# Run tests
./tests/test_bitcal
```

### Running Tests

```bash
# Run all tests
./tests/test_bitcal

# Run specific test suite
./tests/test_bitcal --gtest_filter=BitArrayTest*

# Run benchmarks
./benchmarks/bench_bitcal
```

---

## Code Style

### File Naming

- **Headers**: lowercase + underscore (`scalar_ops.hpp`)
- **Tests**: `test_<module>.cpp` (`test_bitcal.cpp`)
- **Benchmarks**: `bench_<module>.cpp` (`bench_bitcal.cpp`)

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Namespace | lowercase | `bitcal` |
| Class/Struct | lowercase + underscore | `bitarray` |
| Function | lowercase + underscore | `popcount()` |
| Variable | lowercase + underscore | `bit_index` |
| Constant | SCREAMING_SNAKE_CASE | `BITCAL_HAS_AVX2` |
| Template parameter | PascalCase | `Bits`, `Backend` |

### Formatting

We use `.clang-format` for consistent formatting. Before committing:

```bash
# Check formatting
clang-format --dry-run --Werror include/bitcal/*.hpp

# Apply formatting
clang-format -i include/bitcal/*.hpp
```

### Code Quality

- No compiler warnings with `-Wall -Wextra -Wpedantic`
- All public APIs must have documentation comments
- Use `constexpr` and `noexcept` where appropriate
- Prefer `if constexpr` over runtime dispatch

---

## Commit Convention

We use semantic commit messages with conventional prefixes:

| Prefix | Description | Example |
|--------|-------------|---------|
| `feat:` | New feature | `feat: add bit2048 type support` |
| `fix:` | Bug fix | `fix: correct AVX2 cross-lane shift` |
| `docs:` | Documentation | `docs: update API reference for popcount` |
| `perf:` | Performance optimization | `perf: optimize is_zero with SIMD test` |
| `refactor:` | Code refactoring | `refactor: simplify shift implementation` |
| `test:` | Test-related | `test: add bit1024 edge case tests` |
| `chore:` | Build/tooling | `chore: add ARM32 CI support` |
| `spec:` | Specification changes | `spec: add ANDNOT to API specification` |

### Commit Message Format

```
<type>: <short description>

[optional body explaining the change]

[optional footer with breaking changes or references]

# Example:
feat: add ANDNOT operation for native SIMD support

Implements andnot() method using:
- _mm_andnot_si128 for SSE2
- _mm256_andnot_si256 for AVX2
- vbicq_u64 for NEON

Performance: ~2× faster than separate NOT + AND.

Closes #42
```

---

## Adding a New SIMD Backend

To add support for a new SIMD instruction set (e.g., AVX-512, SVE):

1. **Update `/specs/rfc/`** with the design proposal
2. Create `<backend>_ops.hpp` in `include/bitcal/`
3. Add platform detection and `BITCAL_HAS_*` macro in `config.hpp`
4. Add new backend to `simd_backend` enum
5. Add dispatch logic in `bitcal.hpp`
6. Add test cases in `/specs/testing/`
7. Update documentation in `/docs/`

### Example: Adding AVX-512

```cpp
// include/bitcal/avx512_ops.hpp
#pragma once
#if defined(BITCAL_HAS_AVX512)
#include <immintrin.h>

namespace bitcal::avx512 {
    // Implement 512-bit native operations
}
#endif
```

```cpp
// include/bitcal/config.hpp
#if defined(__AVX512F__) && defined(__AVX512BW__)
    #define BITCAL_HAS_AVX512 1
#endif
```

---

## Documentation

### Where to Put Documentation

| Type | Location |
|------|----------|
| User guides | `/docs/setup/`, `/docs/tutorials/` |
| API reference | `/docs/api/` |
| Architecture explanations | `/docs/architecture/` |
| Internal notes | `/docs/internal/` |
| Specifications | `/specs/` |

### Documentation Standards

- Keep documentation up-to-date with code changes
- Use clear, concise language
- Include code examples where helpful
- Link to related documents

### Multilingual Support

- Primary documentation is in English (`/docs/en/`)
- Chinese translations in `/docs/zh/`
- README: `README.md` (English) and `README.zh-CN.md` (Chinese)

---

## Release Process

### Version Numbering

We follow **Semantic Versioning** (MAJOR.MINOR.PATCH):

- **MAJOR**: Breaking API changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes (backward compatible)

### Release Checklist

1. **Update specs** if needed
2. **Update `CHANGELOG.md`** and `CHANGELOG.zh-CN.md`
3. **Update version numbers**:
   - `include/bitcal/config.hpp`: `BITCAL_VERSION_*`
   - `CMakeLists.txt`: `project(bitcal VERSION x.y.z)`
4. **Run full test suite** on all platforms
5. **Create git tag**: `git tag vx.y.z`
6. **Push**: `git push origin main --tags`
7. **Create GitHub Release** with release notes

---

## Getting Help

- 📖 **Full Documentation**: [GitHub Pages](https://lessup.github.io/bitcal/)
- 🐛 **Issue Tracker**: [GitHub Issues](https://github.com/LessUp/bitcal/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/LessUp/bitcal/discussions)

---

*Thank you for contributing to BitCal! Your efforts help make this library better for everyone.*
