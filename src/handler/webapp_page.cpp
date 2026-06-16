#include "handler/webapp_page.h"

#include <string>

#include "handler/settings.h"
#include "version.h"

namespace webapp_page {

std::string page(Request &, Response &response) {
  response.headers["X-Robots-Tag"] =
      "noindex, nofollow, noarchive, nosnippet, noimageindex";

  std::string dashboard_link =
      global.statisticsEnabled
          ? R"html(
                <a class="nav-link" href="/dashboard">
                    <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 19V5"></path><path d="M4 19h16"></path><path d="M8 16v-5"></path><path d="M13 16V8"></path><path d="M18 16v-3"></path></svg>
                    <span data-lang="en">Dashboard</span>
                    <span data-lang="zh">仪表盘</span>
                </a>)html"
          : "";

  return R"html(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="robots" content="noindex, nofollow, noarchive, nosnippet, noimageindex">
    <meta name="color-scheme" content="light dark">
    <title>SubConverter-Extended</title>
    <script>
        (function () {
            var saved = localStorage.getItem("sce-webapp-lang");
            if (saved) { document.documentElement.lang = saved; return; }
            var languages = navigator.languages && navigator.languages.length ? navigator.languages : [navigator.language || ""];
            document.documentElement.lang = languages.some(function(l) { return /^zh\b/i.test(l); }) ? "zh-CN" : "en";
        })();
    </script>
    <link rel="icon" type="image/svg+xml" href="/version/favicon-dark.svg">
    <link rel="icon" type="image/svg+xml" href="/version/favicon-light.svg" media="(prefers-color-scheme: light)">
    <link rel="icon" type="image/svg+xml" href="/version/favicon-dark.svg" media="(prefers-color-scheme: dark)">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@400;500;600;700&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-gradient: linear-gradient(135deg, #f8fafc 0%, #eef2f7 48%, #e2e8f0 100%);
            --bg-grid: rgba(15, 23, 42, 0.055);
            --bg-sheen: linear-gradient(115deg, transparent 0%, transparent 33%, rgba(2, 132, 199, 0.08) 48%, rgba(101, 163, 13, 0.06) 60%, transparent 73%, transparent 100%);
            --surface: rgba(255, 255, 255, 0.82);
            --surface-strong: rgba(255, 255, 255, 0.92);
            --surface-border: rgba(15, 23, 42, 0.1);
            --text-primary: #1a202c;
            --text-secondary: #4a5568;
            --text-muted: #64748b;
            --accent: #2563eb;
            --accent-2: #0891b2;
            --accent-gradient: linear-gradient(135deg, #0284c7 0%, #0891b2 45%, #65a30d 100%);
            --shadow: 0 28px 70px rgba(15, 23, 42, 0.13);
            --control-bg: rgba(255, 255, 255, 0.72);
            --control-hover: rgba(255, 255, 255, 0.92);
            --control-border: rgba(26, 32, 44, 0.12);
            --success: #10b981;
            --success-bg: rgba(16, 185, 129, 0.1);
            --error: #ef4444;
            --error-bg: rgba(239, 68, 68, 0.1);
            --warn: #f59e0b;
            --warn-bg: rgba(245, 158, 11, 0.1);
        }
        @media (prefers-color-scheme: dark) {
            :root {
                --bg-gradient: linear-gradient(135deg, #05070b 0%, #0d111a 46%, #111827 100%);
                --bg-grid: rgba(148, 163, 184, 0.075);
                --bg-sheen: linear-gradient(115deg, transparent 0%, transparent 31%, rgba(56, 189, 248, 0.1) 47%, rgba(59, 130, 246, 0.08) 60%, transparent 74%, transparent 100%);
                --surface: rgba(15, 23, 42, 0.72);
                --surface-strong: rgba(21, 29, 43, 0.9);
                --surface-border: rgba(148, 163, 184, 0.18);
                --text-primary: #f8f9fa;
                --text-secondary: #a0aec0;
                --text-muted: #7f8ea3;
                --accent: #38bdf8;
                --accent-2: #22d3ee;
                --accent-gradient: linear-gradient(135deg, #38bdf8 0%, #22d3ee 42%, #84cc16 100%);
                --shadow: 0 32px 80px rgba(0, 0, 0, 0.62);
                --control-bg: rgba(20, 24, 33, 0.7);
                --control-hover: rgba(35, 42, 56, 0.86);
                --control-border: rgba(255, 255, 255, 0.16);
                --success: #34d399;
                --success-bg: rgba(52, 211, 153, 0.15);
                --error: #f87171;
                --error-bg: rgba(248, 113, 113, 0.15);
                --warn: #fbbf24;
                --warn-bg: rgba(251, 191, 36, 0.15);
            }
        }
        html[lang^="zh"] [data-lang="en"],
        html:not([lang^="zh"]) [data-lang="zh"] { display: none; }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: 'Outfit', system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", "Microsoft YaHei", "PingFang SC", "Noto Sans CJK SC", sans-serif;
            min-height: 100vh;
            color: var(--text-primary);
            background: var(--bg-gradient);
            background-attachment: fixed;
            -webkit-font-smoothing: antialiased;
            position: relative;
        }
        body::before, body::after {
            content: ""; position: fixed; inset: 0; pointer-events: none;
        }
        body::before {
            background-image: linear-gradient(var(--bg-grid) 1px, transparent 1px), linear-gradient(90deg, var(--bg-grid) 1px, transparent 1px);
            background-size: 36px 36px;
            mask-image: linear-gradient(to bottom, transparent 0%, #000 15%, #000 85%, transparent 100%);
            -webkit-mask-image: linear-gradient(to bottom, transparent 0%, #000 15%, #000 85%, transparent 100%);
            opacity: 0.6;
        }
        body::after { background: var(--bg-sheen); opacity: 0.85; }
        .shell {
            position: relative; z-index: 1;
            width: min(920px, calc(100% - 32px));
            margin: 0 auto;
            padding: 28px 0 48px;
        }
        /* Header */
        .topbar {
            display: flex; align-items: center; justify-content: space-between;
            gap: 16px; margin-bottom: 24px; flex-wrap: wrap;
        }
        .brand { display: flex; align-items: center; gap: 14px; }
        .brand-icon {
            width: 48px; height: 48px;
            background: var(--accent-gradient);
            border-radius: 14px;
            display: flex; align-items: center; justify-content: center;
            box-shadow: 0 8px 24px rgba(2, 132, 199, 0.25);
        }
        .brand-icon svg { width: 28px; height: 28px; stroke: white; stroke-width: 2; fill: none; }
        .brand-text h1 {
            font-size: 1.5rem; font-weight: 700; line-height: 1.2;
            background: linear-gradient(135deg, var(--text-primary) 0%, var(--accent) 100%);
            -webkit-background-clip: text; -webkit-text-fill-color: transparent; background-clip: text;
        }
        .brand-text p { font-size: 0.85rem; color: var(--text-secondary); font-weight: 500; margin-top: 2px; }
        .topbar-actions { display: flex; align-items: center; gap: 10px; flex-wrap: wrap; justify-content: flex-end; }
        .nav-link {
            display: inline-flex; align-items: center; gap: 6px;
            padding: 8px 14px; border-radius: 999px;
            background: var(--control-bg); border: 1px solid var(--control-border);
            color: var(--text-primary); text-decoration: none;
            font-size: 0.85rem; font-weight: 600;
            backdrop-filter: blur(12px); -webkit-backdrop-filter: blur(12px);
            transition: all 0.2s ease;
        }
        .nav-link:hover { background: var(--control-hover); transform: translateY(-1px); }
        .nav-link svg { width: 16px; height: 16px; stroke: currentColor; fill: none; }
        .lang-btn {
            min-width: 44px; height: 36px; padding: 0 12px;
            border-radius: 999px; border: 1px solid var(--control-border);
            background: var(--control-bg); color: var(--text-primary);
            font-size: 0.8rem; font-weight: 700; cursor: pointer;
            backdrop-filter: blur(12px); -webkit-backdrop-filter: blur(12px);
            transition: all 0.2s ease;
        }
        .lang-btn:hover { background: var(--control-hover); }
        /* Quick Actions */
        .quick-actions {
            display: flex; gap: 8px; flex-wrap: wrap; margin-bottom: 20px;
        }
        .quick-btn {
            display: inline-flex; align-items: center; gap: 6px;
            padding: 8px 14px; border-radius: 999px;
            background: var(--surface); border: 1px solid var(--surface-border);
            color: var(--text-secondary); text-decoration: none;
            font-size: 0.82rem; font-weight: 600;
            backdrop-filter: blur(12px); -webkit-backdrop-filter: blur(12px);
            transition: all 0.2s ease; cursor: pointer;
        }
        .quick-btn:hover {
            background: var(--surface-strong); color: var(--accent);
            border-color: var(--accent); transform: translateY(-1px);
        }
        .quick-btn svg { width: 14px; height: 14px; stroke: currentColor; fill: none; stroke-width: 2; }
        /* Panel */
        .panel {
            background: var(--surface); border: 1px solid var(--surface-border);
            box-shadow: var(--shadow); border-radius: 24px;
            backdrop-filter: blur(24px); -webkit-backdrop-filter: blur(24px);
            overflow: hidden;
        }
        /* Tabs */
        .tabs {
            display: flex; border-bottom: 1px solid var(--surface-border);
            background: rgba(255,255,255,0.3);
        }
        @media (prefers-color-scheme: dark) { .tabs { background: rgba(255,255,255,0.03); } }
        .tab {
            flex: 1; padding: 16px 20px; border: none; background: none;
            color: var(--text-muted); font: inherit; font-size: 0.9rem; font-weight: 600;
            cursor: pointer; position: relative; transition: color 0.2s;
        }
        .tab:hover { color: var(--text-primary); }
        .tab.active { color: var(--accent); }
        .tab.active::after {
            content: ""; position: absolute; bottom: -1px; left: 0; right: 0;
            height: 2px; background: var(--accent-gradient);
        }
        .tab-content { display: none; padding: 24px; }
        .tab-content.active { display: block; }
        /* Form */
        .form-grid {
            display: grid; gap: 20px;
        }
        .form-row { display: grid; grid-template-columns: 1fr 1fr; gap: 16px; }
        @media (max-width: 640px) { .form-row { grid-template-columns: 1fr; } }
        .field { display: flex; flex-direction: column; gap: 6px; }
        .field.full { grid-column: 1 / -1; }
        .field label {
            font-size: 0.8rem; font-weight: 700; color: var(--text-secondary);
            text-transform: uppercase; letter-spacing: 0.04em;
        }
        .field input, .field select, .field textarea {
            font-family: inherit; font-size: 0.9rem; padding: 12px 14px;
            background: var(--control-bg); border: 1px solid var(--control-border);
            border-radius: 12px; color: var(--text-primary); outline: none;
            transition: border-color 0.2s, box-shadow 0.2s;
        }
        .field textarea { min-height: 100px; resize: vertical; line-height: 1.5; }
        .field input:focus, .field select:focus, .field textarea:focus {
            border-color: var(--accent);
            box-shadow: 0 0 0 3px rgba(37, 99, 235, 0.12);
        }
        .field input::placeholder, .field textarea::placeholder { color: var(--text-muted); }
        .field .hint { font-size: 0.75rem; color: var(--text-muted); margin-top: 2px; }
        .field .error-hint { color: var(--error); }
        .field input.error, .field textarea.error { border-color: var(--error); }
        /* Checkboxes */
        .checkbox-grid {
            display: grid; grid-template-columns: repeat(auto-fill, minmax(160px, 1fr)); gap: 8px;
        }
        .checkbox-field {
            display: flex; align-items: center; gap: 8px;
            padding: 10px 12px; border-radius: 10px;
            background: var(--control-bg); border: 1px solid var(--control-border);
            cursor: pointer; transition: all 0.15s;
        }
        .checkbox-field:hover { background: var(--control-hover); border-color: var(--accent); }
        .checkbox-field input { width: 16px; height: 16px; accent-color: var(--accent); cursor: pointer; }
        .checkbox-field span { font-size: 0.82rem; font-weight: 500; color: var(--text-primary); }
        /* Buttons */
        .btn-primary {
            display: flex; align-items: center; justify-content: center; gap: 10px;
            width: 100%; padding: 16px 24px; margin-top: 24px;
            background: var(--accent-gradient); border: none; border-radius: 14px;
            color: white; font: inherit; font-size: 1rem; font-weight: 700;
            cursor: pointer; transition: all 0.2s;
            box-shadow: 0 8px 24px rgba(2, 132, 199, 0.3);
        }
        .btn-primary:hover { opacity: 0.92; transform: translateY(-1px); box-shadow: 0 12px 32px rgba(2, 132, 199, 0.35); }
        .btn-primary:active { transform: scale(0.98); }
        .btn-primary:disabled { opacity: 0.5; cursor: not-allowed; transform: none; }
        .btn-primary svg { width: 20px; height: 20px; stroke: currentColor; fill: none; }
        .btn-secondary {
            display: inline-flex; align-items: center; gap: 6px;
            padding: 8px 16px; border-radius: 10px;
            background: var(--surface); border: 1px solid var(--surface-border);
            color: var(--text-primary); font-size: 0.82rem; font-weight: 600;
            cursor: pointer; transition: all 0.15s;
        }
        .btn-secondary:hover { background: var(--surface-strong); border-color: var(--accent); }
        .btn-secondary svg { width: 14px; height: 14px; stroke: currentColor; fill: none; stroke-width: 2; }
        /* Result Section */
        .result-panel {
            background: var(--surface); border: 1px solid var(--surface-border);
            box-shadow: var(--shadow); border-radius: 24px;
            backdrop-filter: blur(24px); -webkit-backdrop-filter: blur(24px);
            overflow: hidden; margin-top: 20px;
        }
        .result-header {
            display: flex; align-items: center; justify-content: space-between;
            padding: 16px 20px; border-bottom: 1px solid var(--surface-border);
            background: rgba(255,255,255,0.3);
        }
        @media (prefers-color-scheme: dark) { .result-header { background: rgba(255,255,255,0.03); } }
        .result-header h3 { font-size: 1rem; font-weight: 600; }
        .result-actions { display: flex; gap: 8px; }
        .btn-copy {
            padding: 8px 14px; border-radius: 8px;
            background: var(--accent); border: none;
            color: white; font-size: 0.82rem; font-weight: 600;
            cursor: pointer; transition: all 0.15s;
        }
        .btn-copy:hover { opacity: 0.88; }
        .btn-copy.copied { background: var(--success); }
        #result-output {
            padding: 16px 20px; max-height: 500px; overflow-y: auto;
            font-family: "SF Mono", "Cascadia Code", "Fira Code", "Consolas", monospace;
            font-size: 0.82rem; line-height: 1.6;
            white-space: pre-wrap; word-break: break-all;
            background: var(--control-bg); color: var(--text-primary);
        }
        .result-footer {
            display: flex; align-items: center; justify-content: space-between;
            padding: 12px 20px; border-top: 1px solid var(--surface-border);
            font-size: 0.78rem; color: var(--text-muted);
        }
        .result-meta { display: flex; gap: 16px; }
        .result-meta span { display: flex; align-items: center; gap: 4px; }
        /* Status Messages */
        .toast {
            position: fixed; bottom: 24px; left: 50%; transform: translateX(-50%) translateY(100px);
            padding: 14px 20px; border-radius: 12px;
            font-size: 0.88rem; font-weight: 600;
            box-shadow: 0 12px 40px rgba(0,0,0,0.2);
            z-index: 1000; opacity: 0; transition: all 0.3s ease;
            display: flex; align-items: center; gap: 10px;
        }
        .toast.show { transform: translateX(-50%) translateY(0); opacity: 1; }
        .toast svg { width: 18px; height: 18px; stroke: currentColor; fill: none; stroke-width: 2; flex-shrink: 0; }
        .toast.success { background: var(--success); color: white; }
        .toast.error { background: var(--error); color: white; }
        .toast.warn { background: var(--warn); color: #1a1a1a; }
        /* Loading */
        .spinner {
            width: 20px; height: 20px;
            border: 2px solid rgba(255,255,255,0.3);
            border-top-color: white;
            border-radius: 50%;
            animation: spin 0.7s linear infinite;
        }
        @keyframes spin { to { transform: rotate(360deg); } }
        /* History Panel */
        .history-section { margin-top: 20px; }
        .history-header {
            display: flex; align-items: center; justify-content: space-between;
            margin-bottom: 12px;
        }
        .history-header h4 { font-size: 0.9rem; font-weight: 600; color: var(--text-secondary); }
        .history-list { display: flex; flex-direction: column; gap: 8px; }
        .history-item {
            display: flex; align-items: center; gap: 12px;
            padding: 12px 14px; border-radius: 12px;
            background: var(--control-bg); border: 1px solid var(--control-border);
            cursor: pointer; transition: all 0.15s;
        }
        .history-item:hover { background: var(--control-hover); border-color: var(--accent); }
        .history-item .target-badge {
            padding: 4px 10px; border-radius: 6px;
            background: var(--accent-gradient); color: white;
            font-size: 0.72rem; font-weight: 700; text-transform: uppercase;
        }
        .history-item .url-preview {
            flex: 1; font-size: 0.82rem; color: var(--text-secondary);
            overflow: hidden; text-overflow: ellipsis; white-space: nowrap;
        }
        .history-item .time {
            font-size: 0.72rem; color: var(--text-muted);
            white-space: nowrap;
        }
        .history-item .del-btn {
            padding: 4px; border-radius: 6px; border: none;
            background: transparent; color: var(--text-muted);
            cursor: pointer; opacity: 0; transition: all 0.15s;
        }
        .history-item:hover .del-btn { opacity: 1; }
        .history-item .del-btn:hover { color: var(--error); background: var(--error-bg); }
        /* Presets */
        .presets-bar {
            display: flex; gap: 8px; margin-bottom: 16px; flex-wrap: wrap;
        }
        .preset-btn {
            padding: 6px 12px; border-radius: 8px;
            background: var(--surface); border: 1px solid var(--surface-border);
            color: var(--text-secondary); font-size: 0.78rem; font-weight: 600;
            cursor: pointer; transition: all 0.15s;
        }
        .preset-btn:hover { background: var(--surface-strong); color: var(--accent); border-color: var(--accent); }
        .preset-btn.active { background: var(--accent); color: white; border-color: var(--accent); }
        /* Footer */
        footer {
            margin-top: 32px; text-align: center;
            font-size: 0.8rem; color: var(--text-muted); line-height: 1.8;
        }
        footer a { color: var(--accent); text-decoration: none; }
        footer a:hover { text-decoration: underline; }
        /* Mobile */
        @media (max-width: 640px) {
            body { padding: 16px 12px; }
            .topbar { flex-direction: column; align-items: flex-start; }
            .topbar-actions { width: 100%; justify-content: flex-start; }
            .tabs { overflow-x: auto; }
            .tab { padding: 14px 16px; font-size: 0.85rem; white-space: nowrap; }
            .checkbox-grid { grid-template-columns: 1fr 1fr; }
            .quick-actions { overflow-x: auto; padding-bottom: 8px; flex-wrap: nowrap; }
            .quick-btn { white-space: nowrap; }
        }
    </style>
</head>
<body>
    <div class="shell">
        <!-- Header -->
        <header class="topbar">
            <div class="brand">
                <div class="brand-icon">
                    <svg viewBox="0 0 24 24"><path d="M12 2L2 7l10 5 10-5-10-5z"/><path d="M2 17l10 5 10-5"/><path d="M2 12l10 5 10-5"/></svg>
                </div>
                <div class="brand-text">
                    <h1>SubConverter-Extended</h1>
                    <p data-lang="en">Subscription Configuration Converter</p>
                    <p data-lang="zh">订阅配置转换工具</p>
                </div>
            </div>
            <div class="topbar-actions">
                <a class="nav-link" href="/version">
                    <svg viewBox="0 0 24 24" aria-hidden="true"><circle cx="12" cy="12" r="10"></circle><path d="M12 16v-4"></path><circle cx="12" cy="8" r="1"></circle></svg>
                    <span data-lang="en">Version</span><span data-lang="zh">版本</span>
                </a>
                <a class="nav-link" href="/inspect">
                    <svg viewBox="0 0 24 24" aria-hidden="true"><circle cx="11" cy="11" r="7"></circle><path d="M21 21l-4.35-4.35"></path></svg>
                    <span data-lang="en">Inspect</span><span data-lang="zh">诊断</span>
                </a>
                )html" +
            dashboard_link +
            R"html(
                <button class="lang-btn" id="lang-btn" aria-label="Toggle language">EN</button>
            </div>
        </header>

        <!-- Quick Actions -->
        <div class="quick-actions">
            <a class="quick-btn" href="/clash" title="Clash / Mihomo"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Clash</a>
            <a class="quick-btn" href="/clashr" title="ClashR"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>ClashR</a>
            <a class="quick-btn" href="/surge" title="Surge"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Surge</a>
            <a class="quick-btn" href="/quanx" title="Quantumult X"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>QuanX</a>
            <a class="quick-btn" href="/quan" title="Quantumult"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Quan</a>
            <a class="quick-btn" href="/loon" title="Loon"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Loon</a>
            <a class="quick-btn" href="/surfboard" title="Surfboard"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Surfboard</a>
            <a class="quick-btn" href="/mellow" title="Mellow"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Mellow</a>
            <a class="quick-btn" href="/singbox" title="Sing-box"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Sing-box</a>
            <a class="quick-btn" href="/ss" title="Shadowsocks"><svg viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>SS</a>
            <a class="quick-btn" href="/ssr" title="ShadowsocksR"><svg viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>SSR</a>
            <a class="quick-btn" href="/v2ray" title="V2Ray"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>V2Ray</a>
            <a class="quick-btn" href="/trojan" title="Trojan"><svg viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/></svg>Trojan</a>
            <a class="quick-btn" href="/mixed" title="Mixed"><svg viewBox="0 0 24 24"><path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/></svg>Mixed</a>
        </div>

        <!-- Main Panel -->
        <div class="panel">
            <div class="tabs">
                <button class="tab active" data-tab="basic" data-lang="en">Basic</button>
                <button class="tab active" data-tab="basic" data-lang="zh">基础</button>
                <button class="tab" data-tab="advanced" data-lang="en">Advanced</button>
                <button class="tab" data-tab="advanced" data-lang="zh">高级</button>
                <button class="tab" data-tab="history" data-lang="en">History</button>
                <button class="tab" data-tab="history" data-lang="zh">历史</button>
            </div>

            <!-- Basic Tab -->
            <div class="tab-content active" id="tab-basic">
                <form id="convert-form" autocomplete="off">
                    <!-- Presets -->
                    <div class="presets-bar">
                        <button type="button" class="preset-btn" data-preset="clash">Clash</button>
                        <button type="button" class="preset-btn" data-preset="surge">Surge</button>
                        <button type="button" class="preset-btn" data-preset="quanx">QuanX</button>
                        <button type="button" class="preset-btn" data-preset="singbox">Sing-box</button>
                        <button type="button" class="preset-btn" data-preset="nodes">🔗 <span data-lang="en">Node List</span><span data-lang="zh">节点列表</span></button>
                    </div>

                    <div class="form-grid">
                        <div class="form-row">
                            <div class="field">
                                <label for="target" data-lang="en">Target Format</label>
                                <label for="target" data-lang="zh">目标格式</label>
                                <select id="target" name="target">
                                    <option value="clash" selected>Clash / Mihomo</option>
                                    <option value="clashr">ClashR</option>
                                    <option value="surge">Surge</option>
                                    <option value="quanx">Quantumult X</option>
                                    <option value="quan">Quantumult</option>
                                    <option value="loon">Loon</option>
                                    <option value="surfboard">Surfboard</option>
                                    <option value="mellow">Mellow</option>
                                    <option value="singbox">Sing-box</option>
                                    <option value="ss">SS (Simple)</option>
                                    <option value="ssr">SSR</option>
                                    <option value="v2ray">V2Ray</option>
                                    <option value="trojan">Trojan</option>
                                    <option value="mixed">Mixed</option>
                                </select>
                            </div>
                            <div class="field">
                                <label for="emoji" data-lang="en">Emoji</label>
                                <label for="emoji" data-lang="zh">Emoji</label>
                                <select id="emoji" name="emoji">
                                    <option value="" data-lang="en">Default</option><option value="" data-lang="zh">默认</option>
                                    <option value="true" data-lang="en">Enabled</option><option value="true" data-lang="zh">启用</option>
                                    <option value="false" data-lang="en">Disabled</option><option value="false" data-lang="zh">禁用</option>
                                </select>
                            </div>
                        </div>

                        <div class="field full">
                            <label for="url" data-lang="en">Subscription URL(s)</label>
                            <label for="url" data-lang="zh">订阅地址（多个用 | 分隔）</label>
                            <textarea id="url" name="url" placeholder="https://example.com/sub&#10;vless://...|https://example2.com/sub" required></textarea>
                            <span class="hint" data-lang="en">One or more URLs separated by |. Supports provider: prefix.</span>
                            <span class="hint" data-lang="zh">支持多个链接，用 | 分隔。支持 provider: 前缀。</span>
                        </div>

                        <div class="form-row">
                            <div class="field">
                                <label for="include" data-lang="en">Include (regex)</label>
                                <label for="include" data-lang="zh">包含（正则）</label>
                                <input id="include" name="include" placeholder="香港|台湾">
                            </div>
                            <div class="field">
                                <label for="exclude" data-lang="en">Exclude (regex)</label>
                                <label for="exclude" data-lang="zh">排除（正则）</label>
                                <input id="exclude" name="exclude" placeholder="过期|剩余">
                            </div>
                        </div>

                        <div class="field full">
                            <label for="config" data-lang="en">External Config (URL)</label>
                            <label for="config" data-lang="zh">外部配置文件（URL）</label>
                            <input id="config" name="config" type="url" placeholder="https://example.com/config.ini">
                            <span class="hint" data-lang="en">Optional. Your default config will be used if empty.</span>
                            <span class="hint" data-lang="zh">可选。留空则使用默认配置文件。</span>
                        </div>
                    </div>

                    <button type="submit" class="btn-primary" id="convert-btn">
                        <svg viewBox="0 0 24 24"><path d="M5 12h14M12 5l7 7-7 7"/></svg>
                        <span data-lang="en">Convert</span><span data-lang="zh">转换</span>
                    </button>
                </form>
            </div>

            <!-- Advanced Tab -->
            <div class="tab-content" id="tab-advanced">
                <form id="advanced-form" autocomplete="off">
                    <div class="form-grid">
                        <div class="field full">
                            <label data-lang="en">Options</label>
                            <label data-lang="zh">选项</label>
                            <div class="checkbox-grid">
                                <label class="checkbox-field">
                                    <input type="checkbox" name="list">
                                    <span data-lang="en">Node list only</span><span data-lang="zh">仅节点列表</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="sort">
                                    <span data-lang="en">Sort nodes</span><span data-lang="zh">排序节点</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="udp">
                                    <span>UDP</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="tfo">
                                    <span>TFO</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="scv">
                                    <span data-lang="en">Skip cert verify</span><span data-lang="zh">跳过证书验证</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="tls13">
                                    <span>TLS 1.3</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="new_name">
                                    <span data-lang="en">New name field</span><span data-lang="zh">新名称字段</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="append_type">
                                    <span data-lang="en">Append type</span><span data-lang="zh">附加类型</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="fdn">
                                    <span data-lang="en">Filter deprecated</span><span data-lang="zh">过滤弃用</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="expand">
                                    <span data-lang="en">Expand rulesets</span><span data-lang="zh">展开规则集</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="append_info">
                                    <span data-lang="en">Append userinfo</span><span data-lang="zh">附加用户信息</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="insert">
                                    <span data-lang="en">Insert URL</span><span data-lang="zh">插入链接</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="prepend">
                                    <span data-lang="en">Prepend insert</span><span data-lang="zh">插入到开头</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="strict">
                                    <span data-lang="en">Strict update</span><span data-lang="zh">严格更新</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="classic">
                                    <span data-lang="en">Classical rule provider</span><span data-lang="zh">经典规则提供者</span>
                                </label>
                                <label class="checkbox-field">
                                    <input type="checkbox" name="provider_proxy_direct">
                                    <span data-lang="en">Provider proxy direct</span><span data-lang="zh">Provider 直连</span>
                                </label>
                            </div>
                        </div>

                        <div class="form-row">
                            <div class="field">
                                <label for="group" data-lang="en">Group / Provider Name</label>
                                <label for="group" data-lang="zh">组 / Provider 名称</label>
                                <input id="group" name="group" placeholder="MyProvider">
                            </div>
                            <div class="field">
                                <label for="rename" data-lang="en">Rename Rules</label>
                                <label for="rename" data-lang="zh">重命名规则</label>
                                <input id="rename" name="rename" placeholder="old=New">
                            </div>
                        </div>

                        <div class="field full">
                            <label for="filter_script" data-lang="en">Filter Script URL</label>
                            <label for="filter_script" data-lang="zh">过滤脚本 URL</label>
                            <input id="filter_script" name="filter_script" type="url" placeholder="https://example.com/filter.js">
                        </div>
                    </div>
                </form>
            </div>

            <!-- History Tab -->
            <div class="tab-content" id="tab-history">
                <div class="history-section">
                    <div class="history-header">
                        <h4 data-lang="en">Recent Conversions</h4>
                        <h4 data-lang="zh">最近转换</h4>
                        <button class="btn-secondary" id="clear-history">
                            <svg viewBox="0 0 24 24"><path d="M3 6h18M19 6v14a2 2 0 01-2 2H7a2 2 0 01-2-2V6m3 0V4a2 2 0 012-2h4a2 2 0 012 2v2"/></svg>
                            <span data-lang="en">Clear</span><span data-lang="zh">清空</span>
                        </button>
                    </div>
                    <div class="history-list" id="history-list">
                        <p class="hint" style="padding: 20px; text-align: center;" data-lang="en">No conversion history yet.</p>
                        <p class="hint" style="padding: 20px; text-align: center;" data-lang="zh">暂无转换历史。</p>
                    </div>
                </div>
            </div>
        </div>

        <!-- Result Panel -->
        <div class="result-panel" id="result-panel" style="display: none;">
            <div class="result-header">
                <h3 data-lang="en">Result</h3>
                <h3 data-lang="zh">结果</h3>
                <div class="result-actions">
                    <button class="btn-copy" id="copy-btn">
                        <span data-lang="en">Copy</span><span data-lang="zh">复制</span>
                    </button>
                </div>
            </div>
            <pre id="result-output"></pre>
            <div class="result-footer">
                <div class="result-meta">
                    <span id="result-size"><span data-lang="en">Size:</span><span data-lang="zh">大小:</span> 0 B</span>
                    <span id="result-lines"><span data-lang="en">Lines:</span><span data-lang="zh">行数:</span> 0</span>
                </div>
                <span id="result-time"></span>
            </div>
        </div>

        <footer>
            <span data-lang="en">SubConverter-Extended )html" +
      std::string(VERSION) +
      R"html( · <a href="/version">Version</a> · <a href="https://github.com/Aethersailor/SubConverter-Extended" target="_blank">GitHub</a> · <a href="https://www.gnu.org/licenses/gpl-3.0.html" target="_blank">GPL-3.0</a></span>
            <span data-lang="zh">SubConverter-Extended )html" +
      std::string(VERSION) +
      R"html( · <a href="/version">版本</a> · <a href="https://github.com/Aethersailor/SubConverter-Extended" target="_blank">GitHub</a> · <a href="https://www.gnu.org/licenses/gpl-3.0.html" target="_blank">GPL-3.0</a></span>
        </footer>
    </div>

    <!-- Toast -->
    <div class="toast" id="toast">
        <svg viewBox="0 0 24 24"><path d="M20 6L9 17l-5-5"/></svg>
        <span id="toast-msg"></span>
    </div>

    <script>
    (function () {
        'use strict';
        var STORAGE_KEY_FORM = 'sce-webapp-form-v1';
        var STORAGE_KEY_HISTORY = 'sce-webapp-history-v1';
        var MAX_HISTORY = 20;

        // Elements
        var form = document.getElementById('convert-form');
        var advancedForm = document.getElementById('advanced-form');
        var resultPanel = document.getElementById('result-panel');
        var resultOutput = document.getElementById('result-output');
        var convertBtn = document.getElementById('convert-btn');
        var copyBtn = document.getElementById('copy-btn');
        var historyList = document.getElementById('history-list');
        var toast = document.getElementById('toast');
        var toastMsg = document.getElementById('toast-msg');

        // Helpers
        function isZh() { return /^zh\b/i.test(document.documentElement.lang); }
        function text(en, zh) { return isZh() ? zh : en; }
        function showToast(msg, type) {
            toast.className = 'toast ' + (type || 'success');
            toastMsg.textContent = msg;
            toast.querySelector('svg').innerHTML = type === 'error'
                ? '<path d="M18 6L6 18M6 6l12 12"/>'
                : '<path d="M20 6L9 17l-5-5"/>';
            toast.classList.add('show');
            setTimeout(function() { toast.classList.remove('show'); }, 3000);
        }
        function formatBytes(bytes) {
            if (bytes < 1024) return bytes + ' B';
            if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB';
            return (bytes / (1024 * 1024)).toFixed(1) + ' MB';
        }

        // Tab switching
        document.querySelectorAll('.tab').forEach(function(tab) {
            tab.addEventListener('click', function() {
                var target = this.getAttribute('data-tab');
                document.querySelectorAll('.tab').forEach(function(t) { t.classList.remove('active'); });
                document.querySelectorAll('.tab-content').forEach(function(c) { c.classList.remove('active'); });
                document.querySelectorAll('[data-tab="' + target + '"]').forEach(function(t) { t.classList.add('active'); });
                document.getElementById('tab-' + target).classList.add('active');
                if (target === 'history') renderHistory();
            });
        });

        // Language toggle
        document.getElementById('lang-btn').addEventListener('click', function() {
            var next = isZh() ? 'en' : 'zh-CN';
            document.documentElement.lang = next;
            localStorage.setItem('sce-webapp-lang', next);
            this.textContent = isZh() ? 'EN' : '中';
        });
        // Init lang button
        document.getElementById('lang-btn').textContent = isZh() ? 'EN' : '中';

        // Presets
        var PRESETS = {
            clash: { target: 'clash', emoji: 'true', list: false },
            surge: { target: 'surge', emoji: 'true', list: false },
            quanx: { target: 'quanx', emoji: 'true', list: false },
            singbox: { target: 'singbox', emoji: 'true', list: false },
            nodes: { target: 'clash', emoji: 'true', list: true }
        };
        document.querySelectorAll('.preset-btn[data-preset]').forEach(function(btn) {
            btn.addEventListener('click', function() {
                var preset = PRESETS[this.getAttribute('data-preset')];
                if (!preset) return;
                document.getElementById('target').value = preset.target;
                document.getElementById('emoji').value = preset.emoji;
                var listCb = advancedForm.querySelector('[name="list"]');
                if (listCb) listCb.checked = preset.list;
                document.querySelectorAll('.preset-btn').forEach(function(b) { b.classList.remove('active'); });
                this.classList.add('active');
                saveForm();
                showToast(text('Preset applied', '预设已应用'), 'success');
            });
        });

        // Form persistence
        function saveForm() {
            var data = {
                target: document.getElementById('target').value,
                emoji: document.getElementById('emoji').value,
                url: document.getElementById('url').value,
                include: document.getElementById('include').value,
                exclude: document.getElementById('exclude').value,
                config: document.getElementById('config').value,
                group: document.getElementById('group').value,
                rename: document.getElementById('rename').value,
                filter_script: document.getElementById('filter_script').value,
                checkboxes: {}
            };
            var cbs = ['list','sort','udp','tfo','scv','tls13','new_name','append_type','fdn','expand','append_info','insert','prepend','strict','classic','provider_proxy_direct'];
            cbs.forEach(function(name) {
                var el = document.querySelector('[name="' + name + '"]');
                if (el) data.checkboxes[name] = el.checked;
            });
            try { localStorage.setItem(STORAGE_KEY_FORM, JSON.stringify(data)); } catch(e) {}
        }
        function loadForm() {
            try {
                var raw = localStorage.getItem(STORAGE_KEY_FORM);
                if (!raw) return;
                var data = JSON.parse(raw);
                if (data.target) document.getElementById('target').value = data.target;
                if (data.emoji !== undefined) document.getElementById('emoji').value = data.emoji;
                if (data.url) document.getElementById('url').value = data.url;
                if (data.include) document.getElementById('include').value = data.include;
                if (data.exclude) document.getElementById('exclude').value = data.exclude;
                if (data.config) document.getElementById('config').value = data.config;
                if (data.group) document.getElementById('group').value = data.group;
                if (data.rename) document.getElementById('rename').value = data.rename;
                if (data.filter_script) document.getElementById('filter_script').value = data.filter_script;
                if (data.checkboxes) {
                    Object.keys(data.checkboxes).forEach(function(name) {
                        var el = document.querySelector('[name="' + name + '"]');
                        if (el) el.checked = data.checkboxes[name];
                    });
                }
            } catch(e) {}
        }
        // Auto-save on input
        form.addEventListener('input', saveForm);
        advancedForm.addEventListener('change', saveForm);
        loadForm();

        // History
        function getHistory() {
            try { return JSON.parse(localStorage.getItem(STORAGE_KEY_HISTORY) || '[]'); } catch(e) { return []; }
        }
        function saveHistory(history) {
            try { localStorage.setItem(STORAGE_KEY_HISTORY, JSON.stringify(history)); } catch(e) {}
        }
        function addHistory(entry) {
            var history = getHistory();
            history.unshift(entry);
            if (history.length > MAX_HISTORY) history = history.slice(0, MAX_HISTORY);
            saveHistory(history);
        }
        function renderHistory() {
            var history = getHistory();
            if (!history.length) return;
            historyList.innerHTML = history.map(function(item, i) {
                return '<div class="history-item" data-index="' + i + '">' +
                    '<span class="target-badge">' + (item.target || '?') + '</span>' +
                    '<span class="url-preview">' + (item.url || '').substring(0, 60) + (item.url && item.url.length > 60 ? '...' : '') + '</span>' +
                    '<span class="time">' + item.time + '</span>' +
                    '<button class="del-btn" data-index="' + i + '" title="' + text('Delete', '删除') + '">' +
                    '<svg width="14" height="14" viewBox="0 0 24 24" stroke="currentColor" fill="none" stroke-width="2"><path d="M18 6L6 18M6 6l12 12"/></svg>' +
                    '</button></div>';
            }).join('');
        }
        historyList.addEventListener('click', function(e) {
            var delBtn = e.target.closest('.del-btn');
            if (delBtn) {
                e.stopPropagation();
                var idx = parseInt(delBtn.getAttribute('data-index'));
                var history = getHistory();
                history.splice(idx, 1);
                saveHistory(history);
                renderHistory();
                return;
            }
            var item = e.target.closest('.history-item');
            if (item) {
                var idx = parseInt(item.getAttribute('data-index'));
                var entry = getHistory()[idx];
                if (entry) {
                    if (entry.target) document.getElementById('target').value = entry.target;
                    if (entry.url) document.getElementById('url').value = entry.url;
                    if (entry.include) document.getElementById('include').value = entry.include;
                    if (entry.exclude) document.getElementById('exclude').value = entry.exclude;
                    document.querySelectorAll('.tab[data-tab="basic"]').forEach(function(t) { t.click(); });
                    saveForm();
                    showToast(text('Config restored', '配置已恢复'), 'success');
                }
            }
        });
        document.getElementById('clear-history').addEventListener('click', function() {
            saveHistory([]);
            renderHistory();
            showToast(text('History cleared', '历史已清空'), 'success');
        });

        // URL validation
        function validateUrl(url) {
            if (!url) return true; // Empty is OK (may use default)
            var urls = url.split('|');
            var valid = true;
            urls.forEach(function(u) {
                u = u.trim();
                if (!u) return;
                // Check for valid URI or base64/vmess/trojan etc
                if (!/^(https?:\/\/|vmess:\/\/|vless:\/\/|ss:\/\/|ssr:\/\/|trojan:\/\/|hysteria2:\/\/|tuic:\/\/|wireguard:\/\/|provider:)/i.test(u)) {
                    valid = false;
                }
            });
            return valid;
        }
        document.getElementById('url').addEventListener('blur', function() {
            var url = this.value.trim();
            if (url && !validateUrl(url)) {
                this.classList.add('error');
                var hint = this.parentElement.querySelector('.error-hint') || document.createElement('span');
                hint.className = 'hint error-hint';
                hint.textContent = text('Invalid URL format', 'URL 格式无效');
                if (!this.parentElement.querySelector('.error-hint')) this.parentElement.appendChild(hint);
            } else {
                this.classList.remove('error');
                var hint = this.parentElement.querySelector('.error-hint');
                if (hint) hint.remove();
            }
        });

        // Form submit
        form.addEventListener('submit', function(e) {
            e.preventDefault();
            var urlVal = document.getElementById('url').value.trim();
            if (!urlVal) {
                showToast(text('Please enter subscription URL', '请输入订阅地址'), 'error');
                return;
            }
            if (!validateUrl(urlVal)) {
                showToast(text('Invalid URL format', 'URL 格式无效'), 'error');
                return;
            }

            var params = new URLSearchParams();
            var fd = new FormData(form);

            // Basic params
            params.set('target', fd.get('target') || 'clash');
            params.set('url', urlVal);

            var config = fd.get('config') || '';
            if (config) params.set('config', config.trim());

            var emoji = fd.get('emoji') || '';
            if (emoji) params.set('emoji', emoji);

            var include = fd.get('include') || '';
            if (include) params.set('include', include.trim());

            var exclude = fd.get('exclude') || '';
            if (exclude) params.set('exclude', exclude.trim());

            // Advanced params (from advanced form)
            var advFd = new FormData(advancedForm);
            var group = advFd.get('group') || '';
            if (group) params.set('group', group.trim());

            var rename = advFd.get('rename') || '';
            if (rename) params.set('rename', rename.trim());

            var filterScript = advFd.get('filter_script') || '';
            if (filterScript) params.set('filter_script', filterScript.trim());

            // Checkboxes
            var checkboxes = ['list','sort','udp','tfo','scv','tls13','new_name','append_type','fdn','expand','append_info','insert','prepend','strict','classic','provider_proxy_direct'];
            checkboxes.forEach(function(name) {
                var el = advancedForm.querySelector('[name="' + name + '"]');
                if (el && el.checked) params.set(name, 'true');
            });

            // UI state
            convertBtn.disabled = true;
            convertBtn.innerHTML = '<div class="spinner"></div> <span data-lang="en">Converting...</span><span data-lang="zh">转换中...</span>';
            resultPanel.style.display = 'none';

            var startTime = Date.now();
            var apiUrl = '/sub?' + params.toString();

            fetch(apiUrl)
                .then(function(resp) {
                    if (!resp.ok) {
                        return resp.text().then(function(text) {
                            throw new Error('HTTP ' + resp.status + ': ' + (text || resp.statusText));
                        });
                    }
                    return resp.text();
                })
                .then(function(text) {
                    // Show result
                    resultOutput.textContent = text;
                    resultPanel.style.display = 'block';
                    resultPanel.scrollIntoView({ behavior: 'smooth', block: 'nearest' });

                    // Stats
                    var size = new Blob([text]).size;
                    var lines = text.split('\\n').length;
                    document.getElementById('result-size').innerHTML =
                        '<span data-lang="en">Size:</span><span data-lang="zh">大小:</span> ' + formatBytes(size);
                    document.getElementById('result-lines').innerHTML =
                        '<span data-lang="en">Lines:</span><span data-lang="zh">行数:</span> ' + lines.toLocaleString();
                    var elapsed = ((Date.now() - startTime) / 1000).toFixed(1);
                    document.getElementById('result-time').textContent = text('Converted in ', '转换耗时 ') + elapsed + 's';

                    // Save to history
                    addHistory({
                        target: document.getElementById('target').value,
                        url: urlVal.substring(0, 200),
                        include: include,
                        exclude: exclude,
                        time: new Date().toLocaleString()
                    });

                    showToast(text('Conversion successful', '转换成功'), 'success');
                })
                .catch(function(err) {
                    showToast(err.message || text('Conversion failed', '转换失败'), 'error');
                })
                .finally(function() {
                    convertBtn.disabled = false;
                    convertBtn.innerHTML = '<svg viewBox="0 0 24 24"><path d="M5 12h14M12 5l7 7-7 7"/></svg> <span data-lang="en">Convert</span><span data-lang="zh">转换</span>';
                });
        });

        // Copy button
        copyBtn.addEventListener('click', function() {
            var text = resultOutput.textContent;
            if (!text) return;
            if (navigator.clipboard && navigator.clipboard.writeText) {
                navigator.clipboard.writeText(text).then(function() {
                    copyBtn.classList.add('copied');
                    copyBtn.innerHTML = '<span data-lang="en">Copied!</span><span data-lang="zh">已复制!</span>';
                    setTimeout(function() {
                        copyBtn.classList.remove('copied');
                        copyBtn.innerHTML = '<span data-lang="en">Copy</span><span data-lang="zh">复制</span>';
                    }, 2000);
                });
            }
        });

        // Quick actions sync
        document.querySelectorAll('.quick-btn[href^="/"]').forEach(function(btn) {
            btn.addEventListener('click', function(e) {
                e.preventDefault();
                var target = this.getAttribute('href').replace('/', '');
                document.getElementById('target').value = target;
                saveForm();
                window.location.href = this.getAttribute('href') + (this.search || '');
            });
        });
    })();
    </script>
</body>
</html>)html";
}

} // namespace webapp_page
