# Core Operations

This page distinguishes the **persistent vNext API contract** from **current helper entry points** that happen to exist in the shipped headers.

## Public glossary

| Term | Meaning |
| --- | --- |
| Owning block | `bit_block<Bits>` returned by a result-producing algorithm |
| Non-owning view | `bit_view` / `const_bit_view` passed into an algorithm |
| Free algorithm | a namespace-level function such as `bit_and<Bits>()` |
| Persistent contract | the API promised by `openspec/specs/api/bitcal-public-api.md` |

## Persistent bitwise result algorithms

```cpp
template <std::size_t Bits>
bit_block<Bits> bit_and(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_or(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_xor(const const_bit_view lhs, const const_bit_view rhs) noexcept;

template <std::size_t Bits>
bit_block<Bits> bit_andnot(const const_bit_view lhs, const const_bit_view rhs) noexcept;
```

### Use them when

- you want a fresh owning result block
- the result width is known at compile time
- both input views describe the same retained width

### Contract notes

- `Bits` must match the width of the result block you want back
- callers are responsible for passing views whose `word_count()` matches `bit_block<Bits>::word_count`
- the persistent API contract treats these free algorithms, not a member-heavy value type, as the architectural center

## Persistent comparison and query algorithms

```cpp
bool equals(const const_bit_view lhs, const const_bit_view rhs) noexcept;
bool is_zero(const const_bit_view value) noexcept;
std::uint64_t popcount(const const_bit_view value) noexcept;
```

### What they answer

| Need | Function |
| --- | --- |
| Are two views bitwise identical? | `equals()` |
| Is every bit clear? | `is_zero()` |
| How many bits are set? | `popcount()` |

## Persistent shift algorithms

```cpp
template <std::size_t Bits>
bit_block<Bits> shift_left(const const_bit_view value, int count) noexcept;

template <std::size_t Bits>
bit_block<Bits> shift_right(const const_bit_view value, int count) noexcept;
```

### Boundary behavior

| Condition | Behavior |
| --- | --- |
| `count == 0` | returns an unchanged result |
| `count >= Bits` | returns an all-zero result |
| `count < 0` | undefined behavior |

## Current helper in shipped headers: `and_into()`

```cpp
void and_into(const const_bit_view lhs, const const_bit_view rhs, bit_view out) noexcept;
```

`and_into()` is useful when you already have writable storage and want an in-place AND write. It exists in the current headers, but it is **not** part of the persistent API spec documented in `openspec/specs/api/bitcal-public-api.md`.

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

auto produced_and = bitcal::bit_and<256>(lhs_const.view(), rhs_const.view());
auto produced_or = bitcal::bit_or<256>(lhs_const.view(), rhs_const.view());
auto produced_xor = bitcal::bit_xor<256>(lhs_const.view(), rhs_const.view());
auto produced_andnot = bitcal::bit_andnot<256>(lhs_const.view(), rhs_const.view());
const auto& produced_and_const = produced_and;
const auto& produced_or_const = produced_or;
const auto& produced_xor_const = produced_xor;
const auto& produced_andnot_const = produced_andnot;

auto shifted = bitcal::shift_left<256>(produced_or_const.view(), 4);
const auto& shifted_const = shifted;

auto same = bitcal::equals(produced_and_const.view(), produced_xor_const.view());
auto empty = bitcal::is_zero(produced_andnot_const.view());
auto ones = bitcal::popcount(shifted_const.view());

bitcal::bit_block<256> scratch;
bitcal::and_into(lhs_const.view(), rhs_const.view(), scratch.view());
```

## Choosing the right shape

| Need | Prefer |
| --- | --- |
| Return a fresh owning bitwise result | `bit_and<Bits>()`, `bit_or<Bits>()`, `bit_xor<Bits>()`, `bit_andnot<Bits>()` |
| Compare or query existing data | `equals()`, `is_zero()`, `popcount()` |
| Return a shifted owning result | `shift_left<Bits>()`, `shift_right<Bits>()` |
| Reuse writable storage in current headers | `and_into()` helper |

## Notes on assertions and safety

The retained contract relies on caller-side width discipline. Treat word-count agreement and non-negative shift counts as part of the API contract, not as optional runtime conveniences.

## Backend boundary reminder

These functions define the public behavior. The exact SIMD kernel that executes them is intentionally an implementation detail documented in [SIMD Dispatch](../architecture/simd-dispatch.md).

## Read next

- [Types Reference](./types.md)
- [SIMD Dispatch](../architecture/simd-dispatch.md)
- [Performance Baseline](../architecture/performance-baseline.md)
