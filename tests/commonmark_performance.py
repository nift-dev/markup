#!/usr/bin/env python3
"""Coarse release guards for large CommonMark documents."""

import argparse
import resource
import subprocess
import sys
import time


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--program", required=True)
    args = parser.parse_args()

    documents = [
        ("plain-2mb", ("ordinary text for a large paragraph " * 65536) + "\n"),
        ("blocks", ("# heading\n\n- one\n- two\n\n> quote\n\n" * 20000)),
        ("delimiters", ("*one* **two** `three` [four](/url) " * 30000) + "\n"),
    ]
    started = time.monotonic()
    for name, source in documents:
        try:
            result = subprocess.run(
                [args.program, "--format", "markdown", "-"], input=source,
                text=True, capture_output=True, timeout=10, check=False)
        except subprocess.TimeoutExpired:
            print(f"FAIL {name}: exceeded 10 second guard", file=sys.stderr)
            return 1
        if result.returncode:
            print(f"FAIL {name}: {result.stderr}", file=sys.stderr)
            return 1
        if not result.stdout or len(result.stdout) > len(source) * 8 + 1024:
            print(f"FAIL {name}: invalid output size", file=sys.stderr)
            return 1

    elapsed = time.monotonic() - started
    usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
    # Linux reports KiB; macOS reports bytes.
    rss_mib = usage / (1024 * 1024) if sys.platform == "darwin" else usage / 1024
    if rss_mib > 384:
        print(f"FAIL peak child RSS {rss_mib:.1f} MiB exceeds 384 MiB", file=sys.stderr)
        return 1
    if elapsed > 20:
        print(f"FAIL total runtime {elapsed:.2f}s exceeds 20s", file=sys.stderr)
        return 1
    print(f"3 large-document guards passed in {elapsed:.2f}s; peak child RSS {rss_mib:.1f} MiB")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
