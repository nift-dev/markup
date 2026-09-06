# Markup++ release procedure

## Release identity

- Repository: `nift-dev/markup`
- Executable: `markup`
- Current candidate: `0.1.0`
- Tag format: `vX.Y.Z`
- Public scripts: `https://nift-dev.github.io/markup-website/{install,download,update,uninstall}`
- Assets: Linux x86-64, macOS arm64, macOS x86-64 and Windows x86-64 archives plus `SHA256SUMS`.

A validated checkpoint is not a public release. Pushing the annotated release
tag is the deliberate publication action.

## Candidate validation

1. Start from a clean intended `main` commit and confirm `markup --version`
   matches the proposed tag version.
2. Run `make test-release-local`, `make test-sanitize`, and `make test-packaging`.
3. Run `make check-nift-sync NIFT_DIR=/path/to/nift`, followed by Nift's
   Markup++ integration and regression gates.
4. Reconcile the README, release notes, compatibility-profile documents, known
   differences, website claims and handovers with the exact candidate.
5. Synchronize all four `packaging/*.sh` scripts byte-for-byte to the website
   root and generated `public/` checkout. Commit generated `public/` first and
   website source second.
6. Push the website commits and confirm every live script matches its canonical
   source. This must happen before release rehearsal.
7. Commit and push the candidate, then dispatch `.github/workflows/release.yml`
   with version `X.Y.Z`. Require every platform archive smoke, packaging test,
   public-script check and the aggregate `rehearse` job to pass at the exact
   candidate SHA.
8. Download the rehearsal bundle, verify its exact five-file set and retain the
   checksums as candidate evidence. Rehearsal must not publish anything.

## Publication

1. Obtain explicit approval to release.
2. Create an annotated `vX.Y.Z` tag at the exact rehearsed commit and push only
   that tag.
3. Require the tag workflow to rebuild and smoke-test all platform archives
   before `publish` creates the GitHub release.
4. Confirm the release contains exactly the four expected archives and
   `SHA256SUMS`. Published assets are immutable; never replace or automatically
   repair an existing release.
5. Require every `installer-public-smoke` job to install the tagged version
   through the live public installer.
6. Independently download the manifest and representative public archives,
   verify them, extract them and run `markup --version` plus one conversion.

## Close-out

Record the full tag commit, release and workflow URLs, asset checksums, platform
job conclusions, installer results, website source/generated commits, Nift
synchronization identity and known limitations. Confirm every repository is
clean and contains no ignored build products.

After release, move the CLI and build-system version to the next development
identity before unrelated work. Change `markup::api_version` only for an actual
public API or semantic-contract change.
