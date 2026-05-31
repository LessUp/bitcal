# Interview Brief

This page is a short maintainer-facing and interview-facing summary of **project positioning, design spine, shipped scope, benchmark methodology, and current result boundaries**. It is not a new canonical whitepaper and it is not a new source of performance truth; go back to the [Whitepaper](/en/whitepaper/) for the full architecture argument and to [Performance](/en/performance/) for the complete retained evidence trail.

## One-sentence project description

**BitCal** is a **C++23, header-only, x86-64-first** bit-manipulation library currently in a **vNext / 4.0.0 redesign**. The main change is not “adding more bit APIs”; it is replacing the older `bitarray`-centered model with a new public contract built around **`bit_block<Bits>` + `bit_view` / `const_bit_view` + free algorithms**.

## My role

In interview terms, this page supports the following summary:

- I am the core designer and implementer of BitCal;
- I led the vNext public-model redesign, the tighter x86-64-first support posture, and the contract alignment between benchmarks, docs, and OpenSpec;
- My focus is not feature count, but the convergence of **public API, memory layout, dispatch boundaries, and performance evidence**.

## Why build it

BitCal is not trying to duplicate `<bit>`. The center of `<bit>` is **standardized bit operations on scalar values**. The center of `std::bitset` is **fixed-size container semantics**. BitCal is aimed at a different layer:

- how bit data is laid out for SIMD-friendly execution;
- how owning storage, borrowed views, and algorithm layers are separated cleanly;
- how backend dispatch and kernels can keep evolving without leaking into the public contract.

In other words, `<bit>` is primarily a **value-semantic toolkit**, while BitCal is a **data-layout and batch-execution model**.

## What is actually shipped today

The shipped vNext public surface is currently centered on:

- `bit_block<Bits>` for fixed-width owning storage;
- `bit_view` / `const_bit_view` for non-owning contiguous `uint64_t` word views;
- free algorithms: `bit_and`, `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `is_zero`, `popcount`, `shift_left`, and `shift_right`;
- `<bitcal/bitcal.hpp>` as the single stable include seam;
- an x86-64-first posture with AVX2 as the primary active fast path.

It is equally important to state the current boundaries honestly:

- **dynamic bitsets** are not part of the shipped vNext contract yet;
- **compressed bitmaps / adaptive compression** are not shipped public features;
- **serialization / deserialization** is not in the current public surface;
- **SSE2 / AVX-512 / NEON** have detection and lower-level reserved work, but the public algorithm path is currently centered on **AVX2 + scalar fallback**.

That is not an accidental gap list. It is an intentional engineering order: stabilize the block / view / algorithm contract first, then expand toward compression, dynamic layers, and more published backends.

## How the benchmark is designed

The public performance section reads retained baseline artifacts that live in the repository. In interview terms, the benchmark methodology can be summarized like this:

| Design point | Current choice |
| --- | --- |
| Comparator | `std::bitset` |
| Widths covered | 128 / 192 / 256 / 512 |
| Algorithms covered | `bit_and`, `bit_or`, `bit_xor`, `bit_andnot`, `popcount`, `equals`, `is_zero`, `shift_left`, `shift_right` |
| Statistic | `median ns/op` |
| Execution parameters | warmup 100, samples 25, iterations/sample 5000 |
| Output format | raw JSON plus summary JSON committed to the repo |
| Context binding | backend, CPU, and commit are always recorded |

The key idea is that the benchmark is treated as a **retained checkpoint**, not as a one-off speed claim:

1. it measures the currently shipped public algorithms, not retired compatibility layers;
2. every published number maps back to named public APIs;
3. every result stays attached to ISA, machine, and commit context;
4. losing results are preserved instead of being filtered out.

## How to describe the current benchmark results

The retained summary currently reflects:

- backend: **AVX2**
- CPU: **12th Gen Intel(R) Core(TM) i7-12700**
- profile: `retained-vnext-avx2`

In an interview, it is usually better to summarize the pattern instead of reciting the whole table:

| Width | Current takeaway | Example |
| --- | --- | --- |
| 128-bit | Most public operations still lose clearly to `std::bitset` | `bit_and<128>` about **10.59ns vs 0.53ns** |
| 192-bit | Still mostly behind, but the custom-width checkpoint is covered | `bit_xor<192>` about **10.03ns vs 1.52ns** |
| 256-bit | Moving into a mix of parity and isolated wins | `bit_or<256>` about **1.01ns vs 1.01ns**, `is_zero<256>` about **0.95ns vs 0.96ns** |
| 512-bit | Larger widths show mixed parity and losses | `bit_and<512>` about **2.01ns vs 2.01ns**, `shift_right<512>` about **11.05ns vs 10.04ns** |

The most important sentence is: **BitCal does not currently show a universal advantage over `std::bitset`.** That is not a weakness in the explanation; it is the accurate state of the project. At this stage, the bigger achievement is that the **public contract, correctness scope, dispatch boundary, and evidence chain** are all explicit and reproducible.

## Technical points worth emphasizing in an interview

If you want technical depth without overselling, the strongest points are:

1. **public model redesign**: moving from a monolithic `bitarray` model to owner / view / algorithm roles;
2. **header-only without giving up boundary control**: keeping `<bitcal/bitcal.hpp>` as the single stable seam;
3. **width-aware alignment strategy**: preserving natural SIMD alignment for 128 / 256 / 512-bit paths;
4. **keeping backend freedom in `detail/`**: not turning dispatch topology into compatibility debt;
5. **performance evidence discipline**: the benchmark exists as a retained baseline, not as a marketing table.

## Safe wording for future work

The following are reasonable to describe as **reserved design directions**, not as shipped features:

- a **dynamic bitset layer** above the block / view core;
- a **compressed bitmap layer**, likely informed by Roaring-style container splits or WAH/RLE-style encodings;
- pushing **AVX-512 / NEON** from lower-level reserved work into published dispatch paths;
- expanding methodology beyond the retained baseline with aligned / unaligned and owner / borrowed-view workload splits.

Go back to the [Whitepaper](/en/whitepaper/) for the architecture argument, or to [Performance](/en/performance/) for the full benchmark evidence, methodology, and claim boundaries.
