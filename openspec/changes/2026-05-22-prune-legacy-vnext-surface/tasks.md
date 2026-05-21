## 1. Spec truth

- [ ] 1.1 Add this OpenSpec change for legacy surface pruning
- [ ] 1.2 Update API/testing deltas so the shipped umbrella header and retained validation path no longer allow `bitarray`

## 2. VNext algorithm completion

- [ ] 2.1 Add failing tests for the missing free algorithms
- [ ] 2.2 Implement `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, and `shift_right`
- [ ] 2.3 Re-run focused tests until the new surface is green

## 3. Legacy deletion

- [ ] 3.1 Remove `bitarray` from `<bitcal/bitcal.hpp>` and delete `include/bitcal/bitarray.hpp`
- [ ] 3.2 Remove `benchmark_compare_legacy` from the build and delete its source
- [ ] 3.3 Remove `examples/performance_comparison.cpp` and keep only the usage-oriented example path

## 4. Evidence and docs alignment

- [ ] 4.1 Refresh `benchmark_compare` and retained artifacts for the shipped vNext algorithm surface
- [ ] 4.2 Update docs, README, and status pages to describe the cleaned public surface
- [ ] 4.3 Run retained C++ and docs verification paths
