# Markup++

**Executable:** `markup`

Markup++ is a small, self-contained C++17 library and CLI for converting markup
formats to HTML. It is designed to work standalone and, after its format
implementations mature, to be embedded by Nift behind the same kind of narrow
library boundary used for Minify++ and Jsonic++.

Markdown is CommonMark 0.31.2 compliant. AsciiDoc conversion is implemented
through checkpoints AD0-AD11 against pinned Eclipse language/TCK development
snapshots, with the independent Asciidoctor 2.0.26 profile implemented through
AC9. reStructuredText is implemented through RST14 against a frozen Docutils
0.23 oracle. The exact processor-compatibility claims remain gated on a pushed
Linux/macOS/Windows candidate run.

The strict CommonMark engine vendors cmark source under its BSD 2-Clause
license. There is no separately installed library or runtime dependency.

```sh
make
make test

./markup README.md
./markup README.md -o README.html
./markup --standalone --title "My document" README.md -o document.html
printf '# Hello\n' | ./markup --format markdown -
./markup --extensions notes.md # opt into tables/tasks/strikethrough
./markup guide.adoc -o guide.html
./markup manual.rst -o manual.html
```

By default, `markup` writes an HTML fragment to standard output. `-o` writes a
file atomically, and `--standalone` adds a minimal HTML document wrapper.
Existing regular-file permissions are preserved; output symlinks and attempts
to overwrite the input are refused.

Markdown defaults to the CommonMark profile. `--extensions` selects the legacy
Markup++ table, task-list and strikethrough profile explicitly.

## Library API

```cpp
#include <markup/Markup.h>

std::string html;
std::string error;
markup::Options options;

if (!markup::convert(markup::Format::Markdown, source, html, error, options)) {
    // Handle error.
}
```

The library accepts and returns strings. It performs no file access, owns no
global mutable state, and does not depend on Nift. File policy belongs to the
CLI or embedding application.

## Markdown checkpoint

The strict default implements the complete CommonMark 0.31.2 syntax model,
including:

- ATX and Setext headings;
- paragraphs, soft breaks and hard breaks;
- emphasis, strong emphasis and code spans;
- inline links, images, URL/email autolinks and entities;
- unordered and ordered lists;
- block quotes;
- fenced and indented code blocks;
- thematic breaks;
- raw HTML pass-through by default;
- `--safe` mode, which prevents raw HTML pass-through and rejects active URL
  schemes;
- CRLF and unterminated-fence handling.

The optional extended profile additionally supports pipe tables with alignment,
task-list checkboxes and strikethrough.

The default Markdown profile is CommonMark 0.31.2 compliant: all 652 official
examples and the reproducibility, differential, portability, fuzz, performance
and cross-platform release gates pass. Compatibility claims grow from executable
evidence rather than the name “Markdown”.

## Tests

```sh
make test          # semantic, adversarial, CLI and 10k mutation gates
make test-release-local # full local CommonMark release candidate gate
make test-sanitize # ASan + UBSan semantic/adversarial gates
# Where LeakSanitizer cannot inspect /proc under the runner:
make test-sanitize ASAN_OPTIONS=detect_leaks=0:halt_on_error=1
```

See [HANDOVER.md](HANDOVER.md) for the living development contract and
`docs/handover/` for architecture, decisions, testing and roadmap detail.

## AsciiDoc checkpoint

The current AsciiDoc profile includes document metadata and attributes, nested
sections, structural and verbatim blocks, lists, inline substitutions,
references/media, bounded tables, pure macros/conditionals, capability-based
includes, and safe passthrough/URI handling.

The target pins Eclipse specification commit `68ed0b22` and TCK commit
`cdfada9c`. The TCK is an unreleased alpha with 13 inputs and an ASG oracle, so
the exact claim is pinned-development-profile completion rather than stable
language conformance because the alpha TCK has not published its expected ASG oracle. See
`docs/handover/ASCIIDOC.md` for the exact evidence and remaining work.

## reStructuredText checkpoint

RST0-RST14 provide a source-positioned parser, core blocks/inlines, lists,
references, substitutions, tables, standard role/directive registries,
host-resolved includes, diagnostics, deterministic rendering and robustness and
release gates. The reference profile pins Docutils 0.23's standalone reader,
reStructuredText parser and HTML5 writer with raw/file insertion disabled.
See `docs/handover/RESTRUCTUREDTEXT.md` for documented subsets and the remaining
cross-platform publication gate.
