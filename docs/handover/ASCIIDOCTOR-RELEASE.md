# Asciidoctor 2.0.26 core compatibility evidence

The declared profile is Asciidoctor 2.0.26, embedded HTML5, article doctype,
secure mode, reproducible environment attributes and no extension API.

- Gem SHA-256: `16e3accf1fc206bbd6335848649d7fd65f31d2daa60d85af13d47a8ee4b071c1`
- Focused processor corpus: 9 cases
- Public-API focused compatibility checks: 23
- Difference manifest: exact-case, reviewed
- Unsupported inventory: extension APIs, external converters, third-party
  macros and application layers

`make test-asciidoctor-release` is the offline gate. A machine with Ruby and
the pinned gem can additionally regenerate raw reference evidence with
`tests/asciidoctor_reference.py` and run `tests/asciidoctor_compare.py`.

Do not publish the compatibility wording until the pushed Linux, macOS and
Windows jobs pass at the candidate commit. Never shorten the claim to
"Asciidoctor compatible" or "drop-in compatible".
