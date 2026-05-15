# Types Reference

This page records the persistent vNext type contract rather than every convenience currently visible in headers.

## Public glossary

| Term | Public API | Role |
| --- | --- | --- |
| Owning block | `bit_block<Bits>` | Fixed-width storage that owns contiguous words |
| Non-owning view | `bit_view` | Mutable borrowed access to words owned elsewhere |
| Read-only view | `const_bit_view` | Borrowed read-only access |
| Backend boundary | `backend_kind` | Documented backend vocabulary, not the storage model |

## Stable include

```cpp
#include <bitcal/bitcal.hpp>
```

That umbrella header is the stable public include seam for the vNext model.

## Owning block: `bit_block<Bits>`

```cpp
template <std::size_t Bits>
class bit_block;
```

### Constraints

- `Bits >= 64`
- `Bits` must be a multiple of 64

### Public constants

| Constant | Meaning |
| --- | --- |
| `bit_block<Bits>::bits` | total bit width |
| `bit_block<Bits>::word_count` | number of 64-bit words |

### Persistent operations

```cpp
bit_block() noexcept;
bit_view view() noexcept;
const_bit_view view() const noexcept;
```

The persistent API spec does **not** promise helper members such as `storage_alignment`, `from_words(...)`, `bit_block::word(...)`, or `copy_words_to(...)`.

## Non-owning view: `bit_view`

```cpp
class bit_view;
```

### Persistent view contract

```cpp
std::uint64_t* data() noexcept;
std::size_t word_count() const noexcept;
std::uint64_t word(std::size_t index) const noexcept;
```

Use `bit_view` when you want algorithms or calling code to mutate existing contiguous word storage.

## Read-only view: `const_bit_view`

```cpp
class const_bit_view;
```

### Persistent view contract

```cpp
const std::uint64_t* data() const noexcept;
std::size_t word_count() const noexcept;
std::uint64_t word(std::size_t index) const noexcept;
```

Use `const_bit_view` for query algorithms and read-only inputs.

## Lifetime rule for views

Views never own memory. The caller is responsible for ensuring the underlying words outlive the view.

In practice, the stable pattern is:

- own storage in `bit_block<Bits>`
- borrow through `view()`
- pass those views into free algorithms

## Backend boundary

```cpp
enum class backend_kind {
    scalar,
    sse2,
    avx2,
    avx512
};
```

These names are useful for architecture discussions and validation records. The persistent API spec does not currently promise extra backend-selection helpers.

## How the pieces fit together

```cpp
bitcal::bit_block<256> owned;
auto writable = owned.view();
writable.data()[0] = 0xFULL;

const auto& owned_const = owned;
auto readable = owned_const.view();
auto first_word = readable.word(0);
auto words = readable.word_count();
```

That is the public type story promised today: own with a block, borrow with views, and avoid coupling to convenience helpers that are outside the persistent spec.

## Read next

- [Core Operations](./core-operations.md)
- [Architecture Overview](../architecture/overview.md)
- [vNext Whitepaper](../architecture/vnext-whitepaper.md)
