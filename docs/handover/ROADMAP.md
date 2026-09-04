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

## M2 - AsciiDoc

- Define a deliberately bounded initial compatibility profile rather than
  claiming the entire Asciidoctor ecosystem.
- Add headings, paragraphs, emphasis, lists, links, code/literal blocks,
  admonitions and tables with explicit unsupported diagnostics.
- Establish an oracle/corpus and document attributes/includes/macros policy.

## M3 - reStructuredText

- Define the docutils-compatible target and security policy.
- Add sections, paragraphs, inline roles, lists, links, literal blocks,
  directives and tables in evidence-backed increments.
- Treat directives and includes as capability/security boundaries, not merely
  syntax.

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
