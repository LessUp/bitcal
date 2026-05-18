# Algorithm Contract

Algorithms are the behavioral center of BitCal vNext. The reference surface therefore explains them in terms of semantics, operands, and boundaries rather than as implementation trophies.

## Algorithm surface

| Concern | Reference question | Documentation expectation |
| --- | --- | --- |
| Inputs | Which owner or view forms are accepted? | State accepted public roles clearly. |
| Results | What is returned or mutated? | Describe observable effects before discussing speed. |
| Boundary conditions | Which width, aliasing, or shift-edge cases matter? | Make the semantic edge explicit. |
| Evidence linkage | Where do performance details live? | Hand readers to the performance section instead of embedding unsupported speed claims. |

## Contract rules and non-goals

Reference language should obey these rules:

1. Name the **observable behavior** first.
2. Treat owner and view forms as equally legitimate when semantics allow.
3. Avoid implying that kernel choice is part of the guarantee unless the page says so explicitly.
4. Point to [Performance](/en/performance/index) for benchmark interpretation and to [Research](/en/research/index) for external comparisons.

Non-goals for this page:

- cataloging every internal optimization trick;
- using benchmark wins as substitute API documentation;
- preserving legacy naming just because it used to be familiar.

## Reading algorithms with the rest of the site

If you are asking “what result should this operation produce?”, stay in Reference. If you are asking “why is this algorithm arranged this way?”, go back to the [Whitepaper](/en/whitepaper/index). If you are asking “how much faster is it today, and under which method?”, move to [Performance](/en/performance/index).
