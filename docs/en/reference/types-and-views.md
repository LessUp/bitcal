# Types and Views

The reference surface is moving toward an explicit ownership model rather than a catalog of convenience methods.

## Role table

| Surface | Ownership | Intended use |
| --- | --- | --- |
| `bit_block<Bits>` | owning | fixed-width storage with explicit size and layout |
| `bit_view` | borrowed mutable | operate on existing storage without copying |
| `const_bit_view` | borrowed read-only | inspect or feed existing storage into algorithms |

## Reference rules

- document role before syntax
- keep include guidance simple: use `<bitcal/bitcal.hpp>`
- describe detail headers as implementation material, not supported entry points

## Why this page exists

If a reader cannot tell who owns storage and who merely observes it, the rest of the contract becomes blurry. This page should remove that ambiguity first.
