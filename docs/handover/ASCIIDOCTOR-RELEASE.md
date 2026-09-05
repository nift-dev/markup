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
Unexpected output differences fail the gate; only the reviewed `reference`
case (cross-reference label resolution) is recorded in `differences.json`.

The pinned `reference.json` is regenerated with `tests/asciidoctor_reference.py`
against the pinned gem; the gate fails loudly if it is absent, so the
compatibility claim can never rest on inventory alone.

The pushed gate passed at commit `ee0ec00` in Actions run `33938717946`, with
Linux, macOS, Windows and libFuzzer jobs green. Public wording may now say
"compatible with Asciidoctor core 2.0.26 under the documented profile." Never
shorten this to "Asciidoctor compatible" or "drop-in compatible."