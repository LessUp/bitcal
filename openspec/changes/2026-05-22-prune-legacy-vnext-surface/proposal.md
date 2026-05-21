## Why

BitCal already documents a vNext public model centered on `bit_block`, `bit_view`, `const_bit_view`, and free algorithms, but the shipped tree still keeps a second architectural center alive:

- `<bitcal/bitcal.hpp>` still exports the legacy `bitarray` public header;
- the build still produces `benchmark_compare_legacy`, which frames a removed public model as a retained executable lane;
- the vNext API spec already lists free algorithms that the implementation has not fully shipped yet.

That leaves the repository in a wobbling state where the docs, specs, and code disagree about what BitCal actually is.

## What Changes

- **BREAKING**: remove `bitarray.hpp` from the stable umbrella include and delete the legacy `bitarray` public type from the shipped vNext surface.
- **BREAKING**: remove the built-in `benchmark_compare_legacy` lane and stop presenting a retained legacy benchmark path in docs.
- Promote the missing vNext free algorithms (`bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, `shift_right`) from spec-only surface into shipped implementation and tests.
- Remove low-value benchmark-like example code that keeps the old “performance demo” posture alive outside the retained benchmark path.

## Impact

- Affects OpenSpec for the public API and testing boundary.
- Affects the stable include seam `<bitcal/bitcal.hpp>` and shipped headers under `include/bitcal/`.
- Affects retained benchmark/docs posture because `benchmark_compare` becomes the only built-in comparison path.
- Preserves the v4.0.0 breaking-release target and no-compatibility migration stance.
