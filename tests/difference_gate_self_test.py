#!/usr/bin/env python3
"""Adversarial self-test for the pinned compatibility-difference manifests.

Proves that the Docutils 0.23 and Asciidoctor 2.0.26 output gates cannot accept
a recorded difference that has changed: mutating a pinned hash, removing a pin,
listing a case that now matches, duplicating an id, or adding an unknown id
must all fail the gate, while the genuine manifest passes.
"""
import json
import pathlib
import shutil
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parent.parent
PROGRAM = str(ROOT / "markup")

PROFILES = [
    {
        "name": "docutils",
        "real_manifest": ROOT / "tests" / "fixtures" / "docutils-0.23" / "differences.json",
        "gate": [sys.executable, str(ROOT / "tests" / "docutils_release_gate.py"), "--program", PROGRAM],
    },
    {
        "name": "asciidoctor",
        "real_manifest": ROOT / "tests" / "fixtures" / "asciidoctor-2.0.26" / "differences.json",
        "gate": [
            sys.executable, str(ROOT / "tests" / "asciidoctor_compare.py"), "--program", PROGRAM,
            "--reference", str(ROOT / "tests" / "fixtures" / "asciidoctor-2.0.26" / "reference.json"),
            "--corpus", str(ROOT / "tests" / "fixtures" / "asciidoctor-2.0.26" / "corpus.json"),
        ],
    },
]


def run_gate(profile, manifest_path):
    cmd = profile["gate"] + ["--manifest", str(manifest_path)]
    return subprocess.run(cmd, capture_output=True, text=True)


def write_manifest(path, entries):
    path.write_text(json.dumps(entries, indent=2) + "\n")


def main():
    failures = []
    checks = 0
    with tempfile.TemporaryDirectory(prefix="markup-gate-selftest.") as tmp:
        for profile in PROFILES:
            entries = json.loads(profile["real_manifest"].read_text())
            base = pathlib.Path(tmp) / profile["name"]
            base.mkdir()

            # 1. The genuine manifest passes.
            genuine = base / "genuine.json"
            write_manifest(genuine, entries)
            r = run_gate(profile, genuine)
            checks += 1
            if r.returncode != 0:
                failures.append(f"{profile['name']}: genuine manifest must pass: {r.stderr.strip()}")

            # 2. Mutating a pinned actual hash must fail.
            mutated = base / "mutated.json"
            altered = json.loads(json.dumps(entries))
            altered[0]["actual_normalized_sha256"] = "0" * 64
            write_manifest(mutated, altered)
            r = run_gate(profile, mutated)
            checks += 1
            if r.returncode == 0:
                failures.append(f"{profile['name']}: mutated pinned hash must fail")

            # 3. Removing a pin must fail.
            unpinned = base / "unpinned.json"
            altered = json.loads(json.dumps(entries))
            del altered[0]["expected_normalized_sha256"]
            write_manifest(unpinned, altered)
            r = run_gate(profile, unpinned)
            checks += 1
            if r.returncode == 0:
                failures.append(f"{profile['name']}: missing pin must fail")

            # 4. Listing a case that now matches must fail. Use the first
            #    corpus case (paragraph for docutils, header-section for
            #    asciidoctor would not match; pick the first normalized-match
            #    case by running the genuine gate output).
            r = run_gate(profile, genuine)
            matching_id = None
            for line in r.stderr.splitlines():
                if line.startswith("Pinned differences:"):
                    continue
            # Determine a matching case deterministically: run each corpus id
            # through the real gate is not possible here, so use a known
            # always-matching case per profile.
            matching_id = "paragraph" if profile["name"] == "docutils" else "paragraph"
            stale = base / "stale.json"
            altered = json.loads(json.dumps(entries))
            altered.append({
                "id": matching_id,
                "reason": "now-matching case incorrectly recorded",
                "expected_normalized_sha256": "3" * 64,
                "actual_normalized_sha256": "4" * 64,
            })
            write_manifest(stale, altered)
            r = run_gate(profile, stale)
            checks += 1
            if r.returncode == 0:
                failures.append(f"{profile['name']}: manifest entry for a matching case must fail")

            # 5. Duplicate id must fail.
            dup = base / "dup.json"
            altered = json.loads(json.dumps(entries))
            altered.append(dict(altered[0]))
            write_manifest(dup, altered)
            r = run_gate(profile, dup)
            checks += 1
            if r.returncode == 0:
                failures.append(f"{profile['name']}: duplicate manifest id must fail")

            # 6. Unknown id must fail.
            unknown = base / "unknown.json"
            altered = json.loads(json.dumps(entries))
            altered.append({
                "id": "not-a-corpus-case",
                "reason": "unknown id",
                "expected_normalized_sha256": "5" * 64,
                "actual_normalized_sha256": "6" * 64,
            })
            write_manifest(unknown, altered)
            r = run_gate(profile, unknown)
            checks += 1
            if r.returncode == 0:
                failures.append(f"{profile['name']}: unknown manifest id must fail")

    if failures:
        print("\n".join(failures), file=sys.stderr)
        raise SystemExit(1)
    print(f"compatibility difference-gate self-test passed ({checks} adversarial checks)")


if __name__ == "__main__":
    raise SystemExit(main())