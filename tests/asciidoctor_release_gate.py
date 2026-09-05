#!/usr/bin/env python3
"""Validate the immutable AC9 compatibility profile, manifests and output."""
import argparse, json, pathlib, subprocess, sys

ap = argparse.ArgumentParser()
ap.add_argument("--program", default="./markup")
ap.add_argument("--manifest", default="tests/fixtures/asciidoctor-2.0.26/differences.json")
args = ap.parse_args()

root = pathlib.Path("tests/fixtures/asciidoctor-2.0.26")
profile = json.loads((root / "PROFILE.json").read_text())
corpus = json.loads((root / "corpus.json").read_text())
differences = json.loads((root / "differences.json").read_text())
unsupported = json.loads((root / "unsupported.json").read_text())
ids = [case["id"] for case in corpus]
if profile["version"] != "2.0.26" or len(profile["sha256"]) != 64:
    print("invalid pinned Asciidoctor profile", file=sys.stderr); raise SystemExit(1)
if len(ids) != len(set(ids)):
    print("duplicate corpus ids", file=sys.stderr); raise SystemExit(1)
if not {case["id"] for case in differences} <= set(ids):
    print("differences.json lists unknown case ids", file=sys.stderr); raise SystemExit(1)
if not unsupported:
    print("unsupported boundary is empty", file=sys.stderr); raise SystemExit(1)
if not all(case.get("reason") for case in differences):
    print("every differences.json entry needs a reason", file=sys.stderr); raise SystemExit(1)

# Structural output comparison against the pinned Asciidoctor 2.0.26 reference.
compare = subprocess.run(
    [sys.executable, str(pathlib.Path(__file__).with_name("asciidoctor_compare.py")),
     "--program", args.program, "--manifest", args.manifest],
    capture_output=True, text=True,
)
print(compare.stdout, end="")
if compare.stderr:
    print(compare.stderr, end="", file=sys.stderr)
if compare.returncode == 2:
    print("AC9 output comparison could not run: pinned reference absent", file=sys.stderr)
    raise SystemExit(1)
if compare.returncode != 0:
    print("AC9 output comparison failed", file=sys.stderr)
    raise SystemExit(1)
print("Cross-platform execution is required before publishing the compatibility claim")