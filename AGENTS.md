# Project Philosophy: Spec-Driven Development (SDD)

This project strictly follows the **Spec-Driven Development (SDD)** paradigm. All code implementations must use the specification documents in the `/specs` directory as the single source of truth.

---

## Directory Context

### Specs Directory (`/specs`)

The `/specs` directory contains all specification documents that define what the software should do and how it should be implemented.

| Directory | Purpose | Contents |
|-----------|---------|----------|
| `/specs/product/` | Product Requirements Documents (PRD) | Feature definitions, acceptance criteria, user requirements |
| `/specs/rfc/` | Request for Comments (RFC) | Technical design documents, architecture proposals |
| `/specs/api/` | API Specifications | Public interface definitions, method signatures, contracts |
| `/specs/testing/` | Test Specifications | BDD test cases, coverage requirements, test strategies |

> **Note:** This project (BitCal) is a header-only C++ library and does not use a database. The `/specs/db/` directory is not applicable.

### Documentation Directory (`/docs`)

The `/docs` directory contains user-facing and developer documentation.

| Directory | Purpose |
|-----------|---------|
| `/docs/setup/` | Installation guides, build instructions, migration guides |
| `/docs/api/` | API reference documentation (human-readable) |
| `/docs/architecture/` | High-level architecture explanations |
| `/docs/tutorials/` | User tutorials and usage examples |
| `/docs/assets/` | Static assets (images, diagrams, UML) |
| `/docs/internal/` | Internal development notes (not for end users) |
| `/docs/en/` | English documentation |
| `/docs/zh/` | Chinese documentation (中文文档) |

### Root-Level Documents

| File | Purpose |
|------|---------|
| `README.md` | Project entry point (English) |
| `README.zh-CN.md` | Chinese version of README (简体中文版) |
| `CONTRIBUTING.md` | Contribution guidelines |
| `CHANGELOG.md` | Version changelog |
| `CHANGELOG.zh-CN.md` | Chinese version of changelog |
| `LICENSE` | Open source license |
| `AGENTS.md` | This file — AI agent instructions |

---

## AI Agent Workflow Instructions

When you (AI) are asked to develop a new feature, modify an existing feature, or fix a bug, **you must strictly follow this workflow without skipping any steps**:

### Step 1: Review Specs (审查与分析)

Before writing any code:

1. **Read the relevant specifications** in `/specs` directory:
   - Product specs (`/specs/product/`) for feature requirements
   - RFCs (`/specs/rfc/`) for architectural decisions
   - API specs (`/specs/api/`) for interface contracts

2. **Identify conflicts**: If the user's request conflicts with existing specs:
   - **STOP immediately**
   - Point out the specific conflict
   - Ask the user whether the spec should be updated first

```
Example conflict detection:
- User wants to add bit2048 type
- Spec in /specs/product/ only lists bit64-bit1024
- Action: Point out this is beyond current spec scope, ask to update spec first
```

### Step 2: Spec-First Update (规范优先)

If this is a **new feature** or requires **interface changes**:

1. **Propose spec modifications first**:
   - Create or update documents in `/specs/product/` for feature requirements
   - Create or update RFCs in `/specs/rfc/` for design decisions
   - Create or update `/specs/api/` for API changes
   - Create or update `/specs/testing/` for test requirements

2. **Wait for user confirmation**:
   - Do not proceed to code implementation until specs are approved
   - Use `AskUserQuestion` tool if clarification is needed

3. **Spec update format**:
   ```markdown
   ## Proposed Changes to [spec-file.md]
   
   ### Addition/Modification
   - [ ] New feature: ...
   - [ ] Modified behavior: ...
   
   ### Impact Analysis
   - Affected files: ...
   - Breaking changes: Yes/No
   ```

### Step 3: Implementation (代码实现)

When writing code, **100% comply** with the specifications:

1. **Naming conventions**: Follow exact names from spec
   - Class names: `bitarray` (not `BitArray` or `Bitarray`)
   - Type aliases: `bit64`, `bit128`, `bit256`, `bit512`, `bit1024`
   - Namespace: `bitcal`

2. **API contracts**: Match exact signatures from `/specs/api/`
   - Parameter types and order
   - Return types
   - Error handling behavior

3. **No Gold-Plating**: Do not add features not defined in specs
   - No extra convenience methods
   - No "nice to have" features
   - Stick to spec requirements

4. **Code style**:
   - File naming: lowercase + underscore (`scalar_ops.hpp`)
   - Class/struct names: lowercase + underscore (`bitarray`)
   - Function names: lowercase + underscore (`popcount()`)
   - Constants: SCREAMING_SNAKE_CASE (`BITCAL_HAS_AVX2`)

### Step 4: Test Against Spec (测试验证)

Write tests based on acceptance criteria in specs:

1. **Read `/specs/testing/`** for test requirements
2. **Write tests that verify**:
   - All acceptance criteria from product spec
   - All boundary conditions mentioned
   - All error cases specified
3. **Ensure coverage**:
   - Unit tests for all public APIs
   - Integration tests for cross-component behavior
   - Platform-specific tests where applicable

---

## Code Generation Rules

### API Changes

Any changes to externally exposed APIs **must**:

1. Update `/specs/api/bitcal-public-api.md` first
2. Add test cases to `/specs/testing/bitcal-testing-spec.md`
3. Update user documentation in `/docs/api/`
4. Add changelog entry in `CHANGELOG.md`

### Design Decisions

When encountering uncertain technical details:

1. Consult `/specs/rfc/` for architectural conventions
2. Do not invent design patterns on your own
3. If no RFC exists, propose one before implementing

### Performance Optimizations

For performance-related changes:

1. Document rationale in RFC format
2. Include benchmark methodology
3. Verify against performance targets in `/specs/product/`

---

## Project-Specific Context

### BitCal Library Overview

**BitCal** is a modern, high-performance C++17 header-only library for bit manipulation operations with automatic SIMD acceleration.

**Key Characteristics**:
- Header-only (no compilation required for use)
- C++17 minimum
- Zero external dependencies
- Compile-time SIMD dispatch
- Cross-platform: x86-64, ARM64, ARM32

**SIMD Backends**:
| Backend | Architecture | Width |
|---------|-------------|-------|
| `scalar` | Any | Portable fallback |
| `sse2` | x86-64 | 128-bit |
| `avx2` | x86-64 | 256-bit |
| `neon` | ARM | 128-bit |

**Predefined Types**:
| Type | Bits | Best Backend |
|------|------|--------------|
| `bit64` | 64 | scalar |
| `bit128` | 128 | sse2/neon |
| `bit256` | 256 | avx2 |
| `bit512` | 512 | avx2 |
| `bit1024` | 1024 | avx2 |

### File Organization

```
bitcal/
├── include/bitcal/       # Headers (header-only library)
│   ├── bitcal.hpp        # Main include
│   ├── config.hpp        # Platform detection
│   ├── simd_traits.hpp   # Type traits
│   ├── scalar_ops.hpp    # Scalar backend
│   ├── sse_ops.hpp       # SSE2 backend
│   ├── avx_ops.hpp       # AVX2 backend
│   └── neon_ops.hpp      # NEON backend
├── specs/                # Specifications
├── docs/                 # Documentation
├── tests/                # Unit tests
├── benchmarks/           # Performance benchmarks
└── examples/             # Usage examples
```

---

## Why This Matters

1. **Prevent AI Hallucination**: Forcing the first step to read `/specs` anchors the AI's thinking scope and prevents free-style implementation.

2. **Enforce Modification Path**: Declaring "modify specs before code" ensures document-code synchronization.

3. **Improve PR Quality**: When AI generates Pull Requests, the implementation will be highly consistent with business logic because it is developed according to the acceptance criteria defined in specs.

4. **Maintain Project Coherence**: All changes flow through a documented process, making the project maintainable over time.

---

## Quick Reference

### Before Coding Checklist

- [ ] Read relevant `/specs/product/` documents
- [ ] Read relevant `/specs/rfc/` documents
- [ ] Read `/specs/api/` for interface contracts
- [ ] Check for conflicts with existing specs
- [ ] Propose spec updates if needed
- [ ] Get user approval on spec changes

### Common Commands

```bash
# Build tests
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Run tests
./tests/test_bitcal

# Run benchmarks
./benchmarks/bench_bitcal

# Check formatting (if clang-format is configured)
clang-format --dry-run --Werror include/bitcal/*.hpp
```

---

*This document is the authoritative guide for AI agents working on this project. All code contributions must follow the Spec-Driven Development workflow defined above.*
