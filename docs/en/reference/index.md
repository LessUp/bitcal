# Reference

The reference section exists to explain the public contract after the whitepaper has established the design logic. It is not a mirror of the source tree.

## Contract map

<ReadingPathGrid
  :items="[
    {
      title: 'Types and Views',
      href: '/en/reference/types-and-views',
      badge: 'Roles',
      summary: 'Read the owner and borrowing model before any algorithm details.',
      detail: 'This page defines what each public type is responsible for and what it is not.'
    },
    {
      title: 'Algorithm Contract',
      href: '/en/reference/algorithms',
      badge: 'Behavior',
      summary: 'Confirm how algorithms are described and which guarantees matter.',
      detail: 'Reference focuses on observable semantics, operands, and non-goals.'
    },
    {
      title: 'Research',
      href: '/en/research/index',
      badge: 'Context',
      summary: 'Use external references when you want to compare BitCal against adjacent systems.',
      detail: 'Research gives surrounding context; Reference defines BitCal’s own contract wording.'
    }
  ]"
/>

## What Reference freezes

Before you read page-by-page details, keep these public anchors fixed:

- the stable include seam at `<bitcal/bitcal.hpp>`;
- the owner / view / algorithm split that defines public roles before syntax details;
- the algorithm surface instead of internal kernel layout, because observable contract beats implementation trivia.

## Reading reference correctly

Keep three rules in mind while reading this section:

1. **Role before syntax** — understand owner, borrower, and algorithm responsibilities before scanning function shapes.
2. **Semantics before implementation** — dispatch and kernel choices are relevant only when they change documented behavior or evidence scope.
3. **Contract before convenience** — pages describe what the redesign is willing to maintain, not every helper that might appear during implementation churn.

If you need the architectural rationale first, return to the [Whitepaper](/en/whitepaper/index). If you want to understand the external context or trade-offs behind a contract choice, continue into [Research](/en/research/index).
