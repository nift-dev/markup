# Markup++ release notes

## Development checkpoint 0.1.0

- Added checksum-verifying curl install, download, update and uninstall scripts,
  cross-platform release archives, release rehearsals and public installer smoke
  tests.
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
- Implemented AsciiDoc checkpoints AD1-AD11: a neutral source-positioned model,
  normalization and bounds, headers/attributes/sections, structural and
  verbatim blocks, list families and ordered inline substitutions.
- Added references/media, bounded tables, pure macros/conditionals, explicit
  host-resolved includes, safe passthrough/URI behavior and a pinned alpha gate.
- Added 39 focused AsciiDoc checks and deterministic coverage of all 13 current
  upstream TCK inputs while retaining the CommonMark release gate.
- Completed Asciidoctor compatibility checkpoints AC0-AC9 against a pinned
  2.0.26 profile, with the public claim awaiting cross-platform evidence.
- Completed reStructuredText checkpoints RST0-RST14 against a frozen Docutils
  0.23 oracle: neutral syntax model, roles/directives, tables, references,
  substitutions, host-controlled resources, diagnostics and release gates.
- Added 39 focused RST checks, a large/adversarial robustness suite, all-format
  mutation coverage and a 24-case Docutils profile gate that now compares
  normalized output against the pinned fragments and fails on unexpected
  differences.
- Fixed four reStructuredText parser defects surfaced by the frozen oracle:
  inline literals are no longer swallowed into a following hyperlink label,
  grid-table separators accept `=` runs, simple tables use border column
  boundaries (single- or double-space), and `contents` emits a real table of
  contents with section anchors and backreferences.
- Rendered RST comments as HTML comments, title-cased admonition labels, and
  aligned topic/sidebar output with the Docutils HTML5 writer.
- Generated and pinned a genuine Asciidoctor 2.0.26 `reference.json`; the AC9
  gate now compares normalized Markup++ output against it. Fixed `. ` (single
  dot + space) ordered lists, which the block-title grammar previously
  consumed.
- Clamped AsciiDoc include `indent=` and enforced the 64 MiB expansion budget
  on the selected output; added hostile AsciiDoc include cycle/depth/size and
  table-cell tests; duplicate-anchor diagnostics now use the configured source
  identity.
- Passed the earlier combined AsciiDoc/Asciidoctor and reStructuredText/Docutils
  release gate at commit `ee0ec00` in Actions run `33938717946` (historical
  evidence for that earlier implementation). The strengthened normalized-output
  gates, pinned references, recorded-difference hashes and their adversarial
  self-test passed Linux, macOS and Windows at commit `20cbefe` in Actions run
  `33964331360`.
- Added an exact 41-file synchronization check for Nift's embedded `markuppp/`
  subtree. Standalone Markup++ remains the source of truth.

This is an implementation checkpoint, not a tagged public release. The evidence
now supports the versioned CommonMark 0.31.2 compliance claim and the bounded
Asciidoctor 2.0.26 and Docutils 0.23 core compatibility claims.
