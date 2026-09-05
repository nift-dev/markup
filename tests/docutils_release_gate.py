#!/usr/bin/env python3
"""Validate the RST14 Docutils 0.23 profile by comparing normalized output.

The gate runs every frozen corpus case through the Markup++ CLI and compares
the normalized HTML fragment against the pinned Docutils 0.23 HTML5-writer
reference output. Normalization removes the documented writer-decoration
differences only (attributes, wrapper tags, `<p>` inside list items/table
cells, heading-level offsets caused by Docutils document-title promotion).
Structural differences that are reviewed and intentional are recorded in
`differences.json`; any other unexpected difference fails the gate.
"""
import argparse
import json
import pathlib
import re
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument("--program", default="./markup")
args = parser.parse_args()
root = pathlib.Path("tests/fixtures/docutils-0.23")
profile = json.loads((root / "PROFILE.json").read_text())
corpus = json.loads((root / "corpus.json").read_text())
reference = json.loads((root / "reference.json").read_text())
manifest = json.loads((root / "differences.json").read_text())
ids = [case["id"] for case in corpus]
if profile["version"] != "0.23" or len(profile["sha256"]) != 64:
    raise SystemExit("invalid Docutils profile")
reference_ids = {case["id"] for case in reference}
if len(ids) != len(set(ids)) or set(ids) != reference_ids:
    raise SystemExit("stale, missing or duplicate Docutils evidence")
manifest_ids = {case["id"] for case in manifest}
if len(manifest) != len(manifest_ids):
    raise SystemExit("differences.json has duplicate ids")
unknown_manifest = manifest_ids - set(ids)
if unknown_manifest:
    raise SystemExit(f"differences.json lists unknown case ids: {sorted(unknown_manifest)}")
if not all(case.get("reason") for case in manifest):
    raise SystemExit("every differences.json entry needs a reason")


VOID_TAGS = {"br", "hr", "img"}
CONTAINER_TAGS = {"li", "td", "th", "dd", "dt"}


def normalize_fragment(html):
    """Canonical structural form that removes documented writer decoration."""
    out = []
    section_depth = 0
    container_depth = 0
    literal_spans = 0
    token_re = re.compile(r"<!--.*?-->|<[^>]+>|[^<]+", re.S)
    for m in token_re.finditer(html):
        tok = m.group(0)
        if tok.startswith("<!--"):
            body = tok[4:-3].strip()
            out.append("{cmt " + body + "}")
            continue
        if not tok.startswith("<"):
            if tok.strip():
                out.append(tok.rstrip("\n"))
            continue
        closing = tok.startswith("</")
        inner = tok[2 if closing else 1:-1].strip()
        self_closing = inner.endswith("/")
        if self_closing:
            inner = inner[:-1].strip()
        name = inner.split()[0].lower() if inner else ""
        name = name.rstrip("/")
        if closing:
            if name == "p" and container_depth > 0:
                continue
            if name == "section":
                section_depth = max(0, section_depth - 1)
                out.append("</section>")
                continue
            if name in ("thead", "tbody"):
                continue
            if name == "th":
                out.append("</td>")
                continue
            if name.startswith("h") and len(name) == 2 and name[1].isdigit():
                out.append("</h%d>" % section_depth)
                continue
            if name == "code":
                out.append("{/lit}")
                continue
            if name in CONTAINER_TAGS:
                container_depth = max(0, container_depth - 1)
                out.append("</" + name + ">")
                continue
            if name == "span" and literal_spans > 0:
                literal_spans -= 1
                out.append("{/lit}")
                continue
            out.append("</" + name + ">")
            continue
        if name == "section":
            section_depth += 1
            out.append("<section>")
            continue
        if name in ("thead", "tbody"):
            continue
        if name == "p" and container_depth > 0:
            continue
        if name == "code":
            out.append("{lit}")
            continue
        if name == "span" and "class" in inner and "docutils" in inner and "literal" in inner:
            literal_spans += 1
            out.append("{lit}")
            continue
        if name.startswith("h") and len(name) == 2 and name[1].isdigit():
            out.append("h%d" % section_depth)
            continue
        if name == "th":
            if name in CONTAINER_TAGS:
                container_depth += 1
            out.append("<td>")
            continue
        attrs = ""
        for attr in ("href", "src"):
            am = re.search(attr + r'\s*=\s*"([^"]*)"', inner)
            if am:
                attrs += "@" + attr + '="' + am.group(1) + '"'
        if name in CONTAINER_TAGS:
            container_depth += 1
        out.append("<" + name + attrs + ">")
    result = "".join(out)
    return result.rstrip()


reference_map = {case["id"]: case for case in reference}
manifest_map = {case["id"]: case for case in manifest}
failures = []
accepted = []
passed = 0
for case in corpus:
    runs = [subprocess.run([args.program, "--format", "rst", "-"],
                           input=case["input"], text=True, capture_output=True)
            for _ in range(3)]
    if any(run.returncode for run in runs):
        failures.append(f"{case['id']}: CLI failure")
        continue
    if len({run.stdout for run in runs}) != 1:
        failures.append(f"{case['id']}: non-deterministic HTML")
        continue
    actual = normalize_fragment(runs[0].stdout)
    expected = normalize_fragment(reference_map[case["id"]]["fragment"])
    if actual == expected:
        passed += 1
        continue
    if case["id"] in manifest_map:
        accepted.append(case["id"])
        continue
    failures.append(
        f"{case['id']}: normalized output differs from the Docutils 0.23 reference"
    )

if failures:
    print("\n".join(failures), file=sys.stderr)
    raise SystemExit(1)
print(
    f"RST14 output gate: Docutils {profile['version']}, {len(corpus)} corpus cases, "
    f"{passed} normalized matches, {len(accepted)} recorded differences"
)
if accepted:
    print("Recorded differences:", ", ".join(accepted), file=sys.stderr)
print("Cross-platform execution is required before publishing the compatibility claim")