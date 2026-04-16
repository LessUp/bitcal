# Contributing to BitCal

Thank you for your interest in contributing to BitCal! This document explains how to get involved.

## Spec-Driven Development

BitCal follows **Spec-Driven Development (SDD)**. All new features and changes must be defined in the `/specs` directory before implementation.

### Workflow

1. **Review existing specs** in `/specs/` to understand current requirements
2. **Propose spec updates** before writing code (PRDs, RFCs, API specs)
3. **Get spec approval** from maintainers
4. **Implement** according to the approved spec
5. **Write tests** that verify against spec acceptance criteria

### Spec Directory Structure

| Directory | Purpose |
|-----------|---------|
| `/specs/product/` | Product feature definitions and acceptance criteria |
| `/specs/rfc/` | Technical design documents and architecture proposals |
| `/specs/api/` | Public API specifications |
| `/specs/testing/` | Test coverage requirements |

See [AGENTS.md](AGENTS.md) for detailed AI workflow instructions.

## Development Environment

### Requirements

- C++17 compiler (GCC 7+, Clang 6+, MSVC 2017+)
- CMake 3.16+

### Building

```bash
git clone https://github.com/LessUp/bitcal.git
cd bitcal
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Running Tests

```bash
./tests/test_bitcal
```

## Code Style

- **File naming**: lowercase + underscore (`scalar_ops.hpp`)
- **Namespace**: `bitcal::`
- **Class names**: lowercase + underscore (`bitarray`)
- **Function names**: lowercase + underscore (`shift_left`)

## Commit Convention

Use semantic commit messages:

```
feat: add new feature
fix: fix a bug
docs: documentation update
perf: performance optimization
refactor: code refactoring
test: test-related changes
chore: build/tooling changes
```

## Adding a New SIMD Backend

1. Create `<backend>_ops.hpp` in `include/bitcal/`
2. Add platform detection and `BITCAL_HAS_*` macro in `config.hpp`
3. Add new backend to `simd_backend` enum
4. Add dispatch logic in `bitcal.hpp`
5. Add test cases

## Documentation

- User documentation goes in `/docs/`
- Technical specs go in `/specs/`
- Keep API reference up-to-date in `/docs/api/`
- Architecture explanations in `/docs/architecture/`

## Release Process

1. Update `CHANGELOG.md`
2. Update version numbers (`config.hpp`, `CMakeLists.txt`)
3. Create tag: `git tag v2.x.x`
4. Push: `git push --tags`
