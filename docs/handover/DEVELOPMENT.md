# Development

## Local workflow

```sh
make clean
make test
make test-sanitize
```

Keep public API changes small and compile at least one consumer using only
`-Iinclude`. Do not let private parser types escape through `Markup.h`.

## Adding Markdown behavior

1. State whether the behavior is CommonMark, GFM or a Markup++ extension.
2. Add the valid case, delimiter/indentation neighbors and malformed recovery.
3. Prefer exact output tests for stable behavior.
4. Add adversarial and mutation coverage for new scanning states.
5. Run sanitizers before describing memory-safety evidence.
6. Update compatibility claims and gaps.

Do not “fix” one example using a broad replacement that changes code blocks,
escaped syntax or raw HTML. Block precedence and inline delimiter behavior need
focused neighbors.

## Adding a format

Add a private converter with its own tests and dispatch it from `convert`.
Change `is_supported` only when the converter's documented minimum surface and
error behavior are tested. Keep extension inference in the CLI/API helper, not
inside the converter.

## CLI changes

The CLI must remain a thin caller. Preserve:

- binary reads and writes;
- stdin requiring explicit format;
- stdout as the default;
- atomic destination replacement;
- existing permission preservation;
- output-symlink refusal;
- input overwrite refusal;
- nonzero exits for usage, IO and conversion errors.
