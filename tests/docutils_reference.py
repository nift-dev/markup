#!/usr/bin/env python3
"""Generate frozen Docutils 0.23 doctree and HTML5 evidence."""
import argparse, contextlib, io, json, pathlib, sys

def main():
    ap=argparse.ArgumentParser(); ap.add_argument("--output",default="tests/fixtures/docutils-0.23/reference.json")
    args=ap.parse_args()
    try:
        import docutils
        from docutils.core import publish_doctree, publish_parts
    except ImportError:
        print("docutils==0.23 is required",file=sys.stderr); return 2
    if docutils.__version__ != "0.23": print("docutils==0.23 is required",file=sys.stderr); return 2
    settings={"raw_enabled":False,"file_insertion_enabled":False,"syntax_highlight":"none",
              "report_level":1,"halt_level":6,"warning_stream":None,"traceback":True}
    cases=json.loads(pathlib.Path("tests/fixtures/docutils-0.23/corpus.json").read_text())
    evidence=[]
    for case in cases:
        warnings=io.StringIO(); local=dict(settings,warning_stream=warnings)
        with contextlib.redirect_stderr(warnings):
            tree=publish_doctree(case["input"],source_path=f"{case['id']}.rst",settings_overrides=local)
            parts=publish_parts(case["input"],source_path=f"{case['id']}.rst",writer_name="html5",settings_overrides=local)
        evidence.append({"id":case["id"],"doctree":tree.pformat(),"fragment":parts["fragment"],"diagnostics":warnings.getvalue()})
    pathlib.Path(args.output).write_text(json.dumps(evidence,indent=2,sort_keys=True)+"\n")
    print(f"wrote {len(evidence)} Docutils 0.23 reference cases")
    return 0
if __name__=="__main__": raise SystemExit(main())
