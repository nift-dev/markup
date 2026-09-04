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

`make test-commonmark-cm9` additionally requires three byte-identical 652/652
reports, an empty non-stale expected-failure manifest, and 1,652 deterministic
comparisons between the Markup++ public boundary and the pinned cmark boundary
(the official corpus plus 1,000 generated inputs). This differential gate
detects wrapper/profile drift; it is not independent-parser evidence.

`make test-release-local` runs the semantic/adversarial/CLI/mutation/profile
suites, three reproducible corpus passes, differential cases, large-document
time/RSS budgets and all seven pathological emphasis/link inputs. Run
`make test-sanitize` separately because sanitizer availability is toolchain
specific.

The GitHub Actions workflow adds GCC/Linux, Clang/macOS, MSVC/Windows, Linux
leak-enabled sanitizer and bounded Clang libFuzzer jobs. A workflow file is not
execution evidence: record the first successful pushed run here before changing
the CM10 ledger to complete or publishing the compliance claim.

## Missing evidence

- Differential comparison against established converters with a precisely
  declared extension profile.
- First successful pushed cross-platform CI run and its immutable commit/run ID.
- Long-lived RSS and independent Valgrind checkpoints.
- Large-document performance and complexity guards.
- Successful CI execution evidence for the new coverage-guided libFuzzer gate.
- A broader Unicode/invalid-UTF-8 corpus beyond the strict-mode replacement
  regressions.
- AsciiDoc and reStructuredText corpora.
- Nift synchronization and integration tests.

These are roadmap gates, not reasons to overstate the initial test counts.
