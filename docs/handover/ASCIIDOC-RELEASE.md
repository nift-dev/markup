# AsciiDoc pinned-development-profile release evidence

This file records AD11 evidence against the currently available, unreleased
Eclipse AsciiDoc Language work. It is not certification against a stable
AsciiDoc standard and it is not an Asciidoctor compatibility claim.

## Frozen inputs

- Language specification commit: `68ed0b22e8d9b919897542d8ae14f03e7dbdd2e4`
- TCK commit: `cdfada9c2768b164eadf4bc12e9f9c68e6caf68a`
- TCK package label: `1.0.0-alpha.0`
- Inventoried upstream inputs: 13/13
- Exclusions: none from the input inventory

The alpha TCK's official result oracle is an Abstract Semantic Graph. Its
expected ASG fixtures were not available in the frozen upstream snapshot.
Markup++ therefore gates all available inputs, deterministic conversion and
reviewed HTML digests, while retaining the explicit `development profile`
wording. When expected ASGs become available, matching them is a mandatory
spec-upgrade checkpoint before a stable language-conformance claim.

## Reproduce locally

```sh
make test-asciidoc-release
make test-release-local
make test-sanitize
```

The pushed cross-platform gate must run the same CMake/CTest suite on Linux,
macOS and Windows. Record its run ID here before a tagged release. A local pass
does not substitute for that evidence.
