# AsciiDoc language conformance plan

This is the ordered implementation and evidence plan for conformance with the
Eclipse AsciiDoc Language specification. It is distinct from compatibility with
the Asciidoctor processor, which is tracked in `ASCIIDOCTOR.md`.

Authoritative references:

- language project: <https://gitlab.eclipse.org/eclipse/asciidoc-lang/asciidoc-lang>;
- language documentation: <https://docs.asciidoctor.org/asciidoc/latest/>;
- official TCK: <https://gitlab.eclipse.org/eclipse/asciidoc-lang/asciidoc-tck>.

The specification and TCK revision must be pinned by immutable tag or commit in
the repository when AD0 closes. Ordinary builds and tests must not download a
mutable upstream branch. A claim of conformance applies only to that pinned
revision and the explicitly documented processor capabilities.

## Rules for every checkpoint

1. Enable the applicable pinned TCK cases and add focused neighboring cases.
2. Implement syntax in the format parser and neutral document model, not as
   fixture-specific substitutions.
3. Compare the document structure first and deterministic HTML second.
4. Run all completed checkpoints, malformed-input, mutation and sanitizer gates.
5. Record pass counts, exclusions and reasons; stale exclusions must fail CI.
6. Keep filesystem/network access behind explicit host-provided capabilities.
7. Update compatibility claims only when the checkpoint exit criteria pass on
   Linux, macOS and Windows.

## AD0 - Freeze the language and TCK revisions

- Select immutable specification and TCK revisions and record URLs/checksums.
- Confirm TCK license/provenance and vendor the permitted fixtures or a
  reproducible adapter.
- Classify every case as language syntax, processor API, host capability or not
  applicable, with a reviewable reason for exclusions.
- Build an in-process, section-selectable runner with exact diagnostics and
  stale-exclusion detection.
- Record the initial per-section baseline without making a conformance claim.

Exit: every applicable test is discoverable and reproducible offline.

## AD1 - Establish the AsciiDoc document model and parsing invariants

- Define document, section, block, list, table, inline, reference and attribute
  nodes independently of HTML rendering.
- Normalize line endings while preserving indentation and significant spaces.
- Establish UTF-8, embedded-NUL, source-position, depth and size policies.
- Make parsing deterministic and free from ambient filesystem/global state.

Exit: empty, Unicode, malformed and deeply nested inputs have bounded behavior.

## AD2 - Document header, attributes and sections

- Implement document title, author/revision lines and header boundaries.
- Implement attribute entries, unset operations, references, escaping and
  substitution order, including built-in attributes supported by the library.
- Implement section titles, levels, IDs, discrete headings and nesting rules.
- Diagnose missing attributes and invalid hierarchy consistently.

Exit: applicable header, attribute and section TCK cases pass structurally.

## AD3 - Paragraphs and block boundaries

- Implement paragraphs, hard line breaks, thematic breaks and page breaks.
- Implement literal, listing, source, open, example, sidebar, quote, verse and
  comment blocks with delimiter, title, style, role and option handling.
- Resolve block-attribute-list attachment and interruption precedence.

Exit: block recognition and ambiguous delimiter neighbors pass.

## AD4 - Lists

- Implement unordered, ordered, description and checklist lists.
- Support nesting, continuation, multi-block items, markers and start values.
- Preserve tight/loose and principal-text semantics in the document model.

Exit: all applicable list TCK cases and deep mixed-list regressions pass.

## AD5 - Inline substitutions

- Implement constrained/unconstrained emphasis, strong, monospace and mark.
- Implement escaping, typographic replacements, special characters,
  superscript/subscript and explicit line breaks in the specified order.
- Prevent substitutions in literal contexts and bound adversarial delimiter runs.

Exit: inline TCK cases pass without changing block recognition.

## AD6 - References, links and media

- Implement IDs, cross-references, URLs, mail links, anchors and link text.
- Implement image and icon macro syntax as document nodes without fetching.
- Normalize reference lookup and duplicate/unresolved-reference diagnostics.

Exit: resolution is deterministic and no URI causes implicit IO.

## AD7 - Tables

- Implement table boundaries, columns, cells, spans, formats and cell styles.
- Support AsciiDoc content in cells without confusing table delimiters with
  nested block syntax.
- Add width/row/cell limits and malformed-table recovery tests.

Exit: applicable table TCK cases pass structurally and in fragment HTML.

## AD8 - Built-in macros and conditionals

- Implement language-defined inline/block macros and conditional directives.
- Separate macro parsing from macro execution.
- Define deterministic behavior for unknown macros and disabled capabilities.

Exit: pure string transformations pass; capability-dependent cases are explicit.

## AD9 - Includes and resource capabilities

- Add an optional host resolver callback returning content plus canonical identity.
- Keep the default library conversion IO-free; unresolved includes are errors.
- Enforce traversal, recursion, depth, byte and cycle limits in the host contract.
- Make included identities available to future Nift dependency tracking.

Exit: virtual include graphs pass without ambient filesystem access.

## AD10 - Safe rendering, diagnostics and robustness

- Define raw passthrough, URI and macro behavior for raw and safe modes.
- Emit stable diagnostics with source/include positions.
- Add pathological nesting, expansion bombs, fuzzing, ASan/UBSan and performance
  budgets across every syntax family.

Exit: hostile inputs are bounded and the safe-mode boundary is documented.

## AD11 - Language conformance release gate

- Pass every applicable case in the pinned TCK with no unexplained exclusions.
- Run repeated deterministic, cross-platform and clean-room offline builds.
- Publish the exact spec/TCK revisions, capability profile, exclusions and counts.
- Do not infer Asciidoctor compatibility from language conformance.

Exit: only then may public material make the versioned AsciiDoc language
conformance claim.
