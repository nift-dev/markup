# Markup++ development handover

This is the entry point for developers and coding agents working on Markup++.
It is a living document: update it when formats, public APIs, tests, compatibility
claims, Nift synchronization or product boundaries change.

## Authority and identity

- Product: **Markup++**.
- Repository/extraction directory: `markup`.
- Executable: `markup`.
- Current development version: `0.1.0`.
- Public API version: `markup::api_version == 4`.
- Language/toolchain: self-contained C++17/C99 and Make; cmark source is
  vendored under BSD 2-Clause with no system-library dependency.
- Implemented formats: Markdown, the AD0-AD11/AC0-AC9 AsciiDoc profiles and the
  RST0-RST14 reStructuredText/Docutils 0.23 profile.

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

Nift embeds the approved Markup++ sources from commit `ee0ec00` under
`markuppp/`. The standalone repository remains canonical. Run
`make check-nift-sync NIFT_DIR=/path/to/nift` to compare the public header,
implementation, licence and vendored cmark tree byte for byte. Nift connects
only through the public API; Markup++ must not learn Nift's parser, project
model, tracked state or CLI.

## Current behavior and evidence

The default Markdown profile is CommonMark 0.31.2 compliant. All 652 pinned
examples and the pushed Linux/macOS/Windows release gate pass. The explicitly selected `Extended`
profile preserves the initial converter's tables, tasks and strikethrough
behavior.

The immutable release evidence is recorded in
`docs/handover/COMMONMARK-RELEASE.md`. Do not broaden the versioned claim into
“all Markdown,” and do not conflate the optional extension profile with core
CommonMark. `docs/handover/COMMONMARK.md` remains the maintenance plan.

AsciiDoc is implemented through AD11 against immutable but unreleased Eclipse
specification/TCK snapshots. Call it the completed pinned development profile,
not stable language conformance: the upstream alpha TCK's expected ASG oracle
and a future stable-spec upgrade gate remain. `docs/handover/ASCIIDOC.md`
is authoritative for its completed behavior and limitations.

AsciiDoc AC0-AC9 and reStructuredText RST0-RST14 have now passed their pushed
Linux/macOS/Windows candidate gate at commit `ee0ec00` in Actions run
`33938717946`. Their exact, bounded compatibility wording may be published only
with the pinned profiles and limitations recorded in the release handovers.

Run:

```sh
make test
make test-release-local
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
- `docs/handover/COMMONMARK-RELEASE.md`
- `docs/handover/ASCIIDOC.md`
- `docs/handover/ASCIIDOCTOR.md`
- `docs/handover/RESTRUCTUREDTEXT.md`
- `docs/handover/RESTRUCTUREDTEXT-RELEASE.md`
- `docs/handover/ROADMAP.md`
- `docs/handover/PROJECT-HISTORY.md`
