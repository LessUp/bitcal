# Algorithm Contract

Algorithms are the behavioral center of BitCal vNext.

## Contract priorities

1. observable behavior matters more than backend branding
2. owner and view surfaces should remain equally usable where semantics allow
3. optimization detail should not become mandatory user knowledge

## Documentation shape

Reference pages should explain:

- what the operation conceptually does
- what kind of surface it consumes or returns
- which boundary conditions matter for the reader

They should avoid implying that undocumented implementation detail is part of the guarantee.

## Practical reading rule

If a question is about result semantics, this page should answer it. If a question is about kernel choice or microarchitectural trade-offs, the whitepaper and performance sections should take over.
