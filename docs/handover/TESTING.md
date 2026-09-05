# Testing

## Current gates

`make test` runs the ordinary offline suite, including the AD11 pinned-profile gate:

- `markdown_smoke.cpp`: exact feature and public-API output contracts;
- `markdown_adversarial.cpp`: malformed delimiters, large runs, embedded NUL,
  safe-mode active content and recovery behavior;
- `cli_smoke.sh`: stdout/file/stdin paths, diagnostics, permission preservation,
  atomic replacement boundaries, symlink refusal and unsupported formats;
- `fuzz_smoke.cpp`: 10,000 deterministic mixed-character mutations through both
  implemented formats across raw/safe and fragment/standalone modes;
- `asciidoc.cpp`: 39 focused AD1-AD10 model, capability, reference, safety and rendering checks;
- `asciidoc_release_gate.py`: three-run deterministic AD11 gate over 13/13 pinned alpha TCK inputs;
- `asciidoc_fixture_inventory.py`: validates all 13 inputs in the pinned
  Eclipse TCK alpha snapshot offline.
- `restructuredtext.cpp`: 39 focused RST0-RST12 behavior and capability checks;
- `rst_robustness.cpp`: large mixed, delimiter and malformed-input coverage;
- `docutils_release_gate.py`: validates the frozen 24-case Docutils 0.23 oracle
  inventory and three-run CLI determinism.

`make test-sanitize` rebuilds Markdown semantic/adversarial, AsciiDoc and
reStructuredText semantic/robustness gates under ASan/UBSan.
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
execution evidence. The first successful pushed gate was Actions run
`33933214625` at `3cba857cfd9ba90b9013c697be08d53cc5ad0e82`: GCC/Linux,
Clang/macOS, MSVC/Windows and libFuzzer all passed. The complete immutable ledger
lives in `COMMONMARK-RELEASE.md`.

The CMake/CTest conformance gate generates a C++ fixture from the pinned JSON
corpus and executes all 652 conversions in one process linked to `markup_core`.
This avoids the severe process-startup and antivirus cost of launching the CLI
652 times on hosted Windows. The Python subprocess runner remains authoritative
for report/reproducibility and CLI-boundary checks on Unix, while `cli_smoke.sh`
continues to test the standalone executable contract directly.

## Missing evidence

- Differential comparison against established converters with a precisely
  declared extension profile.
- Long-lived RSS and independent Valgrind checkpoints.
- Large-document performance and complexity guards.
- A broader Unicode/invalid-UTF-8 corpus beyond the strict-mode replacement
  regressions.
- Complete AsciiDoc expected-ASG adapter evidence when upstream publishes it.
- Record pushed cross-platform AC9 and RST14 candidate evidence.
- Nift synchronization and integration tests.

These are roadmap gates, not reasons to overstate the initial test counts.
