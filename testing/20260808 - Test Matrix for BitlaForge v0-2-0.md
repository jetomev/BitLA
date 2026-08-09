# Test Matrix — BitlaForge v0.2.0 (forgekit adoption)

**Date:** 2026-08-08 · **Scope:** the full user-facing surface after the shell
replacement + design simplification. Two legs: an automated headless pilot
suite (new this release) and the live field test on Javier's desktop.

## Leg 1 — headless pilot smoke suite (automated)

Script: session scratchpad `bitla_smoke.py` (Textual `run_test`, 120×40).

| # | Check | Expected |
|---|-------|----------|
| 1.1 | App composes on forgekit shell | initial section = `sec-dashboard` |
| 1.2 | Keys `2` / `3` / `1` | switcher lands on log / config / dashboard |
| 1.3 | Dashboard buttons exist | `#btn-miner-toggle` label **Start Miner**; `#btn-test-miner` present |
| 1.4 | Help menu → `i` | `InstallSetupDialog` opens; Esc closes |
| 1.5 | `?` toggle | ShortcutsDialog opens; second `?` closes it |
| 1.6 | Help menu → `a` | AboutDialog opens; Esc closes |
| 1.7 | Config save flow | pool+wallet filled → `S` → kit ConfirmDialog appears; Esc cancels |
| 1.8 | `M` with no minerd | guard path notifies, no crash |

## Leg 2 — field test (Javier, live desktop)

| # | Check | Expected |
|---|-------|----------|
| 2.1 | Launch `PYTHONPATH=~/Programs/forgekit python main.py` | forgekit shell: title, menu bar, Catppuccin |
| 2.2 | Menu bar mouse + Ctrl accelerators | all five entries respond |
| 2.3 | Dashboard: no Quick Actions text; buttons under Session | per the design ruling |
| 2.4 | Start Miner button | miner starts; button flips to red **Stop Miner**; stats + uptime tick |
| 2.5 | Stop Miner button | clean stop; button back to blue **Start Miner** |
| 2.6 | Test Miner button | `minerd --version` toast |
| 2.7 | Log section | stream + `/` filter + `C` clear unchanged |
| 2.8 | Config: helper text gone; Save → confirm → persists | round-trip to config.toml |
| 2.9 | Help → Install & Setup window | system info + minerd status + providers, fresh on open |
| 2.10 | Shortcuts / License / About windows | kit panels, compact buttons, themed scrollbars |
| 2.11 | Muscle memory: 1-3 / M / R / ? / q | all fire |
