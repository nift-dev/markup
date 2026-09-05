# reStructuredText compatibility release gate

The release profile is the reStructuredText specification and core parser
behavior of Docutils 0.23 using the standalone reader, reStructuredText parser
and HTML5 writer. Raw and file insertion are disabled in the oracle.

`make test-rst-release` runs `tests/docutils_release_gate.py`, which:

1. validates the pinned Docutils 0.23 profile and the exact 24-case corpus
   inventory;
2. runs each case three times through the CLI and requires deterministic output;
3. normalizes the Markup++ fragment and the pinned `reference.json` fragment by
   removing documented writer decoration (attributes, wrapper tags, `<p>`
   inside list items/table cells, and heading-level offsets caused by Docutils
   document-title promotion) and compares the structures;
4. pins every reviewed difference in `differences.json` by the SHA-256 of both
   normalized sides, so a recorded case cannot change arbitrarily: the expected
   reference hash and the actual Markup++ hash must both match the reviewed
   values and the two sides must remain different;
5. fails on any unlisted mismatch and on any manifest entry for a case that now
   matches.

`make test-difference-gates` runs `tests/difference_gate_self_test.py`, which
proves that mutating a pinned hash, removing a pin, listing a now-matching
case, duplicating an id or adding an unknown id all fail the gates.

Historical evidence: commit `ee0ec00` in Actions run `33938717946` passed the
earlier implementation's Linux, macOS, Windows and libFuzzer suite. The
strengthened normalized-output gate above was added after that commit.

Verified cross-platform evidence for the strengthened gate: commit `20cbefe`
in Actions run `33964331360` passed the strengthened gate, the pinned-output
comparison and the difference-gate adversarial self-test on Linux
(GCC/ubuntu), macOS (Clang) and Windows (MSVC), plus the bounded libFuzzer
job. The exact compatibility wording may be published with this evidence.

Before publishing the exact compatibility wording:

1. Run `make test-rst-release` in a clean checkout.
2. Run the full `make test` suite and sanitizer target.
3. Push the candidate and require Linux, macOS and Windows CI to pass the
   strengthened gate.
4. Preserve the immutable commit and Actions run in this file.
5. Review every documented subset/difference; never infer Sphinx compatibility.
6. Publish the pinned Docutils version, 24-case oracle count, writer/settings,
   capability policy and unsupported extensions beside the claim.