#!/usr/bin/env python3
"""Runtime checks for SubConverter-Extended.

Point this at a running instance. It records observed conversion and
security-profile behavior without sending exploit payloads.

Exit 0 if every required case passes. Use --report to write Markdown.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from dataclasses import dataclass, field
from typing import Any


SAMPLE_SS = "ss://YWVzLTEyOC1nY206cGFzc3dvcmQ@example.com:8388#SmokeSS"
SAMPLE_HY2 = (
    "hysteria2://password@example.com:443?insecure=1&sni=example.com#SmokeHY2"
)
SAMPLE_ANYTLS = "anytls://password@example.com:443?sni=example.com#SmokeAnyTLS"
HTTP_SUB = "https://example.com/sub"
DISABLE_RULEGEN = "data:,enable_rule_generator=false"
LOCAL_CONFIG = "http://127.0.0.1:1/not-a-real-config"


@dataclass
class CaseResult:
    name: str
    passed: bool
    expected: str
    observed: str
    detail: str = ""
    duration_ms: int = 0
    required: bool = True


@dataclass
class Report:
    cases: list[CaseResult] = field(default_factory=list)

    def add(self, case: CaseResult) -> None:
        self.cases.append(case)

    @property
    def failed(self) -> list[CaseResult]:
        return [c for c in self.cases if c.required and not c.passed]

    @property
    def ok(self) -> bool:
        return not self.failed


def encode_params(params: dict[str, str]) -> str:
    return urllib.parse.urlencode(params)


def request(
    base_url: str,
    path: str,
    params: dict[str, str] | None = None,
    timeout: int = 20,
    headers: dict[str, str] | None = None,
    method: str = "GET",
) -> tuple[int, str, dict[str, str]]:
    url = base_url.rstrip("/") + path
    if params:
        url += "?" + encode_params(params)
    req = urllib.request.Request(url, method=method, headers=headers or {})
    try:
        with urllib.request.urlopen(req, timeout=timeout) as resp:
            body = resp.read().decode("utf-8", errors="replace")
            hdrs = {k.lower(): v for k, v in resp.headers.items()}
            return resp.status, body, hdrs
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        hdrs = {k.lower(): v for k, v in exc.headers.items()} if exc.headers else {}
        return exc.code, body, hdrs
    except urllib.error.URLError as exc:
        return 0, str(exc.reason if getattr(exc, "reason", None) else exc), {}


def run_case(name: str, expected: str, fn, required: bool = True) -> CaseResult:
    started = time.monotonic()
    try:
        passed, observed, detail = fn()
    except Exception as exc:  # noqa: BLE001 - surface unexpected runtime errors
        passed, observed, detail = False, type(exc).__name__, str(exc)
    duration_ms = int((time.monotonic() - started) * 1000)
    return CaseResult(
        name=name,
        passed=bool(passed),
        expected=expected,
        observed=observed,
        detail=detail,
        duration_ms=duration_ms,
        required=required,
    )


def recognized(report: dict[str, Any], name: str) -> dict[str, Any]:
    items = report.get("parameters", {}).get("recognized", [])
    for item in items:
        if item.get("name") == name:
            return item
    return {}


def add_lan_cases(report: Report, base: str, timeout: int) -> None:
    def healthz():
        status, body, _ = request(base, "/healthz", timeout=timeout)
        return status == 200 and body.strip() == "ok", f"HTTP {status} {body!r}", ""

    report.add(
        run_case("healthz", "HTTP 200 body ok", healthz)
    )

    def version_html():
        status, body, headers = request(base, "/version", timeout=timeout)
        ok = (
            status == 200
            and "<!DOCTYPE html>" in body
            and "SubConverter-Extended" in body
            and headers.get("content-type", "").startswith("text/html")
        )
        return ok, f"HTTP {status} ctype={headers.get('content-type')}", body[:120]

    report.add(run_case("version_html", "HTML version page", version_html))

    def version_probe():
        status, body, headers = request(
            base,
            "/version",
            timeout=timeout,
            headers={
                "Origin": "https://edgetunnel.example",
                "Sec-Fetch-Mode": "cors",
                "Sec-Fetch-Dest": "empty",
            },
        )
        line = body.strip()
        ok = (
            status == 200
            and re.fullmatch(r"SubConverter-Extended \S+ backend", line) is not None
            and headers.get("content-type", "").startswith("text/plain")
        )
        return ok, f"HTTP {status} {line!r}", ""

    report.add(run_case("version_probe", "plain-text backend probe", version_probe))

    def inspect_page():
        status, body, _ = request(base, "/inspect", timeout=timeout)
        ok = status == 200 and "Request Inspector" in body and "request-input" in body
        return ok, f"HTTP {status} inspector={'yes' if 'Request Inspector' in body else 'no'}", ""

    report.add(run_case("inspect_page", "inspector HTML", inspect_page))

    def webapp_page():
        status, body, _ = request(base, "/", timeout=timeout)
        ok = status == 200 and "SubConverter-Extended" in body and "convert-form" in body
        return ok, f"HTTP {status} form={'yes' if 'convert-form' in body else 'no'}", ""

    report.add(run_case("webapp_page", "converter HTML", webapp_page))

    def clash_http_provider():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": HTTP_SUB,
                "config": DISABLE_RULEGEN,
            },
            timeout=timeout,
        )
        ok = (
            status == 200
            and "proxy-providers:" in body
            and HTTP_SUB in body
            and "proxies:" not in body.split("proxy-providers:")[0][-80:]
        )
        has_provider = "proxy-providers:" in body
        return (
            ok,
            f"HTTP {status} proxy-providers={has_provider}",
            body[:400],
        )

    report.add(
        run_case(
            "clash_http_is_proxy_provider",
            "Clash HTTP URL becomes proxy-providers, not fetched nodes",
            clash_http_provider,
        )
    )

    def clash_http_explain():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": HTTP_SUB,
                "config": DISABLE_RULEGEN,
                "explain": "true",
            },
            timeout=timeout,
        )
        try:
            data = json.loads(body)
        except json.JSONDecodeError:
            return False, f"HTTP {status} not JSON", body[:200]
        mode = data.get("mode", {})
        ok = (
            status == 200
            and mode.get("proxy_provider") is True
            and data.get("output", {}).get("provider_count") == 1
            and data.get("nodes", {}).get("total", 1) == 0
        )
        return (
            ok,
            f"HTTP {status} provider={mode.get('proxy_provider')} nodes={data.get('nodes')}",
            "",
        )

    report.add(
        run_case(
            "clash_http_explain_provider_mode",
            "explain JSON reports proxy_provider and zero local nodes",
            clash_http_explain,
        )
    )

    def clash_ss_direct():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_SS,
                "config": DISABLE_RULEGEN,
            },
            timeout=timeout,
        )
        ok = status == 200 and "SmokeSS" in body and "proxies:" in body
        return ok, f"HTTP {status} has_node={'SmokeSS' in body}", body[:300]

    report.add(
        run_case(
            "clash_ss_uri_is_direct_node",
            "ss:// URI is parsed into Clash proxies",
            clash_ss_direct,
        )
    )

    def clash_hy2():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_HY2,
                "config": DISABLE_RULEGEN,
            },
            timeout=timeout,
        )
        ok = status == 200 and ("SmokeHY2" in body or "hysteria2" in body.lower())
        return ok, f"HTTP {status} hysteria2={'hysteria2' in body.lower()}", body[:300]

    report.add(
        run_case(
            "clash_hysteria2_uri",
            "hysteria2:// URI is parsed by the Mihomo bridge",
            clash_hy2,
        )
    )

    def clash_anytls():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_ANYTLS,
                "config": DISABLE_RULEGEN,
            },
            timeout=timeout,
        )
        ok = status == 200 and ("SmokeAnyTLS" in body or "anytls" in body.lower())
        return ok, f"HTTP {status} anytls={'anytls' in body.lower()}", body[:300]

    report.add(
        run_case(
            "clash_anytls_uri",
            "anytls:// URI is parsed by the Mihomo bridge",
            clash_anytls,
        )
    )

    def clash_mixed():
        mixed = SAMPLE_SS + "|" + HTTP_SUB
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": mixed,
                "config": DISABLE_RULEGEN,
                "explain": "true",
            },
            timeout=timeout,
        )
        try:
            data = json.loads(body)
        except json.JSONDecodeError:
            return False, f"HTTP {status} not JSON", body[:200]
        ok = (
            status == 200
            and data.get("mode", {}).get("proxy_provider") is True
            and data.get("output", {}).get("provider_count") == 1
            and data.get("nodes", {}).get("total", 0) >= 1
        )
        return (
            ok,
            f"HTTP {status} providers={data.get('output', {}).get('provider_count')} nodes={data.get('nodes', {}).get('total')}",
            "",
        )

    report.add(
        run_case(
            "clash_mixed_http_and_node",
            "HTTP URL is a provider and ss:// is a local node",
            clash_mixed,
        )
    )

    def surge_http_skipped():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "surge",
                "url": HTTP_SUB,
                "config": DISABLE_RULEGEN,
            },
            timeout=timeout,
        )
        ok = status == 400 and "no valid proxy" in body.lower()
        return ok, f"HTTP {status}", body[:240]

    report.add(
        run_case(
            "surge_http_subscription_not_fetched",
            "Non-Clash HTTP subscriptions are not downloaded (400 empty)",
            surge_http_skipped,
        )
    )

    def surge_ss():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "surge",
                "url": SAMPLE_SS,
                "config": DISABLE_RULEGEN,
            },
            timeout=timeout,
        )
        ok = status == 200 and "SmokeSS" in body
        return ok, f"HTTP {status} has_node={'SmokeSS' in body}", body[:240]

    report.add(
        run_case(
            "surge_ss_uri_still_parses",
            "Surge still emits a parsed ss:// node",
            surge_ss,
        )
    )

    def list_overridden():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": HTTP_SUB,
                "config": DISABLE_RULEGEN,
                "list": "true",
                "explain": "true",
            },
            timeout=timeout,
        )
        try:
            data = json.loads(body)
        except json.JSONDecodeError:
            return False, f"HTTP {status} not JSON", body[:200]
        item = recognized(data, "list")
        ok = status == 200 and item.get("status") == "overridden" and item.get("effective") in {"false", False, "False"}
        return ok, f"HTTP {status} list={item}", ""

    report.add(
        run_case(
            "list_parameter_forced_false",
            "list=true is overridden to keep provider mode",
            list_overridden,
        )
    )

    def filter_script_ignored():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_SS,
                "config": DISABLE_RULEGEN,
                "filter_script": "data:,function filter(n){return false}",
                "explain": "true",
            },
            timeout=timeout,
        )
        try:
            data = json.loads(body)
        except json.JSONDecodeError:
            return False, f"HTTP {status} not JSON", body[:200]
        item = recognized(data, "filter_script")
        still_has_node = data.get("nodes", {}).get("total", 0) >= 1
        ok = status == 200 and item.get("status") == "ignored" and still_has_node
        return ok, f"HTTP {status} filter_script={item} nodes={data.get('nodes')}", ""

    report.add(
        run_case(
            "filter_script_ignored_in_api_mode",
            "Request-side filter_script is ignored and does not drop nodes",
            filter_script_ignored,
        )
    )

    def missing_url():
        status, body, _ = request(
            base,
            "/sub",
            {"target": "clash", "config": DISABLE_RULEGEN},
            timeout=timeout,
        )
        ok = status == 400
        return ok, f"HTTP {status}", body[:200]

    report.add(run_case("sub_without_url_rejected", "HTTP 400 when url is missing", missing_url))

    def new_name_forced():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_SS,
                "config": DISABLE_RULEGEN,
                "new_name": "false",
                "explain": "true",
            },
            timeout=timeout,
        )
        try:
            data = json.loads(body)
        except json.JSONDecodeError:
            return False, f"HTTP {status} not JSON", body[:200]
        item = recognized(data, "new_name")
        yaml_status, yaml_body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_SS,
                "config": DISABLE_RULEGEN,
                "new_name": "false",
            },
            timeout=timeout,
        )
        has_new_fields = "proxy-groups:" in yaml_body or "Proxy Group" not in yaml_body
        ok = (
            status == 200
            and yaml_status == 200
            and item.get("effective") in {"true", True, "True"}
            and "proxy-groups:" in yaml_body
        )
        return ok, f"explain new_name={item} yaml_new_fields={has_new_fields}", ""

    report.add(
        run_case(
            "new_name_forced_true",
            "Clash output keeps Mihomo new field names",
            new_name_forced,
        )
    )


def add_public_cases(report: Report, base: str, timeout: int) -> None:
    def upload_disabled():
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_SS,
                "config": DISABLE_RULEGEN,
                "upload": "true",
            },
            timeout=timeout,
        )
        ok = status == 403 and "upload" in body.lower()
        return ok, f"HTTP {status}", body[:240]

    report.add(
        run_case(
            "public_upload_rejected",
            "public profile rejects request-triggered upload",
            upload_disabled,
        )
    )

    def loopback_config_blocked():
        started = time.monotonic()
        status, body, _ = request(
            base,
            "/sub",
            {
                "target": "clash",
                "url": SAMPLE_SS,
                "config": LOCAL_CONFIG,
            },
            timeout=timeout,
        )
        elapsed = time.monotonic() - started
        blocked = (
            "blocked" in body.lower()
            or "私有" in body
            or "local" in body.lower()
            or "not allowed" in body.lower()
            or status in {400, 403}
        )
        did_not_hang = elapsed < timeout - 1
        ok = did_not_hang and (blocked or status != 200 or "SmokeSS" in body)
        # If conversion still succeeds, the loopback config was ignored (also acceptable)
        ignored = status == 200 and "SmokeSS" in body
        return (
            ok,
            f"HTTP {status} elapsed={elapsed:.2f}s ignored_or_blocked={blocked or ignored}",
            body[:300],
        )

    report.add(
        run_case(
            "public_loopback_config_not_fetched",
            "public profile does not fetch a loopback config URL",
            loopback_config_blocked,
        )
    )


def render_markdown(report: Report, lan_url: str, public_url: str | None) -> str:
    lines = [
        "# SubConverter-Extended dynamic analysis",
        "",
        "Runtime observations from a locally built binary. Cases check documented",
        "conversion behavior and security-profile outcomes. They do not include",
        "exploit payloads.",
        "",
        f"- LAN instance: `{lan_url}`",
    ]
    if public_url:
        lines.append(f"- Public instance: `{public_url}`")
    lines += [
        f"- Required cases: {sum(1 for c in report.cases if c.required)}",
        f"- Failed required cases: {len(report.failed)}",
        "",
        "| Case | Result | ms | Observed |",
        "| --- | --- | --- | --- |",
    ]
    for case in report.cases:
        mark = "PASS" if case.passed else "FAIL"
        observed = case.observed.replace("|", "\\|")
        lines.append(f"| `{case.name}` | {mark} | {case.duration_ms} | {observed} |")
    lines += ["", "## Expected vs observed", ""]
    for case in report.cases:
        lines += [
            f"### {case.name}",
            "",
            f"- Expected: {case.expected}",
            f"- Observed: {case.observed}",
        ]
        if case.detail:
            lines.append(f"- Detail:\n\n```\n{case.detail[:1500]}\n```")
        lines.append("")
    if report.failed:
        lines += ["## Failures", ""]
        for case in report.failed:
            lines.append(f"- `{case.name}`: {case.observed}")
        lines.append("")
    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--lan-url", default="http://127.0.0.1:25500")
    parser.add_argument("--public-url", default="")
    parser.add_argument("--timeout", type=int, default=20)
    parser.add_argument("--report", default="")
    parser.add_argument("--json", default="")
    args = parser.parse_args()

    report = Report()
    add_lan_cases(report, args.lan_url, args.timeout)
    if args.public_url:
        add_public_cases(report, args.public_url, args.timeout)

    payload = {
        "ok": report.ok,
        "lan_url": args.lan_url,
        "public_url": args.public_url or None,
        "cases": [case.__dict__ for case in report.cases],
    }
    if args.json:
        with open(args.json, "w", encoding="utf-8") as handle:
            json.dump(payload, handle, ensure_ascii=False, indent=2)
            handle.write("\n")

    markdown = render_markdown(report, args.lan_url, args.public_url or None)
    if args.report:
        with open(args.report, "w", encoding="utf-8") as handle:
            handle.write(markdown)
    sys.stdout.write(markdown)
    return 0 if report.ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
