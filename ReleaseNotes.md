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

- Passed the first pushed cross-platform release gate at commit `3cba857` in
  Actions run `33933214625`, including GCC/Linux, Clang/macOS, MSVC/Windows and
  bounded libFuzzer coverage.
- Froze immutable Eclipse AsciiDoc language and TCK development snapshots and
  inventoried all 13 current upstream inputs offline.
- Implemented AsciiDoc checkpoints AD1-AD5: a neutral source-positioned model,
  normalization and bounds, headers/attributes/sections, structural and
  verbatim blocks, list families and ordered inline substitutions.
- Added 26 focused AsciiDoc checks while retaining the CommonMark release gate.

This is an implementation checkpoint, not a tagged public release. The evidence
now supports the versioned CommonMark 0.31.2 compliance claim.
