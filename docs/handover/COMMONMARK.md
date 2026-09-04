# CommonMark compliance plan

This is the step-by-step implementation and evidence plan for making Markup++
fully compliant with **CommonMark 0.31.2**. It is a living handover: record
results, architectural discoveries and deliberately deferred work here as each
checkpoint closes.

Authoritative references:

- specification and version index: <https://spec.commonmark.org/0.31.2/>;
- pinned test data: <https://spec.commonmark.org/0.31.2/spec.json>;
- upstream specification/test runner:
  <https://github.com/commonmark/commonmark-spec>.

The target is intentionally versioned. “CommonMark compliant” means the strict
core profile produces the expected HTML for all **652 official 0.31.2 spec
examples**, subject only to the normalization permitted by the official test
runner. Passing a hand-picked feature suite or producing visually equivalent
browser output is not sufficient.

CommonMark core does not include Markup++ extensions such as pipe tables, task
list checkboxes or strikethrough. Those features must be disabled in the strict
profile and tested independently. Raw HTML and CommonMark autolinks are core
features and must not be confused with extension handling.

## Rules for every checkpoint

Each checkpoint follows the same loop:

1. Add the relevant official examples to the enabled conformance run.
2. Add focused local regressions for bugs and important neighboring cases.
3. Implement the syntax through the parser model, not example-number branches.
4. Run the focused section, the full enabled corpus and all existing tests.
5. Run adversarial, mutation and sanitizer gates.
6. Record the before/after pass count and remaining failures by spec section.
7. Update public compatibility claims only when the checkpoint's exit criteria
   are met.

Never mark an official example as expected-failing merely to make CI green.
Temporary expected failures may be tracked in a separate manifest containing
the example number, spec section, reason and owning future checkpoint. The
harness must fail if an expected failure unexpectedly passes so the manifest
cannot become stale.

## CM0 - Freeze the target and build the conformance harness

Goal: make progress measurable before changing parser behavior.

- Vendor the CommonMark 0.31.2 `spec.json` test corpus under `tests/fixtures/`
  with its upstream URL, version, license/provenance note and checksum.
- Do not download mutable `current` data during ordinary tests or CI.
- Add a small adapter executable that accepts Markdown on stdin and writes only
  an HTML fragment on stdout. Diagnostics belong on stderr.
- Add a data-driven runner capable of selecting examples by number and section,
  showing useful diffs, and emitting total/passed/failed counts.
- Ensure the adapter uses a strict CommonMark option profile: extensions off,
  raw HTML on, fragment output, deterministic line endings.
- Record the current baseline by section without treating it as a quality score.
- Add Make targets such as `test-commonmark`, `test-commonmark-section` and
  `commonmark-report`.

Exit criteria:

- Every one of the 652 examples executes through the same public conversion API
  used by embedders.
- A deliberately corrupted expected result makes the harness fail.
- Section filtering and exact example reproduction work locally and in CI.
- The baseline report is committed; no compliance claim is made yet.

## CM1 - Establish a source model and parser invariants

Goal: create foundations capable of expressing the specification cleanly.

- Normalize LF, CRLF and lone CR as CommonMark line endings while preserving
  meaningful tabs and source characters.
- Replace embedded NUL (`U+0000`) with `U+FFFD` as required by the spec.
- Define a block tree with document, block quote, list, item, paragraph,
  heading, thematic break, code block and HTML block nodes.
- Define inline nodes for text, soft/hard breaks, code, emphasis, strong, link,
  image and raw HTML.
- Keep source positions internally even if they are not public yet; positions
  make delimiter/container bugs and future diagnostics tractable.
- Separate parsing from HTML rendering. The renderer must not infer syntax from
  already-rendered strings.
- Establish ownership and maximum-depth policies that work without recursion
  proportional to hostile input depth.

Exit criteria:

- Existing supported behavior is represented through the new node model.
- Text cannot be escaped twice or interpreted after rendering.
- Embedded NUL, all line-ending forms, tabs and empty input have regressions.
- ASan/UBSan and mutation tests pass.

## CM2 - Implement block parsing primitives and precedence

Goal: make leaf blocks conform before adding complex containers.

Implement and close the official sections for:

- tabs and indentation;
- backslash escapes at block boundaries;
- blank lines;
- thematic breaks;
- ATX headings;
- Setext headings;
- indented code blocks;
- fenced code blocks, including info strings and closing-fence rules;
- paragraphs and interruption precedence.

Use a line-by-line block parser with explicit precedence. Do not solve conflicts
with independent regex replacements: the same line may be a heading, thematic
break, list marker, code block continuation or paragraph continuation depending
on container state and indentation.

Exit criteria:

- All official examples in the listed sections pass.
- Ambiguous neighbors are covered locally, especially Setext/thematic-break/list
  conflicts, fence length/character/indentation, and paragraph interruption.
- No regression in currently enabled sections.

## CM3 - Implement HTML blocks and inline HTML

Goal: support all raw HTML categories with exact termination behavior.

- Implement the seven CommonMark HTML block start/termination categories.
- Respect indentation and container context.
- Distinguish block HTML from inline HTML and ordinary angle-bracket text.
- Preserve raw HTML when enabled.
- Keep `allow_raw_html=false` as a Markup++ rendering policy layered after
  parsing; safe mode must not change how surrounding Markdown is recognized.

Exit criteria:

- All HTML block and raw HTML official examples pass in strict mode.
- Safe-mode tests prove that parsing structure remains stable while raw nodes
  are escaped or suppressed according to the documented policy.
- Script-like text, comments, declarations, processing instructions, CDATA and
  malformed tags have focused regressions.

## CM4 - Implement container blocks

Goal: correctly represent block quotes and list structure, including nesting.

- Implement block quote continuation, nesting and lazy paragraph continuation.
- Implement bullet and ordered list markers, delimiter changes and ordered-list
  start numbers.
- Calculate list-item content indentation from marker width and following
  whitespace rather than assuming a fixed indent.
- Support empty items, multi-block items, nested containers and code within
  items.
- Determine tight versus loose lists from blank-line structure and render
  paragraph tags accordingly.
- Handle list interruption rules and precedence with thematic breaks/headings.
- Use an open-container stack so mixed block quotes and lists are processed in a
  single line pass.

Exit criteria:

- All block quote, list item and list official examples pass.
- Deep mixed nesting has bounded-stack tests.
- Tight/loose rendering, lazy continuation, empty items and changing marker
  styles have dedicated regressions.

## CM5 - Implement inline lexical primitives

Goal: finish inline features that do not require paired delimiter resolution.

- Implement the exact ASCII punctuation escape set.
- Implement named and numeric character references with the required HTML5
  entity data and exclusions in code spans/blocks.
- Implement code spans with arbitrary backtick-run lengths and whitespace
  normalization.
- Implement soft line breaks and both hard-break forms, including the terminal
  backslash form.
- Implement CommonMark autolinks for absolute URIs and email addresses.
- Implement the complete inline HTML grammar.
- Preserve Unicode text; define and test the library's UTF-8 input policy while
  applying CommonMark character-class rules by Unicode code point where needed.

Exit criteria:

- Every official example in backslash escapes, entities, code spans, line
  breaks, autolinks and raw HTML passes.
- Long delimiter runs and entity-like strings remain linear-time.
- Code content bypasses entity, emphasis and link interpretation.

## CM6 - Implement emphasis and strong emphasis

Goal: replace simplified marker matching with the specified delimiter algorithm.

- Scan `*` and `_` runs into delimiter records.
- Compute left-flanking/right-flanking status using Unicode whitespace and
  punctuation classes.
- Apply the underscore-specific open/close restrictions.
- Implement rule-of-three behavior and correct consumption of delimiter runs.
- Resolve nested and overlapping emphasis/strong delimiters using a delimiter
  stack and openers-bottom optimization.
- Preserve unmatched delimiters as literal text.

Exit criteria:

- All emphasis/strong official examples pass.
- The pathological emphasis cases identified by the CommonMark parsing appendix
  complete within an explicit linear or tightly bounded target.
- Mixed Unicode punctuation/whitespace and long runs have regressions.

This checkpoint should not be attempted as patches to the current recursive
string substitution logic. A delimiter stack is the durable implementation.

## CM7 - Implement links, images and reference definitions

Goal: complete the most stateful inline family.

- Collect link reference definitions during block parsing before inline parsing.
- Normalize reference labels exactly, including whitespace and Unicode case
  folding requirements.
- Implement inline links/images and full, collapsed and shortcut references.
- Implement destination parsing for angle-bracket and unbracketed forms,
  balanced parentheses, escapes and titles using all permitted delimiters.
- Resolve bracket pairs with the bracket stack and enforce the no-links-inside-
  links rule while still allowing images containing links.
- Apply precedence among code spans, autolinks, raw HTML, emphasis and links.
- Implement renderer URL normalization/percent-encoding needed to match the
  official HTML examples, separately from syntax recognition.

Exit criteria:

- All link reference definition, links and images official examples pass.
- Duplicate/missing definitions, nested brackets, empty labels, escaped
  punctuation and hostile nesting have regressions.
- Reference lookup is bounded and does not rescan the whole document per link.

## CM8 - Complete rendering fidelity and extension isolation

Goal: ensure the parsed tree produces exactly conforming core HTML without
breaking useful optional features.

- Match required escaping in text, attributes, code and URLs.
- Match HTML element choices, attributes and newline placement expected by the
  official examples.
- Keep fragment output as the conformance boundary; standalone wrapping is
  tested separately.
- Introduce an explicit extension profile rather than enabling extensions based
  on incidental syntax recognition.
- Verify strict mode treats pipe tables, task markers and strikethrough as plain
  CommonMark input where the core spec requires that result.
- Test every combination of strict/extensions, raw/safe and fragment/standalone
  options without allowing renderer policy to change the parse tree.

Exit criteria:

- The full core corpus passes with every extension disabled.
- Each extension has isolated tests showing it neither changes unrelated core
  parsing nor contaminates the compliance harness.
- Existing public options have documented, deterministic interactions.

## CM9 - Close all 652 examples and prevent regressions

Goal: reach measured conformance, not an approximate endpoint.

- Drive the expected-failure manifest to zero.
- Review every fix for example-specific conditionals and replace them with a
  general rule where found.
- Run differential tests against `cmark` for generated and curated documents,
  classifying differences rather than blindly copying output.
- Add every independently discovered bug as a minimal local regression even if
  an official example also covers it.
- Produce a section-by-section report showing 652/652.

Exit criteria:

- Three clean consecutive full runs produce 652/652.
- The expected-failure manifest is empty and the harness enforces that it stays
  empty.
- Existing API, CLI, adversarial, mutation, safe-mode and extension suites pass.
- A clean checkout can reproduce the result without network access.

## CM10 - Robustness, portability and compliance release gate

Goal: make the conformance result safe to publish and maintain.

- Run Linux, macOS and Windows CI with at least GCC, Clang and MSVC coverage.
- Run ASan/UBSan where supported and independent leak/RSS testing on a capable
  Linux runner.
- Add coverage-guided fuzzing seeded by all 652 examples and local regressions.
- Add adversarial complexity fixtures for delimiter runs, brackets, nested
  containers, reference labels, entities, fences and malformed HTML.
- Set measured time/memory budgets on representative large documents and
  pathological inputs; prevent accidental quadratic behavior.
- Differentially test multiple randomized corpora against the pinned `cmark`
  version, manually resolving every semantic difference.
- Audit recursion/depth limits, integer arithmetic, iterator validity, error
  paths and deterministic output.
- Update README, release notes, website and Nift integration handovers with the
  exact versioned claim and extension profile.

Exit criteria:

- All 652 CommonMark 0.31.2 examples pass on every supported platform.
- All robustness, sanitizer, fuzz, performance and existing regression gates
  pass from a clean checkout.
- The public wording says “CommonMark 0.31.2 compliant,” not unversioned “all
  Markdown,” and separately lists extensions.
- The pinned corpus and an upgrade procedure remain in-tree so future spec
  versions can be evaluated deliberately.

## Maintaining compliance after CM10

- Every Markdown parser change runs the complete corpus, not only affected
  sections.
- Every bug receives a minimal regression before its fix.
- Spec upgrades are separate checkpoints: vendor the new corpus, review its
  changelog, record the old/new baseline, implement differences, then update the
  advertised version.
- Never silently retarget the `current` CommonMark URL.
- Nift's embedded copy must run the same conformance corpus and a byte/scope sync
  check before Nift can inherit Markup++'s compliance claim.
- AsciiDoc and reStructuredText work may share rendering utilities, but must not
  weaken CommonMark parsing rules or its strict option profile.

## Progress ledger

Update this table when a checkpoint closes. Record exact evidence in the commit
message or linked report rather than replacing criteria with a subjective note.

| Checkpoint | Status | Official examples | Notes |
|---|---|---:|---|
| CM0 - Harness | Complete | 198/652 baseline | Corpus SHA-256 pinned; strict extensions-off adapter |
| CM1 - Model | Complete | 652/652 strict engine | Vendored cmark 0.31.1 AST/parser/HTML renderer |
| CM2 - Leaf blocks | Complete | 139/139 selected | Named section gate plus ambiguity regressions |
| CM3 - HTML | Complete | 64/64 selected | Seven block types, inline HTML and safe-policy regressions |
| CM4 - Containers | Complete | 99/99 selected | Mixed nesting, lazy continuation and tight/loose regressions |
| CM5 - Inline primitives | Complete | 108/108 selected | Escapes, entities, code, breaks, autolinks, raw HTML and UTF-8 validation |
| CM6 - Emphasis | Complete | 132/132 selected | Delimiter-stack behavior plus four 30k-run complexity guards |
| CM7 - Links/images | Complete | 139/139 selected | References/brackets plus three large-input lookup guards |
| CM8 - Rendering/extensions | Complete | 652/652 default core | API v2 profile, explicit legacy extensions and option matrix |
| CM9 - Full corpus | Complete | 652/652 three times | Empty enforced manifest; 1,652 reference differential cases |
| CM10 - Release gate | Local gates complete; pushed CI pending | 652/652 local | GCC local; Clang/macOS, MSVC/Windows and libFuzzer await Actions |
