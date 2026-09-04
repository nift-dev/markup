# Markup++ development handover

This is the entry point for developers and coding agents working on Markup++.
It is a living document: update it when formats, public APIs, tests, compatibility
claims, Nift synchronization or product boundaries change.

## Authority and identity

- Product: **Markup++**.
- Repository/extraction directory: `markup`.
- Executable: `markup`.
- Current development version: `0.1.0`.
- Public API version: `markup::api_version == 2`.
- Language/toolchain: self-contained C++17/C99 and Make; cmark source is
  vendored under BSD 2-Clause with no system-library dependency.
- Implemented format: Markdown.
- Planned formats already represented in the API: AsciiDoc and
  reStructuredText.

Current source and tests define behavior. README and ReleaseNotes describe the
public checkpoint. These handovers preserve rationale and future constraints.

## Product boundary

Markup++ converts markup text to HTML. It is not a site generator, template
engine, stylesheet, document editor, sanitizer, syntax highlighter or general
document publishing pipeline.

`--safe` is a conservative rendering option, not a complete HTML sanitizer for
hostile content. Applications accepting untrusted content must apply a mature
HTML sanitization policy appropriate to their threat model after conversion.

## Architecture and Nift relationship

- Public library API: `include/markup/Markup.h`.
- Conversion implementation: `src/Markup.cpp`.
- File/argument policy: `cli/main.cpp`.
- Tests: `tests/`.

The library accepts strings and returns strings/errors. It performs no file IO
and has no Nift dependency or global mutable state. Format selection is explicit
through `markup::Format`; `format_for_extension` is only a convenience boundary.

Nift does **not** embed Markup++ at this checkpoint. When embedding begins, the
standalone repository is canonical. Mirror a standalone-style subtree into Nift,
keep the public header and implementation byte-identical where practical, add an
explicit synchronization check, then connect Nift only through the public API.
Markup++ must not learn Nift's parser, project model, tracked state or CLI.

## Current behavior and evidence

The default Markdown profile uses the strict CommonMark engine. The explicitly
selected `Extended` profile preserves the initial converter's tables, tasks and
strikethrough behavior.

It is not yet CommonMark compliant. Important gaps are documented in
`docs/handover/COMMONMARK.md`; do not hide them behind broad “Markdown support”
claims. That document is the executable development plan for reaching and
maintaining CommonMark conformance. `docs/handover/ROADMAP.md` keeps only the
higher-level product sequence.

Run:

```sh
make test
make test-sanitize
```

The normal gate includes exact semantic checks, malformed/adversarial inputs,
filesystem-aware CLI behavior and 10,000 deterministic mutations.

## Development standard

For each syntax family:

```text
define the compatibility target
-> collect valid examples and ambiguous neighbors
-> add exact and structural oracles
-> implement without weakening existing behavior
-> run focused, adversarial, mutation and sanitizer gates
-> measure against the declared external conformance corpus
-> update README, handover, roadmap and site truth
-> synchronize Nift only after standalone approval
```

Golden output is useful but not sufficient. Where output may vary legitimately,
parse or inspect its structure. Future CommonMark work should use the official
spec examples as a versioned oracle rather than inventing local interpretations.

## Checkpoints and public actions

A validated checkpoint is not automatically a commit, tag, release, Nift update,
website deployment or push. Keep those actions explicit. Never describe a
development checkpoint as production-ready or standards-complete without the
corresponding evidence.

## Deeper handovers

- `docs/handover/ARCHITECTURE.md`
- `docs/handover/DECISIONS.md`
- `docs/handover/DEVELOPMENT.md`
- `docs/handover/TESTING.md`
- `docs/handover/COMMONMARK.md`
- `docs/handover/ROADMAP.md`
- `docs/handover/PROJECT-HISTORY.md`
