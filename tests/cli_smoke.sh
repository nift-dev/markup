#!/usr/bin/env bash
set -euo pipefail

bin=${MARKUP_BIN:-./markup}
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

printf '# Markup++\n\nConvert **Markdown** safely.\n' >"$tmp/input.md"
"$bin" "$tmp/input.md" >"$tmp/stdout.html"
grep -Fx '<h1>Markup++</h1>' "$tmp/stdout.html" >/dev/null
grep -Fx '<p>Convert <strong>Markdown</strong> safely.</p>' "$tmp/stdout.html" >/dev/null

"$bin" --standalone --title 'CLI test' "$tmp/input.md" -o "$tmp/output.html"
grep -Fx '<title>CLI test</title>' "$tmp/output.html" >/dev/null
printf '# stdin\n' | "$bin" --format md - >"$tmp/stdin.html"
grep -Fx '<h1>stdin</h1>' "$tmp/stdin.html" >/dev/null
printf '~~core~~\n' | "$bin" --format md - | grep -Fx '<p>~~core~~</p>' >/dev/null
printf '~~extension~~\n' | "$bin" --extensions --format md - | grep -Fx '<p><del>extension</del></p>' >/dev/null
printf '~~core~~\n' | "$bin" --commonmark --format md - | grep -Fx '<p>~~core~~</p>' >/dev/null

if printf '# missing format\n' | "$bin" - >"$tmp/bad" 2>"$tmp/error"; then exit 1; fi
grep -F 'stdin requires --format' "$tmp/error" >/dev/null
if "$bin" "$tmp/input.md" -o "$tmp/input.md" 2>"$tmp/error"; then exit 1; fi
grep -F 'refusing to overwrite' "$tmp/error" >/dev/null

printf 'keep\n' >"$tmp/existing.html"
chmod 640 "$tmp/existing.html"
"$bin" "$tmp/input.md" -o "$tmp/existing.html"
mode=$(stat -c '%a' "$tmp/existing.html" 2>/dev/null || stat -f '%Lp' "$tmp/existing.html")
test "$mode" = 640

if ln -s target "$tmp/link.html" 2>/dev/null; then
    if "$bin" "$tmp/input.md" -o "$tmp/link.html" 2>"$tmp/error"; then exit 1; fi
    grep -F 'refusing to replace a symbolic link' "$tmp/error" >/dev/null
fi

printf 'CLI paragraph\n' | "$bin" --format asciidoc - | grep -F '<p>CLI paragraph</p>' >/dev/null
"$bin" --version | grep -Fx 'Markup++ 0.1.0' >/dev/null
"$bin" --help | grep -F 'convert markup formats to HTML' >/dev/null
echo 'CLI smoke checks passed'
