#!/usr/bin/env bash
# Build (if needed), start LAN/public instances, and run runtime analysis.
set -euo pipefail

ROOT="$(CDPATH= cd -- "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="${SCE_BIN:-$ROOT/build/subconverter}"
LAN_PORT="${LAN_PORT:-25500}"
PUBLIC_PORT="${PUBLIC_PORT:-25501}"
WORKDIR="${SCE_RUNTIME_DIR:-/tmp/sce-dynamic}"
REPORT="${REPORT:-$ROOT/docs/dynamic-analysis.md}"
JSON_REPORT="${JSON_REPORT:-$ROOT/tests/dynamic/last-run.json}"

export LD_LIBRARY_PATH="$ROOT/bridge${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

if [[ ! -x "$BIN" ]]; then
  echo "==> binary missing, building first"
  bash "$ROOT/scripts/build-local-linux.sh"
fi

if [[ ! -f "$ROOT/bridge/libmihomo.so" ]]; then
  echo "bridge/libmihomo.so is missing; rebuild with scripts/build-local-linux.sh" >&2
  exit 1
fi

prepare_runtime() {
  local name="$1"
  local pref="$2"
  local dir="$WORKDIR/$name"
  rm -rf "$dir"
  mkdir -p "$dir"
  cp -a "$ROOT/base" "$dir/base"
  cp "$pref" "$dir/pref.toml"
  cp "$ROOT/tests/dynamic/empty.ini" "$dir/empty.ini"
  printf '%s\n' "$dir"
}

wait_ready() {
  local url="$1"
  local log="$2"
  local pid="$3"
  for _ in $(seq 1 40); do
    if ! kill -0 "$pid" >/dev/null 2>&1; then
      echo "instance at $url exited before becoming ready" >&2
      cat "$log" >&2 || true
      return 1
    fi
    if curl -fsS "$url/healthz" >/dev/null 2>&1; then
      return 0
    fi
    sleep 0.25
  done
  echo "instance at $url did not become ready" >&2
  cat "$log" >&2 || true
  return 1
}

LAN_DIR="$(prepare_runtime lan "$ROOT/tests/dynamic/pref-lan.toml")"
PUBLIC_DIR="$(prepare_runtime public "$ROOT/tests/dynamic/pref-public.toml")"

cleanup() {
  if [[ -n "${LAN_PID:-}" ]]; then kill "$LAN_PID" >/dev/null 2>&1 || true; fi
  if [[ -n "${PUBLIC_PID:-}" ]]; then kill "$PUBLIC_PID" >/dev/null 2>&1 || true; fi
}
trap cleanup EXIT

echo "==> starting LAN instance on 127.0.0.1:${LAN_PORT}"
(
  cd "$LAN_DIR"
  "$BIN" -f "$LAN_DIR/pref.toml"
) >"$LAN_DIR/server.log" 2>&1 &
LAN_PID=$!

echo "==> starting public instance on 127.0.0.1:${PUBLIC_PORT}"
(
  cd "$PUBLIC_DIR"
  "$BIN" -f "$PUBLIC_DIR/pref.toml"
) >"$PUBLIC_DIR/server.log" 2>&1 &
PUBLIC_PID=$!

wait_ready "http://127.0.0.1:${LAN_PORT}" "$LAN_DIR/server.log" "$LAN_PID"
wait_ready "http://127.0.0.1:${PUBLIC_PORT}" "$PUBLIC_DIR/server.log" "$PUBLIC_PID"

mkdir -p "$(dirname "$REPORT")" "$(dirname "$JSON_REPORT")"
python3 "$ROOT/scripts/run-dynamic-analysis.py" \
  --lan-url "http://127.0.0.1:${LAN_PORT}" \
  --public-url "http://127.0.0.1:${PUBLIC_PORT}" \
  --report "$REPORT" \
  --json "$JSON_REPORT"

echo "==> report: $REPORT"
echo "==> json: $JSON_REPORT"
echo "==> lan log: $LAN_DIR/server.log"
echo "==> public log: $PUBLIC_DIR/server.log"
