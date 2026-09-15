# Dynamic analysis

This directory holds runtime fixtures for `scripts/run-dynamic-analysis.sh`.

```bash
bash scripts/build-local-linux.sh
bash scripts/run-dynamic-analysis.sh
```

The runner starts two loopback instances:

- `pref-lan.toml` on `127.0.0.1:25500`
- `pref-public.toml` on `127.0.0.1:25501`

and writes:

- `docs/dynamic-analysis.md` (committed report after a successful run)
- `tests/dynamic/last-run.json` (local machine-readable artifact, gitignored)

`empty.ini` is a local default external config so startup does not fall back
to the remote jsdelivr template. Conversion cases still pass
`config=data:,enable_rule_generator=false` so each request stays offline.

Cases cover conversion mode (Clash proxy-provider vs direct node URI),
non-Clash HTTP subscriptions, API-mode script ignore, and public-profile
upload/config fetch behavior. They do not include exploit payloads.

Existing HTTP smoke checks remain in `scripts/run-subconverter-smoke.py`.
