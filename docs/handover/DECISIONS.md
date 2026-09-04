# Decisions

## Settled

- Product name is Markup++; executable name is `markup`.
- Implementation language is dependency-free C++17 for straightforward Nift
  embedding and portable standalone builds.
- The public boundary is string-in/string-out with explicit format and options.
- Fragment HTML is the default; complete documents are opt-in.
- CLI output defaults to stdout. `-o` is explicit and atomic.
- The input file is never overwritten by `-o`; output symlinks are refused.
- Raw HTML is preserved by default because it is normal Markdown behavior.
- `--safe` escapes raw HTML and blocks active URL schemes, but is not marketed as
  a complete hostile-HTML sanitizer.
- Planned formats may be recognized before implementation only if conversion
  fails clearly and non-destructively.
- No CommonMark compliance claim is made at checkpoint 0.1.0.
- Standalone Markup++ will be canonical when Nift embedding starts.

## Deliberately deferred

- CommonMark version/extension profile.
- Reference links, nested and loose lists, footnotes and heading identifiers.
- Whether GFM tables/tasks/strikethrough remain defaults or become extensions.
- Source-position maps and structured diagnostics.
- Streaming conversion. The initial embedding contract uses whole strings, like
  Minify++.
- AsciiDoc and reStructuredText syntax coverage.
- Nift configuration, directives and conversion ordering.
- Packaging and release automation.

## Rejected for now

- Shelling out to Pandoc or language-specific converters: poor embedded boundary
  and external runtime dependency.
- Treating three syntaxes as one generalized grammar: their block models and
  extension ecosystems differ materially.
- Silently accepting `.adoc` or `.rst` while producing partial/Markdown output.
- Calling a small home-grown safe mode a sanitizer.
