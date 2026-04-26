# Project Spec: Governance and Repository Truth

## Overview

This spec defines BitCal's final-state governance model, authoritative directories, and archive-readiness rules.

## Requirements

### Requirement: OpenSpec SHALL be the single source of truth
BitCal SHALL treat `openspec/` as the only authoritative specification and change-management location for all future product, API, RFC, testing, and project-governance requirements.

#### Scenario: Contributor adds a new requirement
- **WHEN** a contributor proposes a new feature, policy, or breaking change
- **THEN** the requirement MUST be created or updated under `openspec/` rather than under a parallel legacy spec tree

### Requirement: The repository SHALL define an archive-ready governance model
BitCal SHALL maintain explicit governance documents that define directory truth, review discipline, change-control rules, and archive-readiness criteria for the final stabilized state of the project.

#### Scenario: Maintainer evaluates project readiness
- **WHEN** a maintainer reviews whether the repository is ready for final stabilization or handoff
- **THEN** the repository MUST provide written criteria covering documentation, workflows, versioning, review flow, and archival status
