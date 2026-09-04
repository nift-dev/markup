# CommonMark 0.31.2 baseline

CM0 measured the original Markup++ Markdown implementation against all 652
official examples in the strict extensions-off profile. It passed **198/652**.

| Section | Passed | Total |
|---|---:|---:|
| Tabs | 0 | 11 |
| Backslash escapes | 4 | 13 |
| Entity and numeric character references | 4 | 17 |
| Precedence | 1 | 1 |
| Thematic breaks | 4 | 19 |
| ATX headings | 12 | 18 |
| Setext headings | 10 | 27 |
| Indented code blocks | 0 | 12 |
| Fenced code blocks | 7 | 29 |
| HTML blocks | 7 | 44 |
| Link reference definitions | 4 | 27 |
| Paragraphs | 3 | 8 |
| Blank lines | 1 | 1 |
| Block quotes | 14 | 25 |
| List items | 10 | 48 |
| Lists | 4 | 26 |
| Inlines | 1 | 1 |
| Code spans | 13 | 22 |
| Emphasis and strong emphasis | 52 | 132 |
| Links | 21 | 90 |
| Images | 0 | 22 |
| Autolinks | 8 | 19 |
| Raw HTML | 7 | 20 |
| Hard line breaks | 7 | 15 |
| Soft line breaks | 1 | 2 |
| Textual content | 3 | 3 |

This is a measurement, not a compatibility claim. Reproduce it with:

```sh
make commonmark-report
```
