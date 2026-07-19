# BitCal

A header-only C++23 bit-operations library. x86-64-first. Experimental.

[中文 README](README.md) — the Chinese README is the primary entry point and is *not* a translation of this file. The two documents target different audiences: this one is written for contributors and design reviewers; the Chinese one for users.

---

## Project Intent

BitCal is a personal practice repository for exploring SIMD bit-operations on fixed-width blocks. The design priority is **small, readable, fast to iterate** — not generality. We delete complexity on sight.

Non-goals:

- General-purpose big-integer arithmetic.
- Cross-platform SIMD portability (ARM / WebAssembly / RISC-V).
- Runtime backend selection.
- Backward compatibility across major versions.

## Architecture

Three-layer public model, all surfaced through `<bitcal/bitcal.hpp>`:

| Layer | Type | Owns storage? |
|-------|------|---------------|
| Owning block | `bit_block<Bits>` | Yes (fixed-width, `Bits % 64 == 0`, `Bits >= 64`) |
| Mutable view | `bit_view` | No |
| Read-only view | `const_bit_view` | No |

Free-function algorithms operate on views; returning forms (`bit_and<Bits>()` etc.) produce a fresh `bit_block<Bits>`, in-place forms (`and_into()` etc.) write into a caller-supplied `bit_view`.

### Header layout

```
include/bitcal/
├── bitcal.hpp              # sole public umbrella
├── config.hpp              # version macros, arch/intrinsics gating, alignment policy
├── bit_view.hpp            # const_bit_view + bit_view
├── bit_block.hpp           # owning storage
├── algorithms.hpp          # public free functions
└── detail/
    └── word_ops.hpp        # scalar kernels + AVX2 dispatch + view-level wrappers
```

### Backend selection

Backend is fixed at compile time by `BITCAL_HAS_AVX2` (derived from compiler flags). There is no runtime dispatch and no abstract backend interface — the dispatch is a single `if constexpr`-style fork inside `detail::word_ops.hpp`.

- `BITCAL_HAS_AVX2 == 1`: blocks of `>= 256` bits use 4-word `__m256i` chunks; tail words handled scalarly.
- `BITCAL_HAS_AVX2 == 0`: everything goes through the scalar loop.

Alignment policy mirrors this: `get_optimal_alignment<Bits>()` returns 32 only when `Bits >= 256 && BITCAL_HAS_AVX2`; otherwise natural `uint64_t` alignment. This avoids dead zero-initialization stores on the scalar path.

## Build

```bash
# AVX2 path
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON -DBITCAL_NATIVE_ARCH=ON
cmake --build build -j"$(nproc)"
ctest --test-dir build --output-on-failure

# Scalar path (validates the BITCAL_HAS_AVX2 == 0 branch)
cmake -S . -B build-scalar -DCMAKE_BUILD_TYPE=Release -DBITCAL_BUILD_TESTS=ON \
      -DBITCAL_NATIVE_ARCH=OFF -DCMAKE_CXX_FLAGS="-mno-avx2"
cmake --build build-scalar -j"$(nproc)"
ctest --test-dir build-scalar --output-on-failure
```

CI runs both paths plus a sanitizer job. See [`.github/workflows/ci.yml`](.github/workflows/ci.yml).

## Testing Strategy

- One test binary, one test file: `tests/test_bitcal.cpp`.
- Lightweight macro framework in `tests/support/test_macros.hpp` — no third-party test dependency.
- Random case generators + a hand-written reference model live in `tests/support/`.
- Coverage target: every retained algorithm × {128, 256, 512} deterministic + a reference-checked random pass. Boundary cases (`shift(0)`, `shift(Bits-1)`, `shift(Bits)`, `shift(SIZE_MAX)`) are explicit.

If you add a public algorithm, also add a reference implementation in `tests/support/reference_model.hpp` and a random-differential test.

## Benchmarks

`benchmarks/` contains two executables (build with `-DBITCAL_BUILD_BENCHMARKS=ON`):

- `bitcal_benchmark`: self-timing of BitCal operations.
- `benchmark_compare`: BitCal vs `std::bitset<N>`.

Both use a self-contained `std::chrono` harness (`benchmark_harness.hpp`) — no google-benchmark dependency. Results are generated locally and gitignored; do not commit them.

## Conventions for Contributors

- **One truth per concept**: code lives in `include/bitcal/`, behavior in `tests/test_bitcal.cpp`, version in `include/bitcal/config.hpp`, user-facing description in `README.md` (Chinese), changelog in `CHANGELOG.md` (Chinese).
- **Code-first, small commits**: implement + test + docs in one closed loop.
- **Delete dead code on sight**: no reserved backends, no unused macros, no speculative abstractions.
- **No runtime backend selection**: the compile-time fork is the contract.
- **No compatibility layer across major versions**: breaking changes are expected in vNext.
- **Chinese is the primary documentation language**; this English README exists for design contributors and is allowed to drift in content from the Chinese README.

See [`AGENTS.md`](AGENTS.md) for the full operating manual.
