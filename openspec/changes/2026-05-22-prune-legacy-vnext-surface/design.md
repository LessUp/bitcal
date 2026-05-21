## Context

The vNext redesign already chose its public model: fixed-width owners, borrowed views, and free algorithms. The remaining problem is not direction; it is inconsistency. The codebase still ships legacy surfaces that suggest the old `bitarray` model is merely “demoted” instead of removed.

This change finishes that cleanup and uses the deletion to strengthen the real vNext surface.

## Goals

- Make the stable public seam ship only the vNext public model.
- Delete legacy executable/code paths that keep `bitarray` visible as a supported public shape.
- Fill the current implementation gap between the documented vNext algorithm surface and the real shipped functions.
- Keep retained performance evidence tied only to the shipped public surface.

## Non-Goals

- This change does not attempt universal performance wins over `std::bitset`.
- This change does not widen platform claims beyond the retained x86-64 evidence path.
- This change does not add a dynamic-width container model.

## Decisions

### 1. `bitarray` is removed, not hidden

`bitarray` is no longer treated as a tolerated legacy public type. The umbrella include must stop exporting it, and the header itself should be deleted from the shipped tree.

**Why:** A removed model that still ships as a public header keeps migration ambiguity alive.

### 2. Missing vNext free algorithms must be implemented before or alongside deletion

The vNext API spec already names `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, and `shift_right`. Those functions should be the retained replacement surface for the deleted `bitarray` behavior.

**Why:** Removing the old model without strengthening the new one would only reduce usefulness.

### 3. Built-in legacy benchmark lanes are removed from the retained tree

`benchmark_compare` remains the only built-in BitCal-vs-`std::bitset` comparison program. If broader comparisons are ever needed later, they should return as explicit opt-in research work under a new change.

**Why:** A permanently shipped legacy lane weakens the claim that the old model is gone.

### 4. Benchmark-like examples are not part of the product contract

The repository keeps `basic_usage.cpp` as the public consumption example and uses the retained benchmark path for performance evidence. Example programs that behave like ad hoc benchmark demos should be deleted.

**Why:** BitCal should keep one clear usage example path and one clear evidence path.

## Validation Implications

- Public tests must fail if free algorithms are missing.
- No retained test/example/benchmark target should require `bitarray`.
- Docs and README must describe the legacy API as removed from the shipped public surface.
- Retained benchmark artifacts must be generated only by `benchmark_compare`.
