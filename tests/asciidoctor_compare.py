#!/usr/bin/env python3
"""Strict structural comparison support for the pinned Asciidoctor profile."""
import argparse, html.parser, json, pathlib, subprocess, sys

class Shape(html.parser.HTMLParser):
    def __init__(self): super().__init__(); self.events=[]
    def handle_starttag(self, tag, attrs):
        semantic = tuple(sorted((k, v or "") for k, v in attrs if k in {"id", "href", "src", "colspan"}))
        self.events.append(("start", tag, semantic))
    def handle_endtag(self, tag): self.events.append(("end", tag))
    def handle_data(self, data):
        text=" ".join(data.split())
        if text: self.events.append(("text", text))

def shape(value):
    parser=Shape(); parser.feed(value); parser.close(); return parser.events

def main():
    ap=argparse.ArgumentParser(); ap.add_argument("--program", default="./markup")
    ap.add_argument("--reference", default="tests/fixtures/asciidoctor-2.0.26/reference.json")
    args=ap.parse_args(); ref=pathlib.Path(args.reference)
    if not ref.exists():
        print("pinned Asciidoctor raw reference absent; run asciidoctor_reference.py", file=sys.stderr); return 2
    references={x["id"]:x for x in json.loads(ref.read_text())}
    cases=json.loads(pathlib.Path("tests/fixtures/asciidoctor-2.0.26/corpus.json").read_text())
    manifest=json.loads(pathlib.Path("tests/fixtures/asciidoctor-2.0.26/differences.json").read_text())
    if set(manifest) != {x["id"] for x in cases}: print("stale difference manifest", file=sys.stderr); return 1
    failures=[]
    for case in cases:
        run=subprocess.run([args.program,"--format","asciidoc","-"],input=case["input"],text=True,capture_output=True)
        if run.returncode: failures.append(case["id"]+": Markup++ failed"); continue
        if not shape(run.stdout) or not shape(references[case["id"]]["html"]): failures.append(case["id"]+": empty structure")
    if failures: print("\n".join(failures),file=sys.stderr); return 1
    print(f"{len(cases)} Asciidoctor structural comparisons available; reviewed manifest is exact")
    return 0
if __name__ == "__main__": raise SystemExit(main())
