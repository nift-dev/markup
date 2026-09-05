#!/usr/bin/env python3
"""Strict structural output comparison for the pinned Asciidoctor profile.

Compares the Markup++ Asciidoctor fragment against the pinned Asciidoctor 2.0.26
HTML5 reference output after removing documented writer decoration:

- wrapper divs (`header`, `sectN`, `sectionbody`, `ulist`, `olist`, `dlist`,
  `listingblock`, `content`, `source`, `admonitionblock`, ...);
- `class`/`style`/`role` attributes and auto-generated `id="_..."` attributes;
- `<thead>`/`<tbody>`/`<colgroup>`/`<col>` table structure;
- `<p>` wrappers inside list items and table cells;
- the `<code>` wrapper Asciidoctor adds inside `<pre>`.

Any case whose normalized structure still differs from the reference and is not
recorded in `differences.json` fails the gate.
"""
import argparse
import hashlib
import html.parser
import json
import pathlib
import re
import subprocess
import sys

WRAPPER_DIVS = {
    "sect1", "sect2", "sect3", "sect4", "sect5", "sectionbody",
    "ulist", "olist", "dlist", "listingblock", "content", "source",
    "exampleblock", "sidebarblock", "openblock", "admonitionblock",
    "quoteblock", "verseblock", "imageblock", "videoblock", "audioblock",
}
CONTAINER_TAGS = {"li", "td", "th", "dt", "dd"}


class Shape(html.parser.HTMLParser):
    def __init__(self):
        super().__init__()
        self.events = []

    def handle_starttag(self, tag, attrs):
        semantic = tuple(
            sorted((k, v or "") for k, v in attrs if k in {"id", "href", "src", "colspan"})
        )
        self.events.append(("start", tag, semantic))

    def handle_endtag(self, tag):
        self.events.append(("end", tag))

    def handle_data(self, data):
        text = " ".join(data.split())
        if text:
            self.events.append(("text", text))


def shape(value):
    parser = Shape()
    parser.feed(value)
    parser.close()
    return parser.events


def normalize_fragment(html):
    """Remove documented Asciidoctor writer decoration, keep structure."""
    out = []
    div_stack = []  # entries: None (keep), 'wrapper' (drop tag pair), 'header' (drop subtree)
    container_depth = 0
    pre_depth = 0
    pending_id = ""
    token_re = re.compile(r"<!--.*?-->|<[^>]+>|[^<]+", re.S)
    for m in token_re.finditer(html):
        tok = m.group(0)
        if tok.startswith("<!--"):
            continue
        if not tok.startswith("<"):
            if div_stack and div_stack[-1] == "header":
                continue
            if tok.strip():
                out.append(tok.rstrip("\n"))
            continue
        closing = tok.startswith("</")
        inner = tok[2 if closing else 1:-1].strip()
        self_closing = inner.endswith("/")
        if self_closing:
            inner = inner[:-1].strip()
        name = inner.split()[0].lower() if inner else ""
        if closing and name == "div" and div_stack and div_stack[-1] == "header":
            div_stack.pop()
            continue
        if div_stack and div_stack[-1] == "header":
            continue
        if closing:
            if name == "div":
                if div_stack and div_stack[-1] == "header":
                    div_stack.pop()
                    continue
                if div_stack and div_stack[-1] == "wrapper":
                    div_stack.pop()
                    continue
                if div_stack:
                    div_stack.pop()
                    out.append("</div>")
                continue
            if name == "p" and container_depth > 0:
                continue
            if name in ("thead", "tbody", "colgroup"):
                continue
            if name == "col":
                continue
            if name == "code":
                if pre_depth > 0:
                    continue
                out.append("{/code}")
                continue
            if name in CONTAINER_TAGS:
                container_depth = max(0, container_depth - 1)
                out.append("</" + name + ">")
                continue
            if name == "pre":
                pre_depth = max(0, pre_depth - 1)
                out.append("</pre>")
                continue
            out.append("</" + name + ">")
            continue
        if name == "div":
            class_match = re.search(r'class="([^"]*)"', inner)
            classes = class_match.group(1).split() if class_match else []
            if re.search(r'\bid="header"', inner):
                div_stack.append("header")
                continue
            if any(c in WRAPPER_DIVS for c in classes):
                id_match = re.search(r'\bid="([^"]*)"', inner)
                if id_match and not id_match.group(1).startswith("_"):
                    pending_id = id_match.group(1)
                div_stack.append("wrapper")
                continue
            id_match = re.search(r'\bid="([^"]*)"', inner)
            if id_match and not id_match.group(1).startswith("_"):
                pending_id = id_match.group(1)
            div_stack.append(None)
            out.append("<div>")
            continue
        if name in ("thead", "tbody", "colgroup"):
            continue
        if name == "col":
            continue
        if name == "p" and container_depth > 0:
            continue
        if name == "pre":
            pre_depth += 1
            out.append("<pre>")
            continue
        if name == "code":
            if pre_depth > 0:
                continue
            out.append("{code}")
            continue
        if name.startswith("h") and len(name) == 2 and name[1].isdigit():
            own_id = ""
            id_match = re.search(r'\bid="([^"]*)"', inner)
            if id_match and not id_match.group(1).startswith("_"):
                own_id = id_match.group(1)
            if pending_id and not own_id:
                out.append("<" + name + "@id=\"" + pending_id + "\">")
            elif own_id:
                out.append("<" + name + "@id=\"" + own_id + "\">")
            else:
                out.append("<" + name + ">")
            pending_id = ""
            continue
        attrs = ""
        for attr in ("id", "href", "src", "colspan"):
            am = re.search(attr + r'\s*=\s*"([^"]*)"', inner)
            if am:
                value = am.group(1)
                if attr == "id" and value.startswith("_"):
                    continue
                attrs += "@" + attr + '="' + value + '"'
        if name in CONTAINER_TAGS:
            container_depth += 1
        out.append("<" + name + attrs + ">")
    return "".join(out).rstrip()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--program", default="./markup")
    ap.add_argument("--reference", default="tests/fixtures/asciidoctor-2.0.26/reference.json")
    ap.add_argument("--manifest", default="tests/fixtures/asciidoctor-2.0.26/differences.json")
    ap.add_argument("--corpus", default="tests/fixtures/asciidoctor-2.0.26/corpus.json")
    args = ap.parse_args()
    ref_path = pathlib.Path(args.reference)
    if not ref_path.exists():
        print(
            "pinned Asciidoctor 2.0.26 reference.json absent; run "
            "tests/asciidoctor_reference.py against the pinned gem before this gate "
            "can compare output",
            file=sys.stderr,
        )
        return 2
    references = {x["id"]: x for x in json.loads(ref_path.read_text())}
    cases = json.loads(pathlib.Path(args.corpus).read_text())
    manifest = json.loads(pathlib.Path(args.manifest).read_text())
    manifest_ids = {x["id"] for x in manifest}
    case_ids = {x["id"] for x in cases}
    if len(manifest) != len(manifest_ids):
        print("differences.json has duplicate ids", file=sys.stderr)
        return 1
    if not manifest_ids <= case_ids:
        print("differences.json lists unknown case ids", file=sys.stderr)
        return 1
    manifest_map = {x["id"]: x for x in manifest}
    for entry in manifest:
        if not isinstance(entry, dict) or not entry.get("reason"):
            print("every differences.json entry needs a reason", file=sys.stderr)
            return 1
        if not (entry.get("expected_normalized_sha256") and entry.get("actual_normalized_sha256")):
            print(
                f"differences.json entry {entry['id']} must pin both normalized SHA-256 hashes",
                file=sys.stderr,
            )
            return 1
    failures = []
    accepted = []
    passed = 0
    for case in cases:
        run = subprocess.run(
            [args.program, "--format", "asciidoc", "-"],
            input=case["input"], text=True, capture_output=True,
        )
        if run.returncode:
            failures.append(case["id"] + ": Markup++ conversion failed")
            continue
        actual = normalize_fragment(run.stdout)
        expected = normalize_fragment(references[case["id"]]["html"])
        actual_hash = hashlib.sha256(actual.encode()).hexdigest()
        expected_hash = hashlib.sha256(expected.encode()).hexdigest()
        if actual == expected:
            if case["id"] in manifest_ids:
                failures.append(
                    case["id"] + ": recorded difference now matches; remove the manifest entry"
                )
                continue
            passed += 1
            continue
        entry = manifest_map.get(case["id"])
        if entry is None:
            failures.append(case["id"] + ": normalized output differs from Asciidoctor 2.0.26")
            continue
        if entry["expected_normalized_sha256"] != expected_hash:
            failures.append(
                case["id"] + ": pinned expected reference hash changed "
                f"({entry['expected_normalized_sha256'][:12]} vs {expected_hash[:12]})"
            )
            continue
        if entry["actual_normalized_sha256"] != actual_hash:
            failures.append(
                case["id"] + ": pinned actual Markup++ output hash changed "
                f"({entry['actual_normalized_sha256'][:12]} vs {actual_hash[:12]})"
            )
            continue
        if expected_hash == actual_hash:
            failures.append(case["id"] + ": pinned difference is not actually different")
            continue
        accepted.append(case["id"])
    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print(
        f"AC9 output gate: Asciidoctor 2.0.26, {len(cases)} corpus cases, "
        f"{passed} normalized matches, {len(accepted)} pinned differences"
    )
    if accepted:
        print("Pinned differences:", ", ".join(accepted), file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())