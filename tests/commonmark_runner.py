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
    parser.add_argument("--expected-failures", default="tests/fixtures/commonmark-expected-failures.json")
    parser.add_argument("--section", action="append", default=[])
    parser.add_argument("--example", action="append", type=int, default=[])
    parser.add_argument("--allow-failures", action="store_true")
    parser.add_argument("--show-failures", type=int, default=10)
    args = parser.parse_args()

    cases = json.loads(pathlib.Path(args.spec).read_text(encoding="utf-8"))
    expected_data = json.loads(pathlib.Path(args.expected_failures).read_text(encoding="utf-8"))
    expected_failures = {entry["example"] for entry in expected_data}
    known_examples = {case["example"] for case in cases}
    unknown = expected_failures - known_examples
    if unknown:
        print(f"commonmark: expected-failure manifest has unknown examples: {sorted(unknown)}", file=sys.stderr)
        return 2
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

    actual_failures = {case["example"] for case, _ in failures}
    selected_numbers = {case["example"] for case in selected}
    stale_expected = (expected_failures & selected_numbers) - actual_failures
    unexpected = actual_failures - expected_failures
    if stale_expected:
        print(f"commonmark: expected failures now pass: {sorted(stale_expected)}", file=sys.stderr)

    for case, result in failures[:args.show_failures]:
        print(f"\nExample {case['example']} - {case['section']}", file=sys.stderr)
        if result.stderr:
            print(result.stderr.rstrip(), file=sys.stderr)
        diff = difflib.unified_diff(
            case["html"].splitlines(keepends=True),
            result.stdout.splitlines(keepends=True),
            fromfile="expected", tofile="actual")
        sys.stderr.writelines(diff)

    if stale_expected:
        return 1
    return 0 if not unexpected or args.allow_failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
