#!/bin/sh
set -eu
repo="${MARKUP_GITHUB_REPOSITORY:-nift-dev/markup}"; version="${MARKUP_VERSION:-}"; download_dir="${MARKUP_DOWNLOAD_DIR:-$PWD}"
command -v curl >/dev/null 2>&1 || { echo "markup download: required command not found: curl" >&2; exit 1; }
command -v mktemp >/dev/null 2>&1 || { echo "markup download: required command not found: mktemp" >&2; exit 1; }
if [ -z "$version" ]; then
  latest_url="$(curl --retry 5 --retry-all-errors --retry-delay 2 -fsSL -o /dev/null -w '%{url_effective}' "https://github.com/$repo/releases/latest")"; version="${latest_url##*/v}"
  case "$version" in ''|*[!0-9A-Za-z._-]*) echo "markup download: could not determine latest release version" >&2; exit 1;; esac
fi
version="${version#v}"; os="$(uname -s)"; arch="$(uname -m)"
case "$os/$arch" in
  Linux/x86_64|Linux/amd64) platform="linux-x86_64";;
  Darwin/arm64|Darwin/aarch64) platform="macos-arm64";;
  Darwin/x86_64|Darwin/amd64) platform="macos-x86_64";;
  *) echo "markup download: unsupported platform: $os/$arch" >&2; exit 1;;
esac
archive="markup-$version-$platform.tar.gz"; base="${MARKUP_RELEASE_BASE:-https://github.com/$repo/releases/download/v$version}"
tmp="$(mktemp -d "${TMPDIR:-/tmp}/markup-download.XXXXXX")"; trap 'rm -rf "$tmp"' EXIT HUP INT TERM
curl --retry 5 --retry-all-errors --retry-delay 2 -fsSL "$base/$archive" -o "$tmp/$archive"
curl --retry 5 --retry-all-errors --retry-delay 2 -fsSL "$base/SHA256SUMS" -o "$tmp/SHA256SUMS"
expected="$(awk -v file="$archive" '$2 == file { print $1; exit }' "$tmp/SHA256SUMS")"
[ -n "$expected" ] || { echo "markup download: checksum for $archive not found" >&2; exit 1; }
if command -v sha256sum >/dev/null 2>&1; then actual="$(sha256sum "$tmp/$archive" | awk '{print $1}')";
elif command -v shasum >/dev/null 2>&1; then actual="$(shasum -a 256 "$tmp/$archive" | awk '{print $1}')";
else echo "markup download: sha256sum or shasum is required" >&2; exit 1; fi
[ "$actual" = "$expected" ] || { echo "markup download: checksum verification failed for $archive" >&2; exit 1; }
mkdir -p "$download_dir"; cp "$tmp/$archive" "$download_dir/$archive"; printf '%s  %s\n' "$expected" "$archive" > "$download_dir/$archive.sha256"
printf 'Downloaded and verified %s in %s\n' "$archive" "$download_dir"
