# Markup++ release notes

## Development checkpoint 0.1.0

- Established the standalone C++17 library and `markup` CLI.
- Added a format-neutral API prepared for Markdown, AsciiDoc and
  reStructuredText converters.
- Implemented the initial Markdown-to-HTML converter and optional standalone
  document wrapper.
- Added raw-HTML-preserving and safe conversion modes.
- Added atomic CLI output with input-overwrite and symlink refusal.
- Added semantic, adversarial, CLI, deterministic mutation and sanitizer gates.
- Pinned the CommonMark 0.31.2 corpus and recorded the original 198/652
  baseline.
- Added a source-vendored cmark 0.31.1 strict engine with no system-library
  dependency.
- Reached 652/652 official examples through the public Markup++ boundary.
- Made CommonMark the default Markdown profile in API version 2; tables, task
  lists and strikethrough now require the explicit `Extended` profile or
  `--extensions`.
- Added section gates, 37 focused regressions, pathological-input guards,
  reproducibility/differential checks, performance/RSS limits, a libFuzzer
  harness, CMake/MSVC support and cross-platform CI configuration.

This is an implementation checkpoint, not a tagged public release and not a
final public compliance claim. That wording remains gated on a successful run
of the newly added Linux, macOS and Windows CI from the pushed commit.
