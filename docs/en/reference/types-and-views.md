# Types and Views

The public reference begins with roles, because the rest of the contract is hard to read if ownership and borrowing are blurry.

## Public type roles

| Surface | Ownership status | Contract reading |
| --- | --- | --- |
| `bit_block<Bits>` | Owning | Fixed-width storage object with explicit size, layout, and word-level identity. |
| `bit_view` | Borrowed mutable | Mutable window over existing storage; does not imply ownership transfer. |
| `const_bit_view` | Borrowed read-only | Read-only window used for inspection, comparison, and algorithm input. |
| `<bitcal/bitcal.hpp>` | Stable entry point | The single supported include seam for the public surface. |

## Ownership and borrowing contract

Readers should be able to infer these rules without guessing:

- ownership lives with `bit_block<Bits>`;
- borrowing lives with `bit_view` and `const_bit_view`;
- borrowed views are normal inputs to algorithms, not second-class adapters;
- include advice stays simple: include `<bitcal/bitcal.hpp>` unless you are working on internals.

## Contract notes worth preserving

- Width is part of the public story, because the redesign is intentionally fixed-width oriented.
- `bit_block<Bits>` keeps width-appropriate storage alignment rather than forcing one x86-wide alignment onto every width.
- Borrowing must be documented clearly enough that aliasing and mutability questions can be answered at the reference layer.
- `equals()` compares full view extents: different `word_count()` values are not equal, even if the shorter prefix matches.
- Detail headers may exist, but they are not automatically public commitments.

## Non-guarantees

This page does not promise:

- that current internal storage layout is a stable ABI contract;
- that every intermediate helper type should appear in public documentation;
- that the older monolithic public shape remains the recommended mental model.
