#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"; tmp="$(mktemp -d "${TMPDIR:-/tmp}/markup-packaging-test.XXXXXX")"; trap 'rm -rf "$tmp"' EXIT
for script in install download update uninstall; do sh -n "$root/packaging/$script.sh"; done
version="$("$root/markup" --version | sed -E 's/^Markup\+\+ ([^ ]+)$/\1/')"
case "$(uname -s)/$(uname -m)" in
  Linux/x86_64|Linux/amd64) platform="linux-x86_64" ;;
  Darwin/arm64|Darwin/aarch64) platform="macos-arm64" ;;
  Darwin/x86_64|Darwin/amd64) platform="macos-x86_64" ;;
  *) echo "unsupported packaging-smoke platform" >&2; exit 1 ;;
esac
archive="markup-$version-$platform.tar.gz"; stage="$tmp/release/markup-$version-$platform"
mkdir -p "$stage" "$tmp/bin" "$tmp/downloads" "$tmp/website"; cp "$root/markup" "$stage/markup"; cp "$root/README.md" "$root/LICENSE" "$stage/"
tar -czf "$tmp/release/$archive" -C "$tmp/release" "markup-$version-$platform"; (cd "$tmp/release" && if command -v sha256sum >/dev/null 2>&1; then sha256sum "$archive"; else shasum -a 256 "$archive"; fi > SHA256SUMS)
MARKUP_VERSION="$version" MARKUP_RELEASE_BASE="file://$tmp/release" MARKUP_INSTALL_DIR="$tmp/bin" sh "$root/packaging/install.sh"
test "$("$tmp/bin/markup" --version)" = "Markup++ $version"; printf '# Hello\n' | "$tmp/bin/markup" --format md - | grep -q '<h1>Hello</h1>'
MARKUP_VERSION="$version" MARKUP_RELEASE_BASE="file://$tmp/release" MARKUP_DOWNLOAD_DIR="$tmp/downloads" sh "$root/packaging/download.sh"
test -f "$tmp/downloads/$archive"; test -f "$tmp/downloads/$archive.sha256"
cp "$root/packaging/install.sh" "$tmp/website/install"
MARKUP_VERSION="$version" MARKUP_RELEASE_BASE="file://$tmp/release" MARKUP_INSTALL_DIR="$tmp/bin" MARKUP_WEBSITE_BASE="file://$tmp/website" sh "$root/packaging/update.sh"
test "$("$tmp/bin/markup" --version)" = "Markup++ $version"
MARKUP_INSTALL_DIR="$tmp/bin" sh "$root/packaging/uninstall.sh"; test ! -e "$tmp/bin/markup"
echo "Markup++ packaging smoke passed"

# Negative coverage: a tampered or missing checksum must fail closed.
mkdir -p "$tmp/badrelease/markup-$version-$platform"
cp "$root/markup" "$tmp/badrelease/markup-$version-$platform/markup"
tar -czf "$tmp/badrelease/$archive" -C "$tmp/badrelease" "markup-$version-$platform"
printf '0' > "$tmp/badrelease/SHA256SUMS"
if MARKUP_VERSION="$version" MARKUP_RELEASE_BASE="file://$tmp/badrelease" MARKUP_INSTALL_DIR="$tmp/bin" sh "$root/packaging/install.sh" >/dev/null 2>&1; then
  echo "install must reject a missing checksum entry" >&2; exit 1
fi
printf '%s  %s\n' "0000000000000000000000000000000000000000000000000000000000000000" "$archive" > "$tmp/badrelease/SHA256SUMS"
if MARKUP_VERSION="$version" MARKUP_RELEASE_BASE="file://$tmp/badrelease" MARKUP_INSTALL_DIR="$tmp/bin" sh "$root/packaging/install.sh" >/dev/null 2>&1; then
  echo "install must reject a checksum mismatch" >&2; exit 1
fi
echo "Markup++ packaging negative checks passed"

# Safe staged replacement: overwriting an existing executable succeeds, an
# existing destination symlink is replaced as a directory entry (referent
# unchanged), a stale candidate staging file is never followed or
# overwritten, and no staged installer file remains.
printf 'old\n' > "$tmp/referent"
ln -s "$tmp/referent" "$tmp/bin/markup"
printf 'stale-referent\n' > "$tmp/stale-referent"
ln -s "$tmp/stale-referent" "$tmp/bin/.markup-install.zzzzzz"
MARKUP_VERSION="$version" MARKUP_RELEASE_BASE="file://$tmp/release" MARKUP_INSTALL_DIR="$tmp/bin" sh "$root/packaging/install.sh"
test ! -L "$tmp/bin/markup"
test "$(cat "$tmp/referent")" = "old"
test -L "$tmp/bin/.markup-install.zzzzzz"
test "$(cat "$tmp/stale-referent")" = "stale-referent"
test "$("$tmp/bin/markup" --version)" = "Markup++ $version"
test "$(find "$tmp/bin" -name '.markup-install.*' ! -name '.markup-install.zzzzzz' | wc -l)" -eq 0
echo "Markup++ safe staged replacement passed"
