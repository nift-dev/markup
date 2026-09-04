#!/usr/bin/env python3
"""Run Markup++ against the pinned CommonMark examples."""

import argparse
import difflib
import json
import pathlib
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--program", required=True)
    parser.add_argument("--spec", default="tests/fixtures/commonmark-0.31.2.json")
    parser.add_argument("--section", action="append", default=[])
    parser.add_argument("--example", action="append", type=int, default=[])
    parser.add_argument("--allow-failures", action="store_true")
    parser.add_argument("--show-failures", type=int, default=10)
    args = parser.parse_args()

    cases = json.loads(pathlib.Path(args.spec).read_text(encoding="utf-8"))
    sections = set(args.section)
    examples = set(args.example)
    selected = [case for case in cases
                if (not sections or case["section"] in sections)
                and (not examples or case["example"] in examples)]
    if not selected:
        print("commonmark: no examples selected", file=sys.stderr)
        return 2

    failures = []
    section_counts = {}
    for case in selected:
        result = subprocess.run(
            [args.program, "--commonmark", "--format", "markdown", "-"],
            input=case["markdown"], text=True, capture_output=True, check=False)
        passed = result.returncode == 0 and result.stdout == case["html"]
        counts = section_counts.setdefault(case["section"], [0, 0])
        counts[1] += 1
        if passed:
            counts[0] += 1
        else:
            failures.append((case, result))

    passed = len(selected) - len(failures)
    for section, (section_passed, total) in section_counts.items():
        print(f"{section}: {section_passed}/{total}")
    print(f"CommonMark 0.31.2: {passed}/{len(selected)} examples passed")

    for case, result in failures[:args.show_failures]:
        print(f"\nExample {case['example']} - {case['section']}", file=sys.stderr)
        if result.stderr:
            print(result.stderr.rstrip(), file=sys.stderr)
        diff = difflib.unified_diff(
            case["html"].splitlines(keepends=True),
            result.stdout.splitlines(keepends=True),
            fromfile="expected", tofile="actual")
        sys.stderr.writelines(diff)

    return 0 if not failures or args.allow_failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
