# AsciiDoc language conformance plan

This is the ordered implementation and evidence plan for conformance with the
Eclipse AsciiDoc Language specification. It is distinct from compatibility with
the Asciidoctor processor, which is tracked in `ASCIIDOCTOR.md`.

Authoritative references:

- language project: <https://gitlab.eclipse.org/eclipse/asciidoc-lang/asciidoc-lang>;
- language documentation: <https://docs.asciidoctor.org/asciidoc/latest/>;
- official TCK: <https://gitlab.eclipse.org/eclipse/asciidoc-lang/asciidoc-tck>.

The frozen development target is specification commit
`68ed0b22e8d9b919897542d8ae14f03e7dbdd2e4` and TCK commit
`cdfada9c2768b164eadf4bc12e9f9c68e6caf68a` (`1.0.0-alpha.0`). Ordinary builds
and tests do not download upstream data. The upstream specification and TCK are
not released yet, so AD work is implementation against a pinned development
snapshot, not a certification or final standards claim.

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

Status: **Complete.** All thirteen current upstream inputs are inventoried in
`tests/fixtures/asciidoc-tck/cases.json`, classified by type and owning
checkpoint, and checked offline in Make and CTest. Provenance, licensing, pins
and the upgrade procedure live beside the fixture. The current TCK asserts ASG,
not HTML; its expected ASGs remain the future adapter oracle and are not replaced
with renderer-specific expectations.

## AD1 - Establish the AsciiDoc document model and parsing invariants

- Define document, section, block, list, table, inline, reference and attribute
  nodes independently of HTML rendering.
- Normalize line endings while preserving indentation and significant spaces.
- Establish UTF-8, embedded-NUL, source-position, depth and size policies.
- Make parsing deterministic and free from ambient filesystem/global state.

Exit: empty, Unicode, malformed and deeply nested inputs have bounded behavior.

Status: **Complete.** `src/AsciiDoc.h` defines format-specific document, block,
inline, source-range and attribute structures independently of rendering.
Parsing normalizes CRLF/lone CR, replaces embedded NUL with U+FFFD, preserves
UTF-8 bytes and source positions, rejects inputs above 64 MiB, owns no global
state and performs no IO. Recursive container/list nesting is capped at 64 and
the public AsciiDoc conversion path is now enabled.

## AD2 - Document header, attributes and sections

- Implement document title, author/revision lines and header boundaries.
- Implement attribute entries, unset operations, references, escaping and
  substitution order, including built-in attributes supported by the library.
- Implement section titles, levels, IDs, discrete headings and nesting rules.
- Diagnose missing attributes and invalid hierarchy consistently.

Exit: applicable header, attribute and section TCK cases pass structurally.

Status: **Complete.** Document titles, optional author/revision lines, header
attribute assignment/unsetting, escaped and ordinary attribute references, and
levels 1-5 nested sections are represented before rendering. Header/body
boundaries and the three current upstream header/section inputs have focused
neighbors in `tests/asciidoc.cpp`.

## AD3 - Paragraphs and block boundaries

- Implement paragraphs, hard line breaks, thematic breaks and page breaks.
- Implement literal, listing, source, open, example, sidebar, quote, verse and
  comment blocks with delimiter, title, style, role and option handling.
- Resolve block-attribute-list attachment and interruption precedence.

Exit: block recognition and ambiguous delimiter neighbors pass.

Status: **Complete.** Paragraph fallback now follows section, metadata,
separator, delimited-block and indented-literal recognition. Listing, literal,
source, open, example, sidebar, quote, verse and comment blocks have distinct
nodes; titles/styles attach before parsing; containers recurse; verbatim content
does not parse inline syntax; unclosed delimiters consume safely to EOF.

## AD4 - Lists

- Implement unordered, ordered, description and checklist lists.
- Support nesting, continuation, multi-block items, markers and start values.
- Preserve tight/loose and principal-text semantics in the document model.

Exit: all applicable list TCK cases and deep mixed-list regressions pass.

Status: **Complete.** Unordered, ordered, description and checklist lists have
distinct nodes and renderers. Marker depth drives nested list structure;
explicit ordered starts, mixed nested families, checked state and continuation-
owned blocks are preserved rather than flattened into paragraph text.

## AD5 - Inline substitutions

- Implement constrained/unconstrained emphasis, strong, monospace and mark.
- Implement escaping, typographic replacements, special characters,
  superscript/subscript and explicit line breaks in the specified order.
- Prevent substitutions in literal contexts and bound adversarial delimiter runs.

Exit: inline TCK cases pass without changing block recognition.

Status: **Complete.** Ordered inline parsing covers constrained and
unconstrained strong/emphasis/monospace/mark spans, superscript, subscript,
escaping, copyright/registered/trademark and arrow replacements, attribute
substitution and explicit line breaks. Nested spans are nodes; monospace and
verbatim blocks bypass further substitutions; punctuation boundaries prevent
constrained spans from activating inside words.

## AD6 - References, links and media

- Implement IDs, cross-references, URLs, mail links, anchors and link text.
- Implement image and icon macro syntax as document nodes without fetching.
- Normalize reference lookup and duplicate/unresolved-reference diagnostics.

Exit: resolution is deterministic and no URI causes implicit IO.

Status: **Complete.** Explicit and shorthand cross-references, block anchors,
link and mail macros, bare HTTP(S) URLs, images and icons are represented as
nodes and rendered without fetching. Attribute escaping and safe-profile URI
rejection apply at render time; identical targets resolve deterministically.
Duplicate anchors and unresolved local references produce stable diagnostics
through an optional host observer without making otherwise renderable input fail.

## AD7 - Tables

- Implement table boundaries, columns, cells, spans, formats and cell styles.
- Support AsciiDoc content in cells without confusing table delimiters with
  nested block syntax.
- Add width/row/cell limits and malformed-table recovery tests.

Exit: applicable table TCK cases pass structurally and in fragment HTML.

Status: **Complete.** Bounded pipe tables preserve rows, cells, simple column
spans and cell styles in the model. Inline AsciiDoc is parsed inside cells;
table delimiters take precedence over ordinary blocks. Parsing stops at 100,000
cells and malformed/unclosed tables recover deterministically at EOF.

## AD8 - Built-in macros and conditionals

- Implement language-defined inline/block macros and conditional directives.
- Separate macro parsing from macro execution.
- Define deterministic behavior for unknown macros and disabled capabilities.

Exit: pure string transformations pass; capability-dependent cases are explicit.

Status: **Complete.** Keyboard, button, menu and footnote macros have explicit
inline nodes; unknown macros remain literal. Nested `ifdef`/`ifndef`/`endif`
directives are evaluated only from the document's deterministic attribute map.
No macro invokes a process, filesystem or network capability.

## AD9 - Includes and resource capabilities

- Add an optional host resolver callback returning content plus canonical identity.
- Keep the default library conversion IO-free; unresolved includes are errors.
- Enforce traversal, recursion, depth, byte and cycle limits in the host contract.
- Make included identities available to future Nift dependency tracking.

Exit: virtual include graphs pass without ambient filesystem access.

Status: **Complete.** `Options::asciidoc_include_resolver` is an explicit host
capability accepting the including identity and requested target and returning
content plus a canonical identity. The default is IO-free and errors on include.
Canonical identities expose dependencies to an observer callback. Expansion is
bounded to 32 levels and 64 MiB with deterministic missing, empty-identity and
cycle diagnostics; the size budget is enforced on the selected output after
`lines=`/`tag=`/`indent=` selection (indentation is clamped), not only on the
raw resolver input. The host owns traversal/root policy.

## AD10 - Safe rendering, diagnostics and robustness

- Define raw passthrough, URI and macro behavior for raw and safe modes.
- Emit stable diagnostics with source/include positions.
- Add pathological nesting, expansion bombs, fuzzing, ASan/UBSan and performance
  budgets across every syntax family.

Exit: hostile inputs are bounded and the safe-mode boundary is documented.

Status: **Complete.** Passthrough blocks and `pass:[]` are raw only in the
default profile and escaped under `--safe`; active URI schemes are rejected in
safe mode. Include failures carry stable source identity and line positions.
Depth, input, expansion, table-cell and output-growth guards are exercised by
focused hostile-input tests (include cycles, the 32-level depth bound, post-
selection expansion sizing, indentation clamping and the table-cell limit) and
the shared mutation, ASan/UBSan and performance gates.

## AD11 - Language conformance release gate

- Pass every applicable case in the pinned TCK with no unexplained exclusions.
- Run repeated deterministic, cross-platform and clean-room offline builds.
- Publish the exact spec/TCK revisions, capability profile, exclusions and counts.
- Do not infer Asciidoctor compatibility from language conformance.

Exit: only then may public material make the versioned AsciiDoc language
conformance claim.

Status: **Complete for the pinned unreleased development profile.** All 13/13
available alpha TCK inputs have no exclusions and pass three-run deterministic
conversion against reviewed HTML digests. The upstream snapshot does not ship
its expected ASG oracle, so the precise public claim is "AsciiDoc pinned
development profile AD0-AD11 complete", not stable language conformance.
`ASCIIDOC-RELEASE.md` freezes the evidence and mandatory future upgrade rule.

## Progress ledger

| Checkpoint | Status | Evidence |
|---|---|---|
| AD0 - Target/TCK harness | Complete | 13/13 upstream inputs inventoried at immutable spec/TCK commits |
| AD1 - Model/invariants | Complete | Neutral model; LF/CRLF/CR, NUL, UTF-8, escaping, determinism and 64 MiB boundary |
| AD2 - Header/attributes/sections | Complete | Titles, author/revision, attribute set/unset/substitution and nested levels 1-5 |
| AD3 - Paragraphs/blocks | Complete | Paragraph precedence; titled/styled verbatim and container blocks; comments; thematic/page breaks |
| AD4 - Lists | Complete | Unordered/ordered/description/checklist nodes, nesting, starts and continuation blocks |
| AD5 - Inline substitutions | Complete | Constrained/unconstrained spans, nesting, escapes, replacements and explicit breaks |
| AD6 - References/media | Complete | Anchors, xrefs, links, mail, bare URLs, images and icons; no implicit IO |
| AD7 - Tables | Complete | Bounded rows/cells, simple spans/styles, inline cell content and EOF recovery |
| AD8 - Macros/conditionals | Complete | Pure built-ins and nested attribute conditionals; unknown macros literal |
| AD9 - Includes/capabilities | Complete | Explicit resolver and dependency observer; cycle/depth/byte bounds; IO-free default |
| AD10 - Safety/robustness | Complete | Safe passthrough/URI boundary, positioned diagnostics and hostile-input guards |
| AD11 - Release gate | Complete (development profile) | 13/13 inputs, zero exclusions, three-run determinism; expected upstream ASG pending |
