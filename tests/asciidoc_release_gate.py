#!/usr/bin/env python3
"""Run the reproducible AD11 pinned-development-snapshot gate."""

import argparse
import hashlib
import json
import pathlib
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--program", default="./markup")
    args = parser.parse_args()
    cases = json.loads(pathlib.Path("tests/fixtures/asciidoc-tck/cases.json").read_text())
    failures = []
    for case in cases:
        outputs = []
        for _ in range(3):
            run = subprocess.run(
                [args.program, "--format", "asciidoc", "-"],
                input=case["input"], text=True, capture_output=True)
            if run.returncode or run.stderr:
                failures.append(f"{case['path']}: exit={run.returncode}: {run.stderr.strip()}")
                break
            outputs.append(run.stdout)
        if len(outputs) != 3:
            continue
        digest = hashlib.sha256(outputs[0].encode()).hexdigest()
        if outputs[1:] != outputs[:1] * 2:
            failures.append(f"{case['path']}: non-deterministic output")
        elif digest != case["html_sha256"]:
            failures.append(f"{case['path']}: HTML digest changed: {digest}")
    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print(f"AD11: {len(cases)}/{len(cases)} pinned alpha TCK inputs parse and render reproducibly")
    print("Oracle scope: pinned input inventory + reviewed Markup++ HTML digests; upstream expected ASG unavailable")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
