#!/usr/bin/env python3
"""Validate the immutable RST14 Docutils 0.23 profile and CLI determinism."""
import argparse
import json
import pathlib
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument("--program", default="./markup")
args = parser.parse_args()
root = pathlib.Path("tests/fixtures/docutils-0.23")
profile = json.loads((root / "PROFILE.json").read_text())
corpus = json.loads((root / "corpus.json").read_text())
reference = json.loads((root / "reference.json").read_text())
ids = [case["id"] for case in corpus]
if profile["version"] != "0.23" or len(profile["sha256"]) != 64:
    raise SystemExit("invalid Docutils profile")
reference_ids = {case["id"] for case in reference}
if len(ids) != len(set(ids)) or set(ids) != reference_ids:
    raise SystemExit("stale, missing or duplicate Docutils evidence")
failures = []
for case in corpus:
    runs = [subprocess.run([args.program, "--format", "rst", "-"],
                           input=case["input"], text=True, capture_output=True)
            for _ in range(3)]
    if any(run.returncode for run in runs):
        failures.append(f"{case['id']}: CLI failure")
    elif len({run.stdout for run in runs}) != 1:
        failures.append(f"{case['id']}: non-deterministic HTML")
if failures:
    print("\n".join(failures), file=sys.stderr)
    raise SystemExit(1)
print(f"RST14 local gate: Docutils {profile['version']}, {len(corpus)} frozen oracle cases, deterministic CLI")
print("Cross-platform execution is required before publishing the compatibility claim")
