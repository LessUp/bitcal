## Context

The vNext public model is already small enough to optimize: fixed-width owning blocks, borrowed views, and free algorithms. The next issue is credibility. BitCal must make a clear distinction between portable scalar correctness, x86 SIMD acceleration, and unsupported accelerator/platform claims.

## Goals

- Make the first optimization wave improve only the retained x86-64 CPU backend.
- Route public runtime `popcount`, `is_zero`, and `equals` through the same x86 dispatch seam as binary bitwise operations.
- Preserve `constexpr` behavior by using scalar loops during constant evaluation.
- Gate AVX-512 usage on exact compiler feature macros, not on broad AVX-512 wording.
- Update tests and specs so users can tell what is accelerated and what is merely buildable or out of scope.

## Non-Goals

- No runtime CPU-feature dispatch in this change.
- No CUDA, GPU, or NPU backend.
- No ARM NEON/SVE/SVE2 backend activation.
- No LoongArch LSX/LASX, Zhaoxin-specific, Kunpeng-specific, or Phytium-specific acceleration claim.
- No shift-path vectorization in this change; shifts remain scalar and are documented as deferred.

## Decisions

### 1. Evidence levels are part of the product contract

BitCal will use separate support language:

| Level | Meaning |
| --- | --- |
| Buildable | The code compiles for the target in retained CI or a documented command. |
| Correct | The retained tests execute on that target or an equivalent runner. |
| SIMD-accelerated | The retained public algorithm path uses a target-specific SIMD implementation. |
| Retained benchmark evidence | Structured benchmark artifacts exist for the target/backend/commit. |

### 2. First phase remains compile-target ISA optimization

The x86 backend continues to be selected by compile-time feature macros such as `__AVX2__`, `__AVX512F__`, and `__AVX512VPOPCNTDQ__`.

This means a binary compiled for AVX2 or AVX-512 must run on matching CPUs. It is not a single portable binary that chooses ISA at runtime.

### 3. Public query/counting algorithms dispatch at runtime only outside constant evaluation

`detail::popcount_words`, `detail::is_zero_words`, and `detail::equals_words` remain usable in constant-evaluation contexts through scalar loops. At normal runtime they may call `detail::*_x64` helpers.

### 4. AVX-512 is feature-specific

AVX-512 bitwise/equality/zero checks may use AVX-512F. Popcount may use AVX512VPOPCNTDQ only when that macro is present. AVX-512F alone is not enough to claim vector popcount.

### 5. Broader platform work is deferred

ARM, domestic CPU SIMD, CUDA/GPU, and NPU work require new OpenSpec changes with their own validation and benchmark evidence. The current change may document boundaries, but it must not implement or imply support for those targets.

## Validation Implications

- Internal tests must cover x86 helper behavior for word counts that exercise tail-only, exact-vector, vector-plus-tail, and multi-vector cases.
- Public tests must continue proving observable behavior through `<bitcal/bitcal.hpp>`.
- The benchmark path may remain x86-64 retained evidence; docs must not promote it to ARM/GPU/NPU evidence.

