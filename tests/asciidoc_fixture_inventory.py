#!/usr/bin/env python3
"""Validate the pinned AsciiDoc TCK input inventory."""

import json
import pathlib
import sys


def main() -> int:
    path = pathlib.Path("tests/fixtures/asciidoc-tck/cases.json")
    cases = json.loads(path.read_text(encoding="utf-8"))
    required = {"path", "type", "checkpoint", "input"}
    if len(cases) != 13:
        print(f"expected 13 pinned inputs, found {len(cases)}", file=sys.stderr)
        return 1
    paths = set()
    for case in cases:
        if set(case) != required or case["type"] not in {"block", "inline"}:
            print(f"invalid case entry: {case!r}", file=sys.stderr)
            return 1
        if case["path"] in paths or not case["input"].endswith("\n"):
            print(f"duplicate or unterminated input: {case['path']}", file=sys.stderr)
            return 1
        paths.add(case["path"])
    print(f"{len(cases)} pinned AsciiDoc TCK inputs inventoried")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
