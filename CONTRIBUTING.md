# Contributing to BitCal

BitCal is a compact C++23 header-only SIMD practice repository.
Contributions should keep the codebase small, truthful, and easy to iterate.

---

## Quick Principles

- Prefer simplification over layer growth.
- Keep public claims aligned with implementation and tests.
- Keep changes narrow and reversible.
- Remove dead code instead of preserving speculative structure.

---

## Development Environment

### Requirements

- C++23 compiler (GCC/Clang/MSVC with C++23 support)
- CMake 3.16+
- Git

### Build and Test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_BUILD_EXAMPLES=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

---

## Code and Test Scope

- Core library: `include/bitcal/`
- Public tests: `tests/test_bitcal.cpp`
- Examples: `examples/`
- Benchmarks: `benchmarks/`

Current backend contract is intentionally compact:
- `scalar`
- `avx2`

Do not introduce unsupported backend claims in docs/tests.

---

## Style

- Follow `.clang-format` and existing naming/style.
- Keep APIs `constexpr`/`noexcept` where appropriate.
- Prefer straightforward compile-time selection over runtime complexity.

---

## Commit Guidance

Use focused semantic prefixes when helpful:
- `feat:` `fix:` `refactor:` `test:` `docs:` `chore:`

Example:

```text
refactor: collapse backend surface to scalar+avx2 and remove dead SIMD files
```

---

## Documentation

- Keep README and docs consistent with shipped behavior.
- Do not maintain duplicate content tracks.
- Root `CHANGELOG.md` and `CHANGELOG.zh-CN.md` are release-history entry points.

---

## Release Checklist

1. Update changelog if user-facing behavior changed
2. Ensure version macros in `include/bitcal/config.hpp` are correct
3. Run full local build + test
4. Tag and publish release artifacts

---

## Getting Help

- GitHub Issues / Discussions
- Project docs site
