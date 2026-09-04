#!/usr/bin/env python3
"""Bound pathological CommonMark inputs without flaky microbenchmarks."""

import argparse
import subprocess
import sys


def run(program: str, name: str, source: str) -> None:
    try:
        result = subprocess.run(
            [program, "--commonmark", "--format", "markdown", "-"],
            input=source, text=True, capture_output=True, timeout=5, check=False)
    except subprocess.TimeoutExpired:
        print(f"FAIL {name}: exceeded 5 second guard", file=sys.stderr)
        raise SystemExit(1)
    if result.returncode != 0:
        print(f"FAIL {name}: exit {result.returncode}: {result.stderr}", file=sys.stderr)
        raise SystemExit(1)
    if len(result.stdout) > len(source) * 8 + 1024:
        print(f"FAIL {name}: unexpectedly amplified output", file=sys.stderr)
        raise SystemExit(1)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--program", required=True)
    parser.add_argument("--family", choices=("emphasis", "links", "all"), default="all")
    args = parser.parse_args()

    cases = []
    if args.family in ("emphasis", "all"):
        cases.extend([
            ("unmatched stars", ("*a " * 30000) + "\n"),
            ("unmatched underscores", ("_a " * 30000) + "\n"),
            ("alternating runs", ("***a___b " * 12000) + "\n"),
            ("many closers", ("a* " * 30000) + "\n"),
        ])
    if args.family in ("links", "all"):
        cases.extend([
            ("unmatched brackets", ("[a" * 30000) + "\n"),
            ("nested brackets", ("[" * 20000) + "x" + ("]" * 20000) + "\n"),
            ("reference labels", ("[x]: /url\n" * 10000) + "[x]\n"),
        ])

    for name, source in cases:
        run(args.program, name, source)
    print(f"{len(cases)} CommonMark pathological {args.family} cases passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
