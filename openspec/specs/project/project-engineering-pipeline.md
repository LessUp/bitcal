# Project Spec: Engineering Pipeline

## Overview

This spec defines BitCal's retained CI/CD, release, versioning, and automation boundaries for the header-only library.

## Requirements

### Requirement: CI/CD SHALL be minimal and justified
BitCal SHALL keep only workflows that provide distinct release, verification, or publishing value, and each remaining workflow MUST have an explainable trigger and scope.

#### Scenario: A workflow is retained
- **WHEN** a GitHub Actions workflow remains in the repository
- **THEN** it MUST have a unique responsibility and MUST NOT duplicate another workflow's validation role

### Requirement: Workflow execution SHALL avoid unnecessary triggers
BitCal SHALL scope workflow triggers by branch, event, and path filters so that unrelated documentation or metadata edits do not invoke full verification matrices without justification.

#### Scenario: Non-code documentation changes are pushed
- **WHEN** a change only affects documentation or repository metadata outside a workflow's scope
- **THEN** heavyweight validation workflows MUST NOT run unless the documented policy explicitly requires them

### Requirement: Release and versioning SHALL align with a header-only library
BitCal SHALL maintain a single version source and a release strategy consistent with its header-only distribution model.

#### Scenario: A new version is cut
- **WHEN** maintainers create a release tag
- **THEN** release artifacts, changelog references, and version strings MUST all resolve to the same canonical version value and packaging policy
