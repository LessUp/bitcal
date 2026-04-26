## ADDED Requirements

### Requirement: AI control documents SHALL be project-specific
BitCal SHALL maintain project-level AI instruction files that encode repository-specific architecture, OpenSpec workflow rules, review discipline, documentation policy, and tooling boundaries rather than generic boilerplate guidance.

#### Scenario: An AI worker starts work in the repository
- **WHEN** an AI-assisted tool reads project instructions
- **THEN** it MUST find BitCal-specific constraints for code changes, documentation handling, OpenSpec usage, and review expectations

### Requirement: AI tool selection SHALL minimize redundant context
BitCal SHALL document which capabilities belong in MCP integrations, which belong in lightweight CLI skills, and which tools are intentionally excluded to avoid token waste or duplicated responsibility.

#### Scenario: A new AI integration is proposed
- **WHEN** maintainers evaluate adding a plugin, skill, or MCP integration
- **THEN** the decision MUST be recorded against the existing tool-selection policy and reject redundant or low-value integrations
