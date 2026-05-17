# Migration Posture

BitCal vNext is not preserving the earlier public model for compatibility. The current direction is intentionally willing to break older expectations so the library can converge on a clearer public contract.

## Current migration stance

- `<bitcal/bitcal.hpp>` remains the only stable include seam
- the library is moving toward `bit_block`, `bit_view`, `const_bit_view`, and free algorithms
- old `bitarray`-centric reading paths should be treated as historical context, not as the center of future design

## Reader expectation

If you are evaluating BitCal now, evaluate the direction that is being actively specified and verified, not the one that happened to be documented first.
