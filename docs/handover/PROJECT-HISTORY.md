# Project history

Markup++ began in September 2026 as a planned Markdown, AsciiDoc and
reStructuredText to HTML converter for the Gantry ecosystem. Its first intended
consumers are a standalone `markup` CLI and a future embedded Nift integration.

Minify++ supplied the architectural precedent: a canonical standalone C++17
library with a thin CLI, mirrored into Nift behind a narrow public API. Jsonic++
reinforced the value of a small self-contained public boundary, living handover
documents and explicit synchronization ownership.

Development checkpoint 0.1.0 established that shape and implemented the first
Markdown subset. The project deliberately began with honest partial-support
language rather than declaring CommonMark compatibility before running the
official corpus.

## CommonMark conformance program

CM0 established a pinned CommonMark 0.31.2 harness and measured the original
converter at 198/652. CM1 embedded cmark 0.31.1 source behind the Markup++ API;
CM2-CM7 closed individual block, HTML, container, inline, emphasis and link
gates; CM8 made CommonMark the default API-v2 profile; and CM9 made 652/652
reproducible with an empty expected-failure manifest and differential boundary
tests. CM10 added local performance, RSS, pathological, sanitizer and fuzz
infrastructure plus cross-platform CI. Actions run `33933214625` passed at
commit `3cba857` on 2026-09-05, closing the versioned CommonMark 0.31.2 release
gate; the corrected MSVC job completed in 1m 4s instead of timing out.
