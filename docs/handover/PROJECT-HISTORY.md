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

## AsciiDoc language program

AD0 pinned the unreleased Eclipse language and TCK development snapshots and
inventoried all 13 current upstream inputs. AD1 established a bounded,
source-positioned neutral model and enabled public conversion. AD2 implemented
headers, attributes and nested sections; AD3 established block precedence and
structural/verbatim containers; AD4 added list families and nesting; AD5 added
ordered inline substitutions. These checkpoints establish useful conversion,
AD6 added references/media, AD7 tables, AD8 macros/conditionals, AD9 explicit
include capabilities, AD10 safe rendering and robustness, and AD11 the pinned
alpha release gate. The development profile is complete; stable conformance
still awaits a released standard/TCK and its expected-ASG oracle.
