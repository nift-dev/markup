# reStructuredText compatibility release gate

The release profile is the reStructuredText specification and core parser
behavior of Docutils 0.23 using the standalone reader, reStructuredText parser
and HTML5 writer. Raw and file insertion are disabled in the oracle.

Before publishing the exact compatibility wording:

1. Run `make test-rst-release` in a clean checkout.
2. Run the full `make test` suite and sanitizer target.
3. Push the candidate and require Linux, macOS and Windows CI to pass.
4. Preserve the immutable commit and Actions run in this file.
5. Review every documented subset/difference; never infer Sphinx compatibility.
6. Publish the pinned Docutils version, 24-case oracle count, writer/settings,
   capability policy and unsupported extensions beside the claim.

Local implementation evidence is complete. The public wording remains gated
until a pushed candidate supplies the three-platform run identifier.
