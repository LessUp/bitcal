# Public Model

BitCal's public story only works if readers can separate ownership, borrowing, and behavior without reading internal headers.

## Owner / view / algorithm model

| Role | Primary surface | Public responsibility |
| --- | --- | --- |
| Owner | `bit_block<Bits>` | Own fixed-width storage, width, and layout-sensitive operations. |
| Mutable borrower | `bit_view` | Provide non-owning mutable access over existing storage. |
| Read-only borrower | `const_bit_view` | Provide non-owning read-only access for inspection and algorithms. |
| Behavioral layer | free algorithms | Describe observable operations without turning one type into a monolith. |

This model is deliberately explicit. A reader should be able to tell who owns bytes, who merely borrows them, and where the operation semantics live.

## Stable include seam

`<bitcal/bitcal.hpp>` remains the single supported include seam.

That decision keeps three things aligned:

- header-only consumption stays simple;
- internal header layout can continue evolving during the redesign;
- reference documentation can point to one public entry instead of implying that detail headers are stable contracts.

## Contract boundaries

The public model promises role clarity, not backend permanence.

What readers may rely on:

- owner and view roles are first-class public concepts;
- algorithms operate over those roles rather than hiding everything behind one stateful object;
- the stable include seam remains `<bitcal/bitcal.hpp>`.

What readers should *not* infer:

- that current internal header names define extra stable entry points;
- that a specific dispatch topology or intrinsic sequence is itself public API;
- that old `bitarray` naming remains the architectural center.

## Why the older center gives way

A monolithic public center often looks simpler at first, but it blurs exactly the boundaries BitCal now wants to sharpen:

- storage policy and behavior become entangled;
- borrowing becomes secondary instead of normal;
- backend talk leaks into reference language;
- future algorithm growth feels like method sprawl instead of clear composition.

The new model chooses explicit roles because that is easier to test, document, benchmark, and evolve.
