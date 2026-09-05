#!/usr/bin/env python3
"""Generate or verify the frozen Asciidoctor 2.0.26 reference corpus."""
import argparse, json, pathlib, subprocess, sys

PROFILE = pathlib.Path("tests/fixtures/asciidoctor-2.0.26/PROFILE.json")
CORPUS = pathlib.Path("tests/fixtures/asciidoctor-2.0.26/corpus.json")

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--asciidoctor", default="asciidoctor")
    ap.add_argument("--output", default="tests/fixtures/asciidoctor-2.0.26/reference.json")
    args = ap.parse_args()
    profile = json.loads(PROFILE.read_text())
    version = subprocess.run([args.asciidoctor, "--version"], text=True, capture_output=True)
    if version.returncode or profile["version"] not in version.stdout:
        print("Asciidoctor 2.0.26 is required", file=sys.stderr); return 2
    evidence = []
    command = [args.asciidoctor, "--backend", "html5", "--doctype", "article",
               "--safe-mode", "secure", "--embedded", "--attribute", "reproducible",
               "--attribute", "env=markup", "--attribute", "env-markup", "-"]
    for case in json.loads(CORPUS.read_text()):
        run = subprocess.run(command, input=case["input"], text=True, capture_output=True, check=True)
        evidence.append({"id": case["id"], "html": run.stdout, "stderr": run.stderr})
    pathlib.Path(args.output).write_text(json.dumps(evidence, indent=2) + "\n")
    print(f"wrote {len(evidence)} Asciidoctor 2.0.26 reference cases")
    return 0
if __name__ == "__main__": raise SystemExit(main())
