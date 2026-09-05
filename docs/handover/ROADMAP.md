# Roadmap

## M0 - Markdown foundation (current)

- Format-neutral C++ API and standalone CLI.
- Useful Markdown block/inline subset.
- Fragment/standalone and raw/safe modes.
- Semantic, adversarial, CLI and deterministic mutation tests.
- Honest compatibility boundary and living handovers.

## M1 - Markdown compatibility

- Complete checkpoints CM0 through CM10 in `COMMONMARK.md`.
- Pass all 652 examples from the pinned CommonMark 0.31.2 corpus in the strict
  core profile.
- Keep tables, task lists and strikethrough outside the core conformance claim.
- Add pathological-input complexity guards, coverage-guided fuzzing and
  cross-platform CI before publishing the compliance claim.

## M2 - AsciiDoc language profile (AD0-AD11 complete)

- Maintain the completed versioned language/TCK checkpoints AD0 through AD11 in
  `ASCIIDOC.md`.
- Independently complete Asciidoctor core compatibility checkpoints AC0 through
  AC9 in `ASCIIDOCTOR.md`; do not conflate processor compatibility with language
  conformance.
- Keep extensions and resource access outside either claim unless explicitly
  implemented and capability-gated.

## M3 - reStructuredText

- Complete RST0 through RST14 in `RESTRUCTUREDTEXT.md` against the specification
  and pinned Docutils 0.23 core parser behavior.
- Compare normalized doctrees independently from stable Markup++ fragment HTML.
- Treat directives, roles, includes and raw content as explicit registries and
  capability/security boundaries.

## M4 - Nift embedding

- Mirror the approved standalone subtree into Nift.
- Add standalone-to-Nift sync checks.
- Decide user-facing configuration/directive behavior and conversion order.
- Add dependency, incremental rebuild, error propagation and cross-platform
  integration tests.
- Keep the standalone `markup` CLI independently buildable and useful.

## Later

- Packaging, signed releases and install scripts.
- Source maps/positions and richer diagnostics if real integrations need them.
- Streaming only if profiling demonstrates a meaningful need.
