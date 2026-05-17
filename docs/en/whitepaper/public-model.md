# Public Model

BitCal vNext is converging on a public model with three explicit roles:

| Role | Public surface | Why it exists |
| --- | --- | --- |
| Owning storage | `bit_block<Bits>` | Keeps fixed-width storage explicit and inspectable |
| Borrowed access | `bit_view`, `const_bit_view` | Lets algorithms work over existing storage without forcing ownership |
| Behavior | free algorithms | Keeps the contract about observable results, not about one member-heavy type |

## Why the split matters

The split is not aesthetic. It protects four boundaries that were easy to blur in older bit-library designs:

1. storage versus borrowing
2. stable user contract versus replaceable backend details
3. algorithm semantics versus implementation strategy
4. migration cost versus convenience nostalgia

<FigureFrame title="Owner, view, algorithm split" caption="BitCal wants storage, borrowing, and behavior to remain individually understandable.">
  <svg viewBox="0 0 760 300" role="img" aria-label="Owner, view, algorithm split">
    <rect x="28" y="36" width="214" height="108" rx="18" data-fill="accent" />
    <text x="58" y="82" fill="currentColor" font-size="28" font-weight="700">bit_block</text>
    <text x="58" y="112" fill="currentColor" font-size="15">Owns fixed-width words and alignment</text>

    <rect x="272" y="36" width="214" height="108" rx="18" data-fill="surface" />
    <text x="304" y="82" fill="currentColor" font-size="28" font-weight="700">bit_view</text>
    <text x="304" y="112" fill="currentColor" font-size="15">Borrows mutable or const storage</text>

    <rect x="516" y="36" width="214" height="108" rx="18" data-fill="surface" />
    <text x="548" y="82" fill="currentColor" font-size="28" font-weight="700">algorithms</text>
    <text x="548" y="112" fill="currentColor" font-size="15">Express public behavior and fast paths</text>

    <path d="M242 90 L272 90" stroke-width="2.5" fill="none" data-stroke="primary" />
    <path d="M486 90 L516 90" stroke-width="2.5" fill="none" data-stroke="primary" />

    <rect x="112" y="188" width="536" height="70" rx="18" data-fill="surface" />
    <text x="146" y="228" fill="currentColor" font-size="24" font-weight="680">Stable public include seam</text>
    <text x="430" y="228" fill="currentColor" font-size="18">&lt;bitcal/bitcal.hpp&gt;</text>
  </svg>
</FigureFrame>

## Contract consequences

- `<bitcal/bitcal.hpp>` remains the only stable include seam.
- Backend selection is not the center of the user contract.
- Users should be able to reason about ownership and borrowing without reading internal headers.
- Documentation should teach the role model before listing operations.

## Why not keep one central class

A monolithic public center can look convenient, but it makes every future change more expensive:

- behavior and storage evolve together even when they should not
- borrowing becomes secondary instead of first-class
- fast-path discussion leaks into API language
- documentation turns into a tour of one giant type instead of a contract narrative

BitCal is choosing sharper boundaries over continuity with that older shape.
