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
4. fails on any case whose normalized output still differs from the reference
   unless that case is recorded with a reason in `differences.json`;
5. requires the reviewed `differences.json` manifest to be current.

Unexpected output differences fail the gate. Cases with reviewed, intentional
differences remain listed in `differences.json` with explicit reasons
(document-title promotion, docinfo field lists, footnote containers, MathML
math roles, and disabled raw/include directives that Docutils reports as
in-fragment system messages).

The pushed gate passed at commit `ee0ec00` in Actions run `33938717946`, with
Linux, macOS, Windows and libFuzzer jobs green. Public wording may now say
"compatible with the reStructuredText specification and Docutils 0.23 core
parser behavior under the documented profile." The gate itself compares
normalized output, not only inventory and determinism.

Before publishing the exact compatibility wording:

1. Run `make test-rst-release` in a clean checkout.
2. Run the full `make test` suite and sanitizer target.
3. Push the candidate and require Linux, macOS and Windows CI to pass.
4. Preserve the immutable commit and Actions run in this file.
5. Review every documented subset/difference; never infer Sphinx compatibility.
6. Publish the pinned Docutils version, 24-case oracle count, writer/settings,
   capability policy and unsupported extensions beside the claim.