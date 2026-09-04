# Testing

## Current gates

`make test` runs:

- `markdown_smoke.cpp`: exact feature and public-API output contracts;
- `markdown_adversarial.cpp`: malformed delimiters, large runs, embedded NUL,
  safe-mode active content and recovery behavior;
- `cli_smoke.sh`: stdout/file/stdin paths, diagnostics, permission preservation,
  atomic replacement boundaries, symlink refusal and unsupported formats;
- `fuzz_smoke.cpp`: 10,000 deterministic mixed-character mutations across raw
  and safe modes, with periodic standalone documents.

`make test-sanitize` rebuilds semantic and adversarial gates under ASan/UBSan.
Sanitizer success is workload evidence, not a universal memory-safety proof.
Some sandboxed runners prevent LeakSanitizer from inspecting `/proc`; use
`make test-sanitize ASAN_OPTIONS=detect_leaks=0:halt_on_error=1` there and record
that leak detection was unavailable rather than describing it as LSan evidence.

## CommonMark evidence

`make test-commonmark` runs all 652 examples from the pinned CommonMark 0.31.2
corpus through the public conversion API and strict extensions-off profile.
The original CM0 baseline was 198/652; the vendored strict engine reaches
652/652. Section-level checkpoint tests and broader release evidence remain in
progress through CM10.

## Missing evidence

- Differential comparison against established converters with a precisely
  declared extension profile.
- Cross-platform CI on Linux, macOS and Windows.
- Long-lived RSS and independent Valgrind checkpoints.
- Large-document performance and complexity guards.
- Fuzzing with a coverage-guided engine.
- Dedicated UTF-8 validity policy and corpus.
- AsciiDoc and reStructuredText corpora.
- Nift synchronization and integration tests.

These are roadmap gates, not reasons to overstate the initial test counts.
