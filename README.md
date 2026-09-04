# Markup++

**Executable:** `markup`

Markup++ is a small dependency-free C++17 library and CLI for converting markup
formats to HTML. It is designed to work standalone and, after its format
implementations mature, to be embedded by Nift behind the same kind of narrow
library boundary used for Minify++ and Jsonic++.

The first development checkpoint implements Markdown. AsciiDoc and
reStructuredText are recognized by the format-neutral API and CLI but return a
clear not-implemented error until their converters land.

```sh
make
make test

./markup README.md
./markup README.md -o README.html
./markup --standalone --title "My document" README.md -o document.html
printf '# Hello\n' | ./markup --format markdown -
```

By default, `markup` writes an HTML fragment to standard output. `-o` writes a
file atomically, and `--standalone` adds a minimal HTML document wrapper.
Existing regular-file permissions are preserved; output symlinks and attempts
to overwrite the input are refused.

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

Implemented now:

- ATX and Setext headings;
- paragraphs, soft breaks and hard breaks;
- emphasis, strong emphasis, strikethrough and code spans;
- inline links, images, URL/email autolinks and entities;
- unordered, ordered and task lists;
- block quotes;
- fenced and indented code blocks;
- thematic breaks;
- pipe tables with alignment;
- raw HTML pass-through by default;
- `--safe` mode, which escapes raw HTML and rejects active URL schemes;
- CRLF and unterminated-fence handling.

This checkpoint is intentionally **not advertised as CommonMark compliant**.
The parser is useful for ordinary project documentation, but nested/mixed list
structures, reference links, footnotes, full delimiter-run semantics, complete
HTML block rules and CommonMark's complete conformance corpus remain roadmap
work. Compatibility claims must grow from executable evidence rather than the
name “Markdown”.

## Tests

```sh
make test          # semantic, adversarial, CLI and 10k mutation gates
make test-sanitize # ASan + UBSan semantic/adversarial gates
# Where LeakSanitizer cannot inspect /proc under the runner:
make test-sanitize ASAN_OPTIONS=detect_leaks=0:halt_on_error=1
```

See [HANDOVER.md](HANDOVER.md) for the living development contract and
`docs/handover/` for architecture, decisions, testing and roadmap detail.
