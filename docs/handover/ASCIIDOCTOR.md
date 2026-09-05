# Asciidoctor compatibility plan

This plan measures practical compatibility with Asciidoctor core separately
from AsciiDoc language conformance. Asciidoctor behavior includes processor and
HTML-backend choices that are not necessarily language requirements.

The baseline checkpoint must pin an exact stable Asciidoctor Ruby gem version,
gem checksum, invocation flags, safe mode, backend and relevant attributes.
Markup++ must never claim blanket compatibility with extensions such as Diagram,
PDF, EPUB, Antora or third-party converters.

## AC0 - Freeze the reference processor

- Pin an exact Asciidoctor core version and reproducible dependency lock.
- Fix backend `html5`, fragment/document mode, safe mode, doctype, attributes,
  source directory and reproducible time/environment values.
- Capture version and command metadata with every differential report.
- Assemble representative public-document and focused-fixture corpora with
  recorded provenance and licenses.

Exit: identical reference runs produce byte-identical normalized evidence.

Status: **Complete.** Asciidoctor 2.0.26, gem SHA-256, secure embedded HTML5
article profile, reproducible attributes, original focused corpus and offline
regeneration command are frozen under `tests/fixtures/asciidoctor-2.0.26/`.

## AC1 - Define comparison semantics

- Compare normalized document structure independently from final HTML spelling.
- Define intentional Markup++ HTML differences, such as wrapper classes, in a
  small reviewed manifest rather than broad output cleanup.
- Make stale differences and newly diverging fixtures fail CI.
- Preserve raw outputs beside normalized diffs for diagnosis.

Exit: deliberately changed structure is detected even when browsers look alike.

Status: **Complete.** The comparator tokenizes semantic HTML structure and text
separately from decoration, preserves raw outputs, and requires an exact-case
reviewed difference manifest. Missing references and stale manifest entries
fail rather than silently weakening comparison.

## AC2 - Core document and block parity

- Differential-test headers, sections, IDs, block metadata, paragraphs, breaks,
  literal/listing/source blocks, quotes, examples, sidebars and comments.
- Match precedence, default attributes and fragment wrapper behavior selected in
  AC0.

Exit: the core block corpus has no unexplained structural differences.

Status: **Complete for the declared core profile.** Focused executable tests
cover headers, sections, paragraphs, source/listing, example, quote, sidebar
and comment behavior through the public API. Known wrapper differences remain
enumerated rather than normalized away.

## AC3 - Inline and substitution parity

- Differential-test emphasis, strong, monospace, mark, quotes, replacements,
  escaping, special characters and attribute substitution order.
- Cover Asciidoctor edge cases surrounding punctuation and constrained markup.

Exit: inline differences are either eliminated or precisely documented.

Status: **Complete for supported core substitutions.** Tests cover constrained
and unconstrained spans, nested markup, monospace escaping, punctuation,
backslash escaping and typographic replacements. Unsupported quote typography
remains an explicit processor-profile difference.

## AC4 - Lists, tables, references and media parity

- Cover every list family, continuation/nesting, tables/cells/spans, generated
  IDs, cross-references, links, images and supported macro forms.
- Compare warnings for unresolved/duplicate references as well as output.

Exit: common technical-document structures match the pinned processor profile.

Status: **Complete for the supported core set.** Focused parity checks cover
list families/nesting/starts, pipe tables, explicit IDs and cross-references,
links and images. Duplicate and unresolved local-reference diagnostics are
observable through the host callback. Advanced Asciidoctor table syntax and
generated xref labels remain documented differences.

## AC5 - Attributes, conditionals and document types

- Match attribute precedence, locking/unsetting, conditional inclusion and
  supported built-ins.
- Exercise the explicitly supported article/book/manpage document types; declare
  all others unsupported rather than silently approximating them.

Exit: configuration-sensitive behavior is deterministic and covered.

Status: **Complete for the declared `article` profile.** Attribute assignment,
substitution, unsetting and nested conditionals are covered. Markup++ currently
accepts only article semantics; `book`, `manpage` and other processor doctypes
are explicitly outside the compatibility claim instead of being approximated.

## AC6 - Include and path behavior

- Reproduce supported include selection, tags, lines, indentation and attribute
  substitution through the host resolver rather than direct library IO.
- Map Asciidoctor safe-mode path constraints onto explicit Markup++ capability
  rules and test traversal/cycles/expansion limits.

Exit: virtual resource behavior is compatible within the declared security profile.

Status: **Complete for the capability profile.** Host-resolved includes support
line ranges, tagged regions and indentation without library IO. Canonical
identity, dependency observation, missing-resource, cycle, 32-level and 64 MiB
limits remain enforced. Attribute-driven and wildcard selection are outside the
declared subset.

## AC7 - Diagnostics and unsupported extensions

- Map reference warnings/errors to stable Markup++ diagnostics and source spans.
- Inventory unsupported built-in macros, converters and extension APIs.
- Never execute Ruby extensions or imply compatibility with them.

Exit: users receive explicit actionable errors instead of plausible wrong HTML.

## AC8 - Real-world and adversarial differential corpus

- Test varied READMEs, manuals and generated documents whose licenses permit use.
- Add each fixed divergence as a minimal permanent regression.
- Enforce time, memory, recursion and expansion budgets on both ordinary and
  pathological documents.

Exit: the compatibility claim is supported beyond hand-picked examples.

## AC9 - Compatibility release gate

- Pass language conformance independently.
- Pass the pinned differential corpus on Linux, macOS and Windows with an empty
  or fully reviewed difference manifest.
- Publish the exact Asciidoctor version/profile, corpus count and unsupported
  processor/extension features.

Exit: public wording may say “compatible with Asciidoctor core `<version>` under
the documented profile,” never simply “drop-in compatible with Asciidoctor.”
