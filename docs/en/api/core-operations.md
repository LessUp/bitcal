# Core Operations

This page documents the **namespace-level algorithms that ship today** in `<bitcal/bitcal.hpp>`.

## Public glossary

| Term | Meaning |
| --- | --- |
| Owning block | `bit_block<Bits>` returned by a result-producing algorithm |
| Non-owning view | `bit_view` / `const_bit_view` passed into an algorithm |
| Free algorithm | a namespace-level function such as `bit_and<Bits>()` |
| Current shipped surface | the symbols exported by today's public umbrella header |

## Current shipped bitwise algorithms

```cpp
template <std::size_t Bits>
bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept;

void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept;
```

### Use them when

- use `bit_and<Bits>()` when you want a fresh owning result block
- use `and_into()` when you already have writable storage for the AND result
- in both cases, the inputs and output must describe the same retained width

### Contract notes

- `Bits` must match the width of the result block you want back
- callers are responsible for passing views whose `word_count()` matches `bit_block<Bits>::word_count`
- `and_into()` expects `out.word_count()` to match both inputs

## Current shipped query algorithms

```cpp
bool is_zero(const const_bit_view value) noexcept;
std::uint64_t popcount(const const_bit_view value) noexcept;
```

### What they answer

| Need | Function |
| --- | --- |
| Is every bit clear? | `is_zero()` |
| How many bits are set? | `popcount()` |

## Status note on broader redesign names

Some redesign material still talks about `bit_or`, `bit_xor`, `bit_andnot`, `equals`, `shift_left`, and `shift_right` as target free-function shapes. Those names are **not** exported today as namespace-level algorithms, so this reference page does not document them as current usable surface.

## Composition example

```cpp
bitcal::bit_block<256> lhs;
bitcal::bit_block<256> rhs;

auto lhs_words = lhs.view();
lhs_words.data()[0] = 0x0FULL;
lhs_words.data()[2] = 0x00F0ULL;

auto rhs_words = rhs.view();
rhs_words.data()[0] = 0x0005ULL;
rhs_words.data()[1] = 0x0001ULL;
rhs_words.data()[2] = 0x00CCULL;

const auto& lhs_const = lhs;
const auto& rhs_const = rhs;

auto produced = bitcal::bit_and<256>(lhs_const.view(), rhs_const.view());
bitcal::bit_block<256> scratch;
bitcal::and_into(lhs_const.view(), rhs_const.view(), scratch.view());

const auto& produced_const = produced;
const auto& scratch_const = scratch;

auto empty = bitcal::is_zero(produced_const.view());
auto ones = bitcal::popcount(scratch_const.view());
```

## Choosing the right shape

| Need | Prefer |
| --- | --- |
| Return a fresh owning AND result | `bit_and<Bits>()` |
| Reuse writable storage for AND | `and_into()` |
| Query existing data without mutation | `is_zero()`, `popcount()` |

## Notes on assertions and safety

The shipped surface relies on caller-side width discipline. Treat word-count agreement as part of the API contract, not as an optional runtime convenience.

## Backend boundary reminder

These functions define the public behavior. The exact SIMD kernel that executes them is intentionally an implementation detail documented in [SIMD Dispatch](../architecture/simd-dispatch.md).

## Read next

- [Types Reference](./types.md)
- [SIMD Dispatch](../architecture/simd-dispatch.md)
- [Performance Baseline](../architecture/performance-baseline.md)
