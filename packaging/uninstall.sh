#!/bin/sh
set -eu
install_dir="${MARKUP_INSTALL_DIR:-$HOME/.local/bin}"; target="$install_dir/markup"
if [ -e "$target" ] || [ -L "$target" ]; then rm -f -- "$target"; printf 'Removed Markup++ from %s\n' "$target"; else printf 'Markup++ is not installed at %s\n' "$target"; fi
printf 'Shell PATH configuration and user files were left unchanged.\n'
