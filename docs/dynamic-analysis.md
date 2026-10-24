# SubConverter-Extended dynamic analysis

Runtime observations from a locally built binary. Cases check documented
conversion behavior and security-profile outcomes. They do not include
exploit payloads.

- LAN instance: `http://127.0.0.1:25500`
- Public instance: `http://127.0.0.1:25501`
- Required cases: 19
- Failed required cases: 0

## Runtime notes

- Clash HTTP subscriptions are emitted as `proxy-providers` and are not downloaded by this process.
- Non-Clash targets skip HTTP subscriptions, so a Surge request with only an HTTP URL returns HTTP 400.
- `list` and `new_name` request values are overridden; explain JSON reports `effective_value`.
- Request-side `filter_script` is ignored because API mode is hardcoded on.
- Public profile rejects `upload=true` with HTTP 403.
- Public profile blocks loopback/private `config` hosts before curl. If that user config fails, `/sub` still tries hardcoded CDN fallback URLs as `TrustedConfig` (those fetches are not public-host-restricted). This fixture sets `max_allowed_rulesets = 8`, so a downloaded Custom_Clash.ini is then rejected for size.

| Case | Result | ms | Observed |
| --- | --- | --- | --- |
| `healthz` | PASS | 17 | HTTP 200 'ok\n' |
| `version_html` | PASS | 1 | HTTP 200 ctype=text/html; charset=utf-8 |
| `version_probe` | PASS | 1 | HTTP 200 'SubConverter-Extended dev backend' |
| `inspect_page` | PASS | 1 | HTTP 200 inspector=yes |
| `webapp_page` | PASS | 1 | HTTP 200 form=yes |
| `clash_http_is_proxy_provider` | PASS | 1 | HTTP 200 proxy-providers=True |
| `clash_http_explain_provider_mode` | PASS | 1 | HTTP 200 provider=True nodes={'insert': 0, 'direct': 0, 'total': 0} |
| `clash_ss_uri_is_direct_node` | PASS | 1 | HTTP 200 has_node=True |
| `clash_hysteria2_uri` | PASS | 1 | HTTP 200 hysteria2=True |
| `clash_anytls_uri` | PASS | 1 | HTTP 200 anytls=True |
| `clash_mixed_http_and_node` | PASS | 1 | HTTP 200 providers=1 nodes=1 |
| `surge_http_subscription_not_fetched` | PASS | 0 | HTTP 400 |
| `surge_ss_uri_still_parses` | PASS | 0 | HTTP 200 has_node=True |
| `list_parameter_forced_false` | PASS | 1 | HTTP 200 list={'name': 'list', 'present': True, 'source': 'request', 'status': 'overridden', 'value_preview': 'true', 'value_hash': 'b326b5062b', 'raw_length': 4, 'value_length': 4, 'effective_value': 'false', 'note': 'This project forces provider mode for Clash-compatible output.', 'sensitive': False} |
| `filter_script_ignored_in_api_mode` | PASS | 1 | HTTP 200 filter_script={'name': 'filter_script', 'present': True, 'source': 'request', 'status': 'ignored', 'value_preview': '[redacted]', 'value_hash': '2297c80c00', 'raw_length': 38, 'value_length': 38, 'effective_value': 'not used', 'note': 'Public requests cannot provide executable filter scripts.', 'sensitive': True} nodes={'insert': 0, 'direct': 1, 'total': 1} |
| `sub_without_url_rejected` | PASS | 0 | HTTP 400 |
| `new_name_forced_true` | PASS | 2 | explain new_name={'name': 'new_name', 'present': True, 'source': 'request', 'status': 'overridden', 'value_preview': 'false', 'value_hash': '68934a3e94', 'raw_length': 5, 'value_length': 5, 'effective_value': 'true', 'note': 'Mihomo-compatible field names are forced for Clash output.', 'sensitive': False} yaml_new_fields=True |
| `public_upload_rejected` | PASS | 1 | HTTP 403 |
| `public_loopback_config_not_fetched` | PASS | 130 | HTTP 200 elapsed=0.13s provided=True loaded=False fallback_used=False nodes=1 |

## Expected vs observed

### healthz

- Expected: HTTP 200 body ok
- Observed: HTTP 200 'ok\n'

### version_html

- Expected: HTML version page
- Observed: HTTP 200 ctype=text/html; charset=utf-8
- Detail:

```
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width
```

### version_probe

- Expected: plain-text backend probe
- Observed: HTTP 200 'SubConverter-Extended dev backend'

### inspect_page

- Expected: inspector HTML
- Observed: HTTP 200 inspector=yes

### webapp_page

- Expected: converter HTML
- Observed: HTTP 200 form=yes

### clash_http_is_proxy_provider

- Expected: Clash HTTP URL becomes proxy-providers, not fetched nodes
- Observed: HTTP 200 proxy-providers=True
- Detail:

```
proxy-providers:
  Provider_3DD403:
    type: http
    url: https://example.com/sub
    interval: 3600
    proxy: DIRECT
    path: ./providers/Provider_3DD403.yaml
    health-check:
      enable: true
      url: https://cp.cloudflare.com/generate_204
      interval: 300
proxy-groups: ~
```

### clash_http_explain_provider_mode

- Expected: explain JSON reports proxy_provider and zero local nodes
- Observed: HTTP 200 provider=True nodes={'insert': 0, 'direct': 0, 'total': 0}

### clash_ss_uri_is_direct_node

- Expected: ss:// URI is parsed into Clash proxies
- Observed: HTTP 200 has_node=True
- Detail:

```
proxies:
  - {name: SmokeSS, server: example.com, port: 8388, cipher: aes-128-gcm, password: password, type: ss, udp: true}
proxy-groups: ~
```

### clash_hysteria2_uri

- Expected: hysteria2:// URI is parsed by the Mihomo bridge
- Observed: HTTP 200 hysteria2=True
- Detail:

```
proxies:
  - {name: SmokeHY2, server: example.com, port: 443, down: "", fingerprint: "", obfs: "", obfs-password: "", password: password, skip-cert-verify: true, sni: example.com, type: hysteria2, up: ""}
proxy-groups: ~
```

### clash_anytls_uri

- Expected: anytls:// URI is parsed by the Mihomo bridge
- Observed: HTTP 200 anytls=True
- Detail:

```
proxies:
  - {name: SmokeAnyTLS, server: example.com, port: 443, fingerprint: "", password: password, skip-cert-verify: false, sni: example.com, type: anytls, udp: true, username: password}
proxy-groups: ~
```

### clash_mixed_http_and_node

- Expected: HTTP URL is a provider and ss:// is a local node
- Observed: HTTP 200 providers=1 nodes=1

### surge_http_subscription_not_fetched

- Expected: Non-Clash HTTP subscriptions are not downloaded (400 empty)
- Observed: HTTP 400
- Detail:

```
Invalid request: no valid proxy nodes or proxy providers were found.
无效请求：未找到有效的代理节点或代理提供者。
Please check whether the subscription URL or node URI format is supported, and whether filters excluded all nodes.
请检查订阅链接或节点 URI 格式是否受支持，以及过滤规则是否排除
```

### surge_ss_uri_still_parses

- Expected: Surge still emits a parsed ss:// node
- Observed: HTTP 200 has_node=True
- Detail:

```
#!MANAGED-CONFIG http://127.0.0.1:25500/sub?config=data%3A%2Cenable_rule_generator%3Dfalse&target=surge&url=ss%3A%2F%2FYWVzLTEyOC1nY206cGFzc3dvcmQ%40example.com%3A8388%23SmokeSS interval=86400 strict=false


```

### list_parameter_forced_false

- Expected: list=true is overridden to keep provider mode
- Observed: HTTP 200 list={'name': 'list', 'present': True, 'source': 'request', 'status': 'overridden', 'value_preview': 'true', 'value_hash': 'b326b5062b', 'raw_length': 4, 'value_length': 4, 'effective_value': 'false', 'note': 'This project forces provider mode for Clash-compatible output.', 'sensitive': False}

### filter_script_ignored_in_api_mode

- Expected: Request-side filter_script is ignored and does not drop nodes
- Observed: HTTP 200 filter_script={'name': 'filter_script', 'present': True, 'source': 'request', 'status': 'ignored', 'value_preview': '[redacted]', 'value_hash': '2297c80c00', 'raw_length': 38, 'value_length': 38, 'effective_value': 'not used', 'note': 'Public requests cannot provide executable filter scripts.', 'sensitive': True} nodes={'insert': 0, 'direct': 1, 'total': 1}

### sub_without_url_rejected

- Expected: HTTP 400 when url is missing
- Observed: HTTP 400
- Detail:

```
Invalid request: missing required target or url parameter.
无效请求：缺少必需的 target 或 url 参数。
Please provide target and url; url may be omitted only when configured insert nodes are enabled.
请提供 target 和 url
```

### new_name_forced_true

- Expected: Clash output keeps Mihomo new field names
- Observed: explain new_name={'name': 'new_name', 'present': True, 'source': 'request', 'status': 'overridden', 'value_preview': 'false', 'value_hash': '68934a3e94', 'raw_length': 5, 'value_length': 5, 'effective_value': 'true', 'note': 'Mihomo-compatible field names are forced for Clash output.', 'sensitive': False} yaml_new_fields=True

### public_upload_rejected

- Expected: public profile rejects request-triggered upload
- Observed: HTTP 403
- Detail:

```
Upload is disabled for the current security profile.
当前安全档位已禁用公开请求上传。
Use security.profile=lan for private deployments, or explicitly enable security.allow_public_upload in public profile.
内网私有部署请使用 security.profile=lan；公网档位如确需上传，请显式开启 secu
```

### public_loopback_config_not_fetched

- Expected: public profile does not use a loopback config URL as the loaded config
- Observed: HTTP 200 elapsed=0.13s provided=True loaded=False fallback_used=False nodes=1
- Detail:

```
{"ok":true,"status_code":200,"requested_target":"clash","target":"clash","mode":{"simple_subscription":false,"proxy_provider":false,"nodelist":false,"expand_rulesets":false,"rule_generator":false,"managed_config":true,"upload_requested":false,"upload_suppressed":false},"inputs":{"raw_url_count":1,"insert_url_count":0,"subscription_url_count":0,"node_link_count":1,"unknown_node_link_count":0},"exte
```

