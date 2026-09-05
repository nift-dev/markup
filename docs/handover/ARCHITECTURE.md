# Architecture

## Layers

Markup++ has three intentionally separate layers:

1. `include/markup/Markup.h` defines formats, options and the string conversion
   contract.
2. `src/Markup.cpp` owns format dispatch and standalone wrapping. Markdown and
   AsciiDoc and reStructuredText parsing remain isolated format implementations.
3. `cli/main.cpp` owns argument parsing, extension inference, streams and atomic
   file replacement.

The separation lets Nift embed the conversion layer without inheriting CLI or
filesystem behavior.

## Format dispatch

`markup::Format` represents Markdown, AsciiDoc and reStructuredText.
`format_for_extension` recognizes all extensions, while `is_supported` and
`convert` dispatches to three enabled implementations. Each converter retains
its own parser model while sharing only the public conversion and wrapper
boundary.

Each future converter should be isolated behind the same dispatch boundary.
Avoid a single parser with format conditionals scattered through tokenization.
Shared HTML escaping/output utilities are appropriate; shared source syntax
assumptions are not.

## AsciiDoc pipeline

`src/AsciiDoc.h` defines a format-specific neutral document model with source
ranges, document attributes, block nodes and inline nodes. `src/AsciiDoc.cpp`
normalizes and bounds input, parses document structure before inline content,
and renders stable HTML fragments. Keep future AD work in this pipeline rather
than adding AsciiDoc cases to the Markdown engine or CLI.

## Markdown pipeline

Strict CommonMark conversion uses the vendored cmark 0.31.1 block tree, inline
delimiter/bracket stacks and HTML renderer through its in-process C API. cmark
is source-vendored under its BSD 2-Clause license; it is not a runtime or system
dependency. Markup++ retains ownership of format dispatch, profiles, safe/raw
selection, standalone wrapping, diagnostics and the public C++ API.

## reStructuredText pipeline

`src/ReStructuredText.h` defines neutral source-positioned document, block and
inline nodes. `src/ReStructuredText.cpp` owns normalization, parsing, transforms
and stable fragment rendering. Includes cross an explicit host resolver with
canonical dependency identities; ambient IO and raw execution remain disabled.

The extension-enabled compatibility path currently normalizes CRLF while splitting lines, recognizes
block constructs in precedence order, and then applies inline conversion to text
containers. Code blocks bypass inline parsing. Block quotes recursively invoke
the Markdown fragment converter on stripped quote content.

The two paths are explicitly isolated by `Options::MarkdownProfile`: CommonMark
is the default, while `Extended` opts into the original table/task/strikethrough
converter. This preserves early behavior without contaminating the standards
claim. Future extension work should migrate individual extensions onto the
conforming node model before retiring the legacy implementation.

In strict mode, `allow_raw_html=false` uses cmark's safe renderer: raw HTML nodes
are replaced by omission comments and unsafe link/image destinations are
blanked. The transitional extension renderer escapes raw HTML instead. Both
prevent pass-through, but the output distinction must remain documented until
CM8 unifies renderer policy.

Strict mode treats input as UTF-8 and enables cmark validation. Invalid byte
sequences are replaced with `U+FFFD`, matching the parser's existing NUL
replacement policy and ensuring generated HTML remains valid UTF-8. The
transitional extension path retains its original byte-preserving behavior.

Output is deterministic and ends block elements with newlines. Fragment mode is
the library default. Standalone mode wraps the fragment in a minimal HTML5
document and escapes the supplied title.

## Failure contract

`convert` clears both output and error before work. Success leaves error empty.
Unsupported formats fail with empty output and a stable diagnostic. Markdown is
permissive: malformed delimiters generally become escaped literal text, matching
the useful recovery behavior expected of Markdown.

## Nift embedding

Nift embedding is deliberately deferred. When it begins:

- copy a standalone-style Markup++ subtree rather than reaching across repos;
- add a byte/scope synchronization script like Minify++;
- keep standalone Markup++ canonical;
- expose conversion through Nift configuration or a narrow directive only after
  the desired product behavior is decided;
- ensure incremental dependency tracking accounts for the markup source;
- keep conversion before/after templating ordering explicit and tested;
- avoid giving Markup++ access to Nift internals.
