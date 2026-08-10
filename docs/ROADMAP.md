# BitlaForge — full roadmap history

*The README carries the upcoming work and the two most recent releases; everything older lives here, newest-first.*

### v0.1.3 — May 29, 2026 — **first AUR release**
- [x] `testing/RELEASE-CHECKLIST.md` + v0.1.3 Test Matrix
- [x] Man page `bitlaforge.1`
- [x] PKGBUILD with hardened headless-mount `check()` + `PYTHONDONTWRITEBYTECODE=1` defenses
- [x] **Published on the AUR** — completes the Forge suite (joining grubForge, alacrittyForge, nogForge)

### v0.1.2 — May 29, 2026
- [x] System info on Setup screen (CPU model + logical/physical cores + load avg + memory) from stdlib
- [x] Config gains **miner name** (default hostname; also used as Stratum worker name) and **niceness** (0–19, default 19)
- [x] Threads input shows "of N available" hint from `os.cpu_count()`
- [x] `miner_runner` appends `wallet.workername` and wraps with `nice -n N`
- [x] Dashboard live tick (1s `set_interval` while mining) — uptime advances smoothly between hashmeter lines
- [x] Miner name surfaced in the Dashboard title
- [x] Live `minerd` CPU% / RAM from `/proc/<pid>/stat` + `/proc/<pid>/status` — makes the niceness setting observable
- [x] Hashrate parser hotfix: integer rates, per-thread aggregate, auto-scale to Mh/s / Gh/s

### v0.1.1 — May 28, 2026
- [x] Persist config to `~/.config/bitlaforge/config.toml` (TOML)
- [x] Wire `minerd` via `asyncio.create_subprocess_exec`: spawn, parse stdout, stop cleanly
- [x] Real Dashboard fields driven by parsed minerd output (hashrate / threads / accepted / rejected / uptime)
- [x] Stream minerd stdout into the Log screen's bounded buffer
- [x] Runtime `which("minerd")` check + Dashboard banner + friendly install guidance
- [x] Setup screen with AUR provider list + `minerd --version` self-test

### v0.1.0 — May 28, 2026
- [x] Pivot from Qt6/Widgets `BitLA` to Textual TUI under the Forge suite
- [x] Sidebar navigation + 3 screens (Dashboard, Log, Config)
- [x] `StatusMixin`, `HelpScreen`, `ConfirmDialog` ported from the Forge baseline
- [x] Catppuccin Mocha styling

---
