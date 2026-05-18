# Dispatch and Kernels

BitCal documents dispatch as a support boundary, not as a personality trait. Readers need to know where implementation freedom begins, because that line defines which changes are architectural and which are merely backend work.

## Dispatch boundary

<FigureFrame title="Public contract versus implementation freedom" caption="The contract line keeps user-visible roles and semantics above dispatch policy and kernel replacement.">
  <svg viewBox="0 0 760 330" role="img" aria-label="Dispatch boundary diagram">
    <rect x="40" y="28" width="680" height="64" rx="18" data-fill="accent" />
    <text x="70" y="68" fill="currentColor" font-size="26" font-weight="700">Public contract</text>
    <text x="280" y="68" fill="currentColor" font-size="16">include seam · owner/view roles · algorithm semantics</text>
    <path d="M60 132 L700 132" stroke-width="3" fill="none" data-stroke="primary" />
    <text x="70" y="122" fill="currentColor" font-size="14">dispatch boundary</text>
    <rect x="40" y="164" width="210" height="96" rx="18" data-fill="surface" />
    <text x="70" y="202" fill="currentColor" font-size="22" font-weight="680">dispatch policy</text>
    <text x="70" y="230" fill="currentColor" font-size="14">feature detection, routing, fallback choice</text>
    <rect x="276" y="164" width="210" height="96" rx="18" data-fill="surface" />
    <text x="308" y="202" fill="currentColor" font-size="22" font-weight="680">kernel families</text>
    <text x="308" y="230" fill="currentColor" font-size="14">AVX2 primary path, scalar portability floor</text>
    <rect x="512" y="164" width="210" height="96" rx="18" data-fill="surface" />
    <text x="546" y="202" fill="currentColor" font-size="22" font-weight="680">micro-tuning</text>
    <text x="546" y="230" fill="currentColor" font-size="14">intrinsic selection, unrolling, alignment strategy</text>
  </svg>
</FigureFrame>

## Kernel families

| Kernel family | Current documentation stance | Why it is separated |
| --- | --- | --- |
| Scalar floor | Retained portability baseline | Gives the project a correctness floor even when no wide path is active. |
| AVX2 path | Primary evidence and optimization target | Current benchmark posture is concentrated here. |
| Future x86 variants | Mention only as design direction | They should not be overclaimed before retained evidence exists. |

## Support boundary

Dispatch and kernels must respect the documented support posture:

- **Linux / Windows x86-64** are the primary optimization and validation targets.
- **Secondary targets** may remain buildable, but must not inherit stronger claims than the retained evidence supports.
- **Public documentation** should mention backend detail only when it changes user-visible semantics, benchmark scope, or support boundaries.

## What readers should carry forward

The dispatch layer is allowed to change aggressively as long as the public role model, algorithm semantics, and include seam remain coherent. That is why the next stop after this page is the [Performance section](/en/performance/): benchmark claims belong below this boundary, not above it.
