# CommonMark release evidence

## Local evidence

Run from a clean checkout:

```sh
make clean
make test-release-local
make test-sanitize
```

Expected evidence at this checkpoint:

- 48 Markdown smoke checks;
- 75 adversarial checks;
- CLI smoke suite;
- 10,000 deterministic mutation cases;
- 37 focused CommonMark regressions;
- 12 profile-matrix conversions;
- three identical 652/652 corpus runs;
- 1,652 pinned-reference differential cases;
- seven large pathological delimiter/bracket/reference cases;
- three large-document time/RSS guards;
- ASan/UBSan over smoke, adversarial, CommonMark and profile tests.
- GNU CMake configure/build plus all six CTest targets.

Local result (2026-09-05): all items above passed with GCC/G++ 13.3.0. The
large-document guard completed in 0.76 seconds with 67.9 MiB peak child RSS.
LeakSanitizer was unavailable because this sandbox blocks its process
inspection; ASan and UBSan passed with leak detection disabled. This result does
not substitute for the leak-enabled Linux CI job.

## Pushed CI evidence

Before publishing “CommonMark 0.31.2 compliant”:

1. Push the exact candidate commit without modifying the vendored corpus.
2. Require the `CI` workflow to pass its GCC/Linux, Clang/macOS, MSVC/Windows
   and libFuzzer jobs.
3. Record the candidate commit, Actions run URL/ID and every job result below.
4. Confirm the expected-failure manifest remains exactly `[]`.
5. Re-run `make test-release-local` from the pushed clean checkout.
6. Change CM10 to complete and update README, release notes and website wording
   in one evidence-only follow-up commit.

Evidence ledger:

- Candidate commit: `3cba857cfd9ba90b9013c697be08d53cc5ad0e82`
- GitHub Actions run: `33933214625` (2026-09-05, success in 1m 9s)
- GCC/Linux: passed
- Clang/macOS: passed
- MSVC/Windows: passed in 1m 4s with the in-process 652-case harness
- Linux sanitizer/leak detection: passed in the Unix/Linux job
- Clang libFuzzer: passed in 26s

The first pushed release gate is complete. Keep the immutable commit and run ID
with future claims; a later passing run does not erase this baseline.
