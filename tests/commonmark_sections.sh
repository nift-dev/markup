#!/usr/bin/env bash
set -euo pipefail

checkpoint=${1:?checkpoint required}
program=${2:-./markup}
runner=(python3 tests/commonmark_runner.py --program "$program" --show-failures 20)

case "$checkpoint" in
    cm2)
        sections=(
            "Tabs" "Backslash escapes" "Precedence" "Thematic breaks"
            "ATX headings" "Setext headings" "Indented code blocks"
            "Fenced code blocks" "Paragraphs" "Blank lines"
        )
        ;;
    cm3)
        sections=("HTML blocks" "Raw HTML")
        ;;
    cm4)
        sections=("Block quotes" "List items" "Lists")
        ;;
    cm5)
        sections=(
            "Backslash escapes" "Entity and numeric character references"
            "Code spans" "Autolinks" "Raw HTML" "Hard line breaks"
            "Soft line breaks"
        )
        ;;
    cm6)
        sections=("Emphasis and strong emphasis")
        ;;
    *)
        echo "commonmark sections: unknown checkpoint '$checkpoint'" >&2
        exit 2
        ;;
esac

for section in "${sections[@]}"; do
    runner+=(--section "$section")
done
"${runner[@]}"
