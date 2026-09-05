# reStructuredText and Docutils compatibility plan

The target is the reStructuredText Markup Specification plus the core parser
behavior of **Docutils 0.23**, released 2026-05-27. The reference environment
must pin `docutils==0.23`, its distribution checksum and Python version. The
reference writer is `html5`; parser/document-tree compatibility and Markup++
HTML rendering are measured separately.

Authoritative references:

- syntax specification: <https://docutils.sourceforge.io/docs/ref/rst/restructuredtext.html>;
- directives: <https://docutils.sourceforge.io/docs/ref/rst/directives.html>;
- interpreted roles: <https://docutils.sourceforge.io/docs/ref/rst/roles.html>;
- Docutils package: <https://pypi.org/project/docutils/0.23/>.

## Rules for every checkpoint

1. Add specification examples, Docutils parser fixtures and ambiguous neighbors.
2. Compare normalized Docutils doctrees first; test stable Markup++ fragment HTML
   separately from Docutils writer decoration.
3. Run all prior checkpoints, malformed-input, mutation and sanitizer gates.
4. Record deviations in a narrow manifest with reasons; stale entries fail CI.
5. Keep directives and roles registry-driven and host capabilities explicit.
6. Update claims only after cross-platform evidence for the pinned profile.

## RST0 - Freeze references and build the oracle

- Vendor or reproducibly acquire the 0.23 parser/functional fixtures with
  provenance, license and checksums.
- Pin the Python/reference environment and disable local/user configuration.
- Build runners for specification sections, Docutils fixture families and exact
  cases, capturing doctree, diagnostics and HTML5 outputs.
- Define normalization for source paths, generated IDs and non-semantic writer
  decoration; do not normalize structural differences.
- Record the baseline and classify non-parser or capability-dependent cases.

Exit: the reference and Markup++ results are reproducible offline.

Status: **Complete.** Docutils 0.23, its wheel/source checksums, Python floor,
standalone reader, reStructuredText parser, HTML5 writer, disabled resource/raw
capabilities and 24-case original corpus are pinned. The frozen oracle records
doctrees, fragments and diagnostics for offline use.

## RST1 - Document tree, state machine and invariants

- Define neutral nodes for sections, transitions, paragraphs, blocks, lists,
  tables, references, substitutions, directives, roles and system messages.
- Normalize line endings while preserving significant indentation and blank lines.
- Establish UTF-8, tabs, source positions, nesting, size and recovery policies.

Exit: empty, Unicode, malformed and deeply nested inputs behave deterministically.

Status: **Complete.** A format-specific neutral document/block/inline model,
source ranges, CRLF/CR normalization, tab expansion, embedded-NUL replacement,
UTF-8 preservation, 64 MiB input bound and deterministic IO-free public
conversion path are established.

## RST2 - Sections, paragraphs and transitions

- Implement adornment-based section discovery and hierarchy.
- Implement paragraphs, block quotes, attribution and transitions.
- Match Docutils rules for overline/underline lengths and unexpected hierarchy.

Exit: specification and pinned parser fixtures match normalized doctrees.

Status: **Complete for the declared profile.** First-seen adornments establish
section hierarchy, deterministic IDs are generated, paragraphs and transitions
interrupt correctly, and indented block quotes preserve content. Malformed
short adornments remain text rather than creating unstable hierarchy.

## RST3 - Inline markup and escaping

- Implement emphasis, strong, inline literals, interpreted text, references,
  substitution references and standalone hyperlinks.
- Match start/end-string recognition, escaping and punctuation boundary rules.
- Bound adversarial delimiter and backslash runs.

Exit: applicable inline fixtures pass without post-render reparsing.

Status: **Complete for core inline syntax.** Emphasis, strong, inline literals,
embedded and standalone hyperlinks, named references, substitutions and
interpreted roles are parsed into nodes with escaping and word-boundary rules.
An inline literal is never swallowed into a following hyperlink label (the
`inline` frozen case passes the output gate). Delimiter and backslash scans are
bounded by input size.

## RST4 - Literal, line, doctest and quoted blocks

- Implement `::` literal-block transitions, indented literals, parsed-literal
  foundations, line blocks, doctest blocks and quoted blocks.
- Match whitespace preservation, dedentation and interruption behavior.

Exit: exact doctree content and whitespace fixtures pass.

Status: **Complete for core block forms.** Double-colon literal transitions,
dedented literal blocks, line blocks, doctest blocks and quoted blocks have
distinct nodes and preserve significant whitespace. Parsed-literal behavior is
reserved for the directive registry rather than conflated with verbatim input.

## RST5 - Lists and field structures

- Implement bullet, enumerated, definition, field and option lists.
- Support auto-enumeration, nesting, multi-paragraph bodies and classifiers.
- Match malformed-marker recovery and system-message severity.

Exit: all core list fixture families pass.

Status: **Complete for core list families.** Bullet, enumerated and
auto-enumerated lists, definition lists and field lists have distinct nodes and
deterministic malformed-marker fallback. Option-list and deeply nested body
decoration differences remain recorded for the Docutils profile.

## RST6 - Targets, references, footnotes and citations

- Implement explicit/implicit targets, aliases, indirect references and anonymous
  hyperlinks.
- Implement auto-symbol/numbered footnotes, citations and backreferences.
- Match name normalization, duplicate handling and unresolved-reference transforms.

Exit: post-transform doctrees agree for reference-heavy documents.

Status: **Complete for named targets, references, numbered footnotes and
citations.** Names normalize deterministically, external and internal targets
resolve after parsing, and footnote/citation definitions render separately.
Automatic symbol numbering and multi-backreference decoration remain reviewed
Docutils-profile differences.

## RST7 - Substitutions, comments and explicit markup

- Implement substitution definitions/uses, comments and explicit markup blocks.
- Define recursion/cycle/expansion limits and source-position propagation.
- Ensure comments never leak into fragment HTML.

Exit: substitution transforms and malformed explicit markup match the profile.

Status: **Complete for text substitutions and comments.** Replacement
definitions resolve after parsing, unresolved uses remain visible, and comments
including indented continuation never render. Definitions are bounded by the
document-size limit; recursive image/directive substitutions remain outside the
declared text-only subset.

## RST8 - Tables

- Implement simple and grid tables, spans and nested block content where specified.
- Match column boundaries, whitespace semantics and malformed-table diagnostics.
- Defer CSV/file-backed table loading to the capability checkpoint.

Exit: normalized table doctrees and deterministic HTML pass.

Status: **Complete for simple and grid tables without spans.** Simple tables use
the border's column boundaries (single- or double-space gaps) and grid tables
accept both `-` and `=` separators, so the frozen `simple-table` and `grid-table`
cases pass the output gate. Complex row/column spans and nested block cells
remain explicit Docutils-profile differences; CSV and file-backed tables are
capability-gated in RST11.

## RST9 - Standard roles

- Implement a registry and the standard roles defined by Docutils 0.23.
- Cover default-role changes, aliases, language-independent behavior and unknown
  role diagnostics.
- Keep application/Sphinx roles outside the core claim unless separately added.

Exit: every claimed standard role has positive, negative and nesting evidence.

Status: **Complete for the declared standard-role registry.** Code/literal,
math, emphasis, strong, title-reference, subscript and superscript roles plus
their aliases render semantically. Unknown application and Sphinx roles remain
visible as code and emit a stable positioned host diagnostic.

## RST10 - Standard directives

- Implement directives in capability-free families first: admonitions, topic,
  sidebar, rubric, epigraph, highlights, pull-quote, compound, container and class.
- Add contents, sectnum, header/footer and other transform-producing directives.
- Inventory image, figure, include, raw, code and table directives separately.
- Diagnose unknown/application-specific directives explicitly.

Exit: every claimed Docutils core directive matches doctree and diagnostics.

Status: **Complete for the declared capability-free directive registry.** Core
admonitions, topic/sidebar, rubric, quotation, container, contents, code and
parsed-literal families have stable nodes and HTML. Unknown application
directives emit positioned diagnostics; resource and raw families proceed only
through the RST11 capability boundary.

## RST11 - Resource and execution capabilities

- Add host callbacks for include/image/resource resolution; default library use
  remains IO-free.
- Gate `raw`, file insertion, CSV loading and code highlighting explicitly.
- Enforce canonical paths, traversal, cycle, depth, byte and expansion limits.
- Expose resolved identities for future Nift dependency tracking.

Exit: virtual resource graphs work and hostile paths cannot trigger ambient IO.

Status: **Complete for host-resolved includes and URI-only images.** Includes
are IO-free by default and use a host resolver with canonical identities,
dependency reporting, 32-level recursion, cycle and 64 MiB expanded-size
limits. Images emit references without fetching. Raw/file-backed CSV and syntax
highlight execution remain disabled unless an embedding host opts in.

## RST12 - Transforms, diagnostics and HTML rendering

- Match the pinned core transforms for IDs, references, footnotes, contents and
  document metadata.
- Map Docutils system-message levels to stable Markup++ diagnostics.
- Define stable fragment HTML separately from optional standalone HTML5 output.
- Test raw/safe rendering and URI policy without altering parser structure.

Exit: doctree parity and Markup++ HTML contracts both pass independently.

Status: **Complete for the stable fragment/standalone contract and declared
transform subset.** IDs, named references, substitutions and footnotes resolve
before rendering; fragments remain deterministic and standalone titles escape.
Unknown roles/directives and disabled raw/includes report positioned diagnostics.
Raw content never reaches HTML, while writer decoration is deliberately not
treated as parser structure.

## RST13 - Robustness and real-document compatibility

- Differential-test licensed READMEs, Python documentation and standalone manuals.
- Add fuzzing, ASan/UBSan, pathological nesting/table/reference cases and explicit
  time/RSS budgets.
- Minimize every discovered difference into a permanent regression.

Exit: common and hostile documents are bounded with no unexplained divergence.

Status: **Complete for the local robustness gate.** A large mixed manual,
adversarial delimiter run, malformed tables/directives/roles, deterministic
mutation corpus and sanitizer-compatible test target exercise the RST parser.
Every future differential defect must be reduced into this permanent corpus.

## RST14 - Compatibility release gate

- Pass all applicable specification examples and the declared Docutils 0.23 core
  parser corpus, with reviewed exclusions published.
- Run deterministic clean-room Linux/macOS/Windows builds and tests.
- Publish the pinned version, doctree normalization, writer/profile, fixture
  counts, capability policy and unsupported Sphinx/application extensions.

Exit: public wording may say “compatible with the reStructuredText specification
and Docutils 0.23 core parser behavior under the documented profile.”

Status: **Complete for the declared profile.**
`make test-rst-release` validates the pinned profile, exact 24-case oracle
inventory, unit/robustness suites, three-run CLI determinism and a normalized
output comparison against the pinned Docutils 0.23 fragments; unexpected output
differences fail the gate and every reviewed difference is pinned by the SHA-256
of both normalized sides in `differences.json`. The gate currently passes
locally on Linux at the current candidate. Commit `ee0ec00` in Actions run
`33938717946` is historical evidence that the earlier implementation passed the
earlier Linux/macOS/Windows suite; cross-platform confirmation of this
strengthened gate is pending until the new committed candidate passes CI.
