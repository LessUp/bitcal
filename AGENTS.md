# Project Philosophy: Spec-Driven Development (SDD)

This project strictly follows the **Spec-Driven Development (SDD)** paradigm. All code implementations must use the specification documents in the `/specs` directory as the single source of truth.

## Directory Context

### Specs Directory (`/specs`)
- `/specs/product/` — Product feature definitions and acceptance criteria (PRDs)
- `/specs/rfc/` — Technical design documents and architecture proposals (RFCs)
- `/specs/api/` — API interface specifications (OpenAPI, schemas)
- `/specs/db/` — Database schema definitions (DBML, migration specs)
- `/specs/testing/` — BDD test case specifications (Gherkin features)

### Documentation Directory (`/docs`)
- `/docs/setup/` — Environment setup and build guides
- `/docs/tutorials/` — User tutorials and usage examples
- `/docs/architecture/` — High-level architecture explanations (may link to `/specs/rfc/`)
- `/docs/assets/` — Static assets (images, diagrams, etc.)

### Root-Level Documents
- `README.md` — Project entry point (English)
- `README.zh-CN.md` — Chinese version of README
- `CONTRIBUTING.md` — Contribution guidelines
- `CHANGELOG.md` — Version changelog
- `LICENSE` — Open source license

## AI Agent Workflow Instructions

When you (AI) are asked to develop a new feature, modify an existing feature, or fix a bug, **you must strictly follow this workflow without skipping any steps**:

### Step 1: Review Specs
- Before writing any code, first read the relevant product specs, RFCs, and API definitions in the `/specs` directory.
- If the user's request conflicts with existing specs, **stop immediately** and point out the conflict. Ask the user whether the spec should be updated first.

### Step 2: Spec-First Update
- If this is a new feature, or if existing interfaces/database structures need to change, **you must first propose modifications to the relevant spec documents** (e.g., OpenAPI files, RFC documents).
- Wait for user confirmation of the spec changes before proceeding to code implementation.

### Step 3: Implementation
- When writing code, **100% comply** with the definitions in the specs (including variable names, API paths, data types, status codes, etc.).
- **Do not** add features that are not defined in the specs (No Gold-Plating).

### Step 4: Test Against Spec
- Write unit tests and integration tests based on the acceptance criteria described in `/specs`.
- Ensure test cases cover all boundary conditions described in the specs.

## Code Generation Rules

- Any externally exposed API changes **must** be synchronized with the corresponding spec files in `/specs/api/`.
- When encountering uncertain technical details, consult the architecture conventions in `/specs/rfc/`. Do not invent design patterns on your own.
- All documentation changes should be made in the `/docs` directory, keeping user docs separate from specs.

## Why This Matters

- **Prevent AI hallucination**: Forcing the first step to read `/specs` anchors the AI's thinking scope and prevents free-style implementation.
- **Enforce modification path**: Declaring "modify specs before code" ensures document-code synchronization.
- **Improve PR quality**: When AI generates Pull Requests, the implementation will be highly consistent with business logic because it is developed according to the acceptance criteria defined in specs.
