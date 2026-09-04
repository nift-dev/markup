#!/usr/bin/env python3
"""Compare Markup++ core output with the pinned cmark boundary."""

import argparse
import json
import random
import subprocess
import sys


def convert(command, source):
    result = subprocess.run(command, input=source, text=True, capture_output=True,
                            timeout=5, check=False)
    if result.returncode:
        raise RuntimeError(f"{command[0]} exited {result.returncode}: {result.stderr}")
    return result.stdout


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--program", required=True)
    parser.add_argument("--reference", required=True)
    parser.add_argument("--random-cases", type=int, default=1000)
    args = parser.parse_args()

    spec = json.load(open("tests/fixtures/commonmark-0.31.2.json", encoding="utf-8"))
    sources = [case["markdown"] for case in spec]
    rng = random.Random(0x4D41524B)
    alphabet = "abc XYZ012\n\t*_`[]()<>!\\&;:.-+'\"/=é中"
    for _ in range(args.random_cases):
        sources.append("".join(rng.choice(alphabet) for _ in range(rng.randrange(512))))

    markup_command = [args.program, "--format", "markdown", "-"]
    reference_command = [args.reference]
    for number, source in enumerate(sources, 1):
        actual = convert(markup_command, source)
        expected = convert(reference_command, source)
        if actual != expected:
            print(f"differential mismatch at case {number}", file=sys.stderr)
            return 1
    print(f"{len(sources)} pinned-reference differential cases passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
