# KatanaEngine Project – Claude Rules

## Project purpose
KatanaEngine is a reusable C++ game engine library.

It must:
- Remain independent from all other projects.
- Expose a clean, stable public API
- Favor correctness, clarity, and performance over shortcuts

TestSandbox exists to validate KatanaEngine features. Engine code should never assume the sandbox exists.

## Hard boundaries
- Do NOT include headers from any other project.
- Do NOT reference assets or paths from other projects.
- Do NOT add project references from KatanaEngine to other projects.
- Do NOT implement “temporary demo logic” in KatanaEngine.

## Project structure expectations
(Adjust if different, but follow the spirit.)

- /KatanaEngine/include/   → public engine headers
- /KatanaEngine/src/       → implementation
- /KatanaEngine/internal/  → optional private headers
- /KatanaEngine/tests/     → optional unit tests

Public headers must not include heavy third-party headers unless unavoidable.

## API design rules
- Public APIs should be minimal and explicit.
- Prefer simple types and clear ownership semantics.
- Avoid exposing STL containers in public APIs unless it is intentional and stable.
- Avoid template-heavy public interfaces unless they are core to the feature.

## Ownership & memory
- Prefer stack allocation where practical.
- Use `std::unique_ptr` for ownership transfer.
- Use `std::shared_ptr` only for true shared lifetime (document why).
- No raw owning pointers.
- Clearly document lifetime expectations for references and pointers.

## Performance rules
- Avoid allocations in per-frame paths (Update, Render, input polling, etc.).
- Avoid virtual calls in tight inner loops unless justified.
- Avoid exceptions in hot paths.
- Prefer data-oriented layouts when reasonable.

## Timing & game loop assumptions
- Engine timing is based on Allegro (`al_get_time`) and `GameTime`.
- Do not embed a concrete game loop in the engine unless explicitly requested.
- Engine systems should be loop-agnostic when possible.

## Coding style
- Match existing naming conventions.

Keep headers clean:
- Forward declare where possible
- Include only what is required

## Adding new subsystems
When adding a new engine feature:

1. Define the public API first (header).
2. Implement internally.
3. Add a minimal usage example in TestSandbox.
4. Avoid overengineering the first version.

## What to avoid
- Large refactors without request
- “Framework-ifying” everything
- Adding editor tooling unless explicitly asked
- Adding scripting layers unless requested

## How to present changes
When modifying engine code:

- List file paths first
- Keep patches small
- Explain API changes clearly
- Call out any breaking change explicitly

## Safety rail
Do not modify this file unless explicitly instructed.
