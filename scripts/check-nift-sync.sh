#!/usr/bin/env bash
set -euo pipefail

NIFT_DIR=${1:-${NIFT_DIR:-}}
if [ -z "$NIFT_DIR" ]; then
    echo "usage: $0 /path/to/nift" >&2
    exit 2
fi

SOURCE_ROOT=$(cd "$(dirname "$0")/.." && pwd)
EMBED_ROOT="$NIFT_DIR/markuppp"
FILES=(LICENSE include/markup/Markup.h src/Markup.cpp src/AsciiDoc.cpp src/AsciiDoc.h src/ReStructuredText.cpp src/ReStructuredText.h)

while IFS= read -r file; do
    FILES+=("${file#"$SOURCE_ROOT/"}")
done < <(find "$SOURCE_ROOT/vendor/cmark" -type f | sort)

for file in "${FILES[@]}"; do
    if [ ! -f "$EMBED_ROOT/$file" ]; then
        echo "Markup++ sync failure: missing Nift file markuppp/$file" >&2
        exit 1
    fi
    if ! cmp -s "$SOURCE_ROOT/$file" "$EMBED_ROOT/$file"; then
        echo "Markup++ sync failure: markuppp/$file differs from standalone $file" >&2
        exit 1
    fi
done

echo "Markup++ standalone/Nift sync passed (${#FILES[@]} files)"
