#!/usr/bin/env python3
"""Generate C++ fixtures for the in-process CommonMark conformance test."""

import argparse
import json
import pathlib


def byte_literal(value: str) -> str:
    """Emit execution-encoding-independent UTF-8 bytes as short C++ literals."""
    data = value.encode("utf-8")
    if not data:
        return '""'
    return "".join(
        '"' + "".join(f"\\{byte:03o}" for byte in data[offset:offset + 48]) + '"'
        for offset in range(0, len(data), 48)
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--spec", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    cases = json.loads(pathlib.Path(args.spec).read_text(encoding="utf-8"))
    output = pathlib.Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)

    lines = [
        "// Generated from the pinned CommonMark fixture. Do not edit.",
        "static const CommonMarkCase commonmark_cases[] = {",
    ]
    for case in cases:
        lines.append(
            "    {%d, %s, %s, %s},"
            % (
                case["example"],
                byte_literal(case["section"]),
                byte_literal(case["markdown"]),
                byte_literal(case["html"]),
            )
        )
    lines.append("};")
    output.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
