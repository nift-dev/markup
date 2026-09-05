#!/usr/bin/env python3
"""Validate the immutable AC9 compatibility profile and manifests offline."""
import json, pathlib, sys

root=pathlib.Path("tests/fixtures/asciidoctor-2.0.26")
profile=json.loads((root/"PROFILE.json").read_text())
corpus=json.loads((root/"corpus.json").read_text())
differences=json.loads((root/"differences.json").read_text())
unsupported=json.loads((root/"unsupported.json").read_text())
ids=[case["id"] for case in corpus]
if profile["version"] != "2.0.26" or len(profile["sha256"]) != 64:
    print("invalid pinned Asciidoctor profile", file=sys.stderr); raise SystemExit(1)
if len(ids) != len(set(ids)) or set(ids) != set(differences):
    print("stale or duplicate compatibility manifest", file=sys.stderr); raise SystemExit(1)
if not unsupported:
    print("unsupported boundary is empty", file=sys.stderr); raise SystemExit(1)
print(f"AC9 profile gate: Asciidoctor {profile['version']}, {len(corpus)} corpus cases, exact difference manifest")
print("Cross-platform execution is required before publishing the compatibility claim")
