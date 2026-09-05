# AsciiDoc language reference pins

Markup++ freezes the currently unreleased Eclipse AsciiDoc language work by
immutable commit rather than following a mutable branch.

- Specification repository: `https://gitlab.eclipse.org/eclipse/asciidoc-lang/asciidoc-lang`
- Specification commit: `68ed0b22e8d9b919897542d8ae14f03e7dbdd2e4`
- TCK repository: `https://gitlab.eclipse.org/eclipse/asciidoc-lang/asciidoc-tck`
- TCK commit: `cdfada9c2768b164eadf4bc12e9f9c68e6caf68a`
- TCK package version at that commit: `1.0.0-alpha.0`
- Snapshot date: 2026-09-05
- Specification/TCK license: EPL-2.0

The upstream README explicitly says releases are not available yet. The current
TCK contains thirteen language cases and validates an Abstract Semantic Graph,
not HTML. `cases.json` preserves every upstream input and its classification so
the baseline is reproducible offline. Expected upstream ASG output will be
consumed by the adapter gate as the internal AsciiDoc document model matures;
HTML fixtures alone must never be presented as TCK evidence.

Upgrade procedure:

1. Fetch both repositories without changing these pins.
2. Review specification, ASG schema and TCK changelogs.
3. Record old/new immutable commits and package versions.
4. Inventory added, removed and changed cases.
5. Vendor the new case inputs/expected ASGs and their checksums.
6. Run the old target first, then establish the new per-family baseline.
7. Update public wording only after the new release gate passes.
