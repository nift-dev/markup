#!/usr/bin/env bash
set -euo pipefail

program=${1:-./markup}
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

for run in 1 2 3; do
    python3 tests/commonmark_runner.py --program "$program" >"$tmp/run-$run"
done
cmp "$tmp/run-1" "$tmp/run-2"
cmp "$tmp/run-1" "$tmp/run-3"
grep -Fx 'CommonMark 0.31.2: 652/652 examples passed' "$tmp/run-1" >/dev/null
echo '3 reproducible CommonMark corpus runs passed'
