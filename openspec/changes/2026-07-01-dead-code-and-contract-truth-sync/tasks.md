## 1. Dead code removal

- [x] 1.1 Delete `include/bitcal/sse_ops.hpp`, `avx_ops.hpp`, `avx512_ops.hpp`, `neon_ops.hpp`
- [x] 1.2 Delete `include/bitcal/backend_ops.def`

## 2. Contract truth sync

- [x] 2.1 Remove `simd_backend` enum and `get_default_simd_backend()` from `config.hpp`
- [x] 2.2 Fix `detail/backend.hpp` self-containment (include `config.hpp`)

## 3. Documentation truth sync

- [x] 3.1 Update `AGENTS.md`: C++23 baseline, `bit_block`/`bit_view`/algorithms as core abstraction
- [x] 3.2 Update `CONTRIBUTING.md`: C++23 compiler requirement, SIMD backend guidance aligned to `detail/x64_dispatch.hpp`

## 4. OpenSpec truth sync

- [x] 4.1 Check all completed tasks in `2026-05-14-vnext-cpp23-x64-redesign/tasks.md`
- [x] 4.2 Check all completed tasks in `2026-05-22-prune-legacy-vnext-surface/tasks.md`
- [x] 4.3 Add this OpenSpec change recording the dead-code and contract-truth sync

## 5. Verification

- [x] 5.1 Build with retained C++23 path and run ctest
- [x] 5.2 Strict warnings build (no new warnings beyond known benign popcount intrinsic sign-conversion)
