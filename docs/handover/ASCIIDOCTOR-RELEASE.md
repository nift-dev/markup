# Asciidoctor 2.0.26 core compatibility evidence

The declared profile is Asciidoctor 2.0.26, embedded HTML5, article doctype,
secure mode, reproducible environment attributes and no extension API.

- Gem SHA-256: `16e3accf1fc206bbd6335848649d7fd65f31d2daa60d85af13d47a8ee4b071c1`
- Focused processor corpus: 9 cases
- Public-API focused compatibility checks: 23
- Difference manifest: exact-case, reviewed, with reasons
- Unsupported inventory: extension APIs, external converters, third-party
  macros and application layers

`make test-asciidoctor-release` runs the offline manifest gate and then
`tests/asciidoctor_compare.py`, which compares the normalized Markup++
fragment with the pinned Asciidoctor 2.0.26 `reference.json` output. The
normalization removes documented writer decoration (wrapper divs, `class`/
`style`/`role` attributes, auto-generated `id="_..."` attributes,
`<thead>`/`<tbody>`/`<colgroup>`/`<col>` structure, `<p>` inside list items and
table cells, and the `<code>` wrapper Asciidoctor adds inside `<pre>`).
Every reviewed difference in `differences.json` is pinned by the SHA-256 of
both normalized sides, so a recorded case cannot change arbitrarily: the
expected reference hash and the actual Markup++ hash must both match the
reviewed values and the two sides must remain different. Any unlisted
mismatch fails the gate, and any manifest entry for a case that now matches
fails it too.

`make test-difference-gates` runs `tests/difference_gate_self_test.py`, which
proves that mutating a pinned hash, removing a pin, listing a now-matching
case, duplicating an id or adding an unknown id all fail the gates.

The pinned `reference.json` was generated with `tests/asciidoctor_reference.py`
against the pinned gem; the gate fails loudly if it is absent, so the
compatibility claim can never rest on inventory alone.

Historical evidence: commit `ee0ec00` in Actions run `33938717946` passed the
earlier implementation's Linux, macOS, Windows and libFuzzer suite. The
strengthened normalized-output gate and the pinned reference were added after
that commit; the gate currently passes locally on Linux at the new candidate.
Cross-platform confirmation of the strengthened gate is **pending** until the
new committed candidate passes Linux, macOS and Windows CI. After that, record
the new immutable commit and Actions run here in a separate evidence-only
commit, then publish the compatibility wording.

Public wording may then say "compatible with Asciidoctor core 2.0.26 under the
documented profile." Never shorten this to "Asciidoctor compatible" or
"drop-in compatible."