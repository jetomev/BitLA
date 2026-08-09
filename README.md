# ⚡ BitlaForge

> A terminal UI for running solo Bitcoin mining as what it really is — a *lottery*. Wraps `minerd`, watches it work, doesn't pretend the odds are anything other than astronomical.

![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)
![Platform: Linux](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)
![Python: 3.11+](https://img.shields.io/badge/Python-3.11+-green.svg)
![Status: Alpha](https://img.shields.io/badge/Status-Alpha-orange.svg)
![Version: 0.2.1](https://img.shields.io/badge/Version-0.2.1-purple.svg)
[![AUR](https://img.shields.io/aur/version/bitlaforge)](https://aur.archlinux.org/packages/bitlaforge)

---

## Why BitlaForge?

Solo Bitcoin mining at today's network difficulty is, statistically, **a lottery**. The chance of finding a block on a single CPU in any given minute is so small it's barely a number. But unlike a pool, if you *do* hit, you take the entire block reward — no per-share split. That's the wager: tiny odds, maximum payout, and a long-running process turning electricity into hashes while you wait.

BitlaForge is the dashboard you watch while your machine buys lottery tickets in compute cycles. It launches `minerd`, parses what it says, and shows you the pool, wallet, threads, hashrate, uptime, and shares without you having to keep a terminal window staring at raw output. It also runs cleanly over SSH, which is what you actually want for a headless rig.

It is the fourth tool in the **Forge suite** for KognogOS — alongside [grubForge](https://github.com/jetomev/grubforge), [alacrittyForge](https://github.com/jetomev/alacrittyforge), and nogForge. Same Catppuccin Mocha aesthetic, same release discipline, same human + AI co-authorship.

---

## Project state

**v0.2.0 — the first Forge app on [forgekit](https://github.com/jetomev/forgekit).** The hand-rolled sidebar / header / footer / help chrome is gone, replaced by the shared Forge Suite shell: a top **menu bar** with underlined accelerators (mouse and keyboard), a full-width workspace, and the kit's floating windows for Help. The redesign also simplified the surface: **three sections** (Dashboard / Log / Config), a two-state **Start Miner / Stop Miner** button plus **Test Miner** right on the Dashboard, and the old Setup screen reborn as a read-only **Help → Install & Setup** window. The whole miner engine (subprocess lifecycle, live stats tick, log streaming, TOML persistence) carried over from v0.1.x unchanged — net −400 lines.

> This repo used to be `BitLA`, a Qt6/Widgets desktop scaffold that landed in November 2025 with simulation-driven UI and no real miner integration. On 2026-05-28 it pivoted to a Textual TUI under the Forge-suite umbrella. The Qt prototype is preserved permanently as the `v0.1.0-qt-archived` git tag; `main` is the TUI from day one.

---

## Features (v0.2.0)

- 🏠 **Dashboard** — live overview driven by parsed minerd output: pool / port / wallet / algorithm / threads / hashrate / accepted / rejected shares / uptime, with per-process CPU and RAM sampled from `/proc`. Two action buttons: **Start Miner** (flips to a red **Stop Miner** while running) and **Test Miner** (`minerd --version` → toast). Persistent **⚠ minerd not detected** banner when the binary isn't on PATH.
- 📜 **Log** — streaming view of minerd's stdout/stderr line-by-line, with a 5,000-line bounded buffer, **/** to focus search, **C** to clear.
- ⚙ **Config** — pool URL, wallet, algorithm (sha256d / scrypt / yescrypt / x11 / x13 / x15 / x17 / groestl), thread count, niceness, miner name. **Persisted** to `~/.config/bitlaforge/config.toml`; loads at every launch.
- 🧭 **forgekit shell** — the Forge Suite menu bar (Dashboard / Log / Config / Help / Quit) with underlined accelerators, Catppuccin Mocha throughout, themed slim scrollbars, compact one-row buttons.
- 🪟 **Help windows** — Shortcuts, **Install & Setup** (system info + minerd status + AUR providers + paths, rebuilt fresh on every open), License, About — all floating forgekit panels.
- 💬 **Unified feedback** — `StatusMixin` from the Forge suite: status-line + toast popup, `popup=False` for passive mount hints so launch is quiet.
- 🎯 **Focus-on-show** — section bindings fire on the first keypress without a panel click.

---

## Requirements

- Linux
- Python 3.11+
- `python-textual`, `python-rich`, `python-tomli-w`
- [`forgekit`](https://github.com/jetomev/forgekit) ≥ 0.2.1 — the shared Forge Suite TUI shell (GitHub; packaging for AUR arrives when AUR submissions reopen)
- `minerd` — optional but required to actually mine. AUR-only; install via one of `cpuminer` (recommended, pooler's original), `cpuminer-multi`, or `cpuminer-opt`. **Help → Install & Setup** inside the app has the install commands, and **Test Miner** on the Dashboard verifies the binary.

---

## Installation

### Arch Linux — AUR (recommended)
```bash
yay -S bitlaforge
```
Then run `bitlaforge`. The AUR package's `optdepends` will prompt for one of the `cpuminer*` variants to actually mine — install whichever fits your needs (see **Help → Install & Setup** for guidance).

### Arch Linux — from source
```bash
sudo pacman -S python-textual python-rich python-tomli-w
git clone https://github.com/jetomev/forgekit.git
git clone https://github.com/jetomev/bitlaforge.git
cd bitlaforge
PYTHONPATH=../forgekit python main.py
```

### Other distributions
```bash
pip install textual rich tomli-w git+https://github.com/jetomev/forgekit
git clone https://github.com/jetomev/bitlaforge.git
cd bitlaforge
python main.py
```

### `minerd` itself (required to actually mine)
The `minerd` binary lives only on the AUR — three providers, pick one. Open **Help → Install & Setup** inside BitlaForge for the full guide; the short version:

```bash
yay -S cpuminer          # pooler's original (recommended)
# or
yay -S cpuminer-multi    # multi-algorithm fork
# or
yay -S cpuminer-opt      # heavily optimised variant
```

If `minerd` is missing, BitlaForge still runs — the Dashboard shows a banner, **Help → Install & Setup** has the install commands, and **Start Miner** surfaces install-guidance instead of failing silently.

---

## Keybindings

### Global
| Key | Action |
|-----|--------|
| `1` | Dashboard |
| `2` | Log |
| `3` | Config |
| `M` | Start / Stop miner (same as the Dashboard button) |
| `T` | Test the miner (`minerd --version` → toast) |
| `R` | Refresh current section |
| `?` or `Ctrl+H` | Toggle the Shortcuts window |
| `q` | Quit |
| `Ctrl+…` | Menu-bar accelerators — the underlined letter opens each menu/section; inside a dropdown, an item's underlined letter picks it |

### Log section
| Key | Action |
|-----|--------|
| `/` | Focus the search filter |
| `C` | Clear the log buffer |

### Config section
| Key | Action |
|-----|--------|
| `E` | Focus the first input (begin editing) |
| `S` | Save the current values |

---

## Project Structure

```
bitlaforge/
├── main.py                                # Entry point
├── bitlaforge/
│   ├── app.py                             # BitlaForgeApp on forgekit.ForgeApp — menu, miner lifecycle, live tick
│   ├── setup_info.py                      # Body of the Help → Install & Setup window
│   ├── config_manager.py                  # TOML read/write at ~/.config/bitlaforge/
│   ├── miner_runner.py                    # Async minerd subprocess + stdout parsing
│   ├── process_stats.py                   # /proc/<pid>/ CPU% + RAM readouts
│   ├── system_info.py                     # /proc/cpuinfo + /proc/meminfo + load avg
│   ├── screens/
│   │   ├── dashboard.py                   # Live miner overview + Start/Stop + Test buttons
│   │   ├── log.py                         # 5,000-line bounded buffer + filter
│   │   └── config.py                      # Pool / wallet / algorithm / threads / name / niceness
│   └── widgets/
│       └── status.py                      # StatusMixin (line + toast)
```

The shell chrome (menu bar, section switcher, dialogs, theme, scrollbars) lives in
[forgekit](https://github.com/jetomev/forgekit) — shared across the Forge Suite.

---

## Safety philosophy

Mining is **opt-in**. Real CPU load, real electricity, real heat. BitlaForge will never:

- Auto-start `minerd` on launch. The miner only runs after you explicitly press **Start Miner** (or **M**), and only if a pool + wallet are configured.
- Hide the active state. The Dashboard always shows whether the miner is running — including the button itself, which reads **Stop Miner** in red while it is.
- Make the Stop path more than one action away. The same button (or **M**) stops it. Always.

---

## Roadmap

### Future
- [ ] Pool reachability check on save (not just on start)
- [ ] Multi-config profiles (switch between pools / wallets / algorithms with one key)
- [ ] Optional auto-restart on minerd crash
- [ ] Notification on accepted-share (rare event, worth surfacing prominently)

### v0.3.0 — Planned (visual identity upgrade)
- [ ] Sparkline (`▁▂▃▄▅▆▇█`) of hashrate-over-time under the Dashboard's hashrate value
- [ ] Per-thread hashrate mini-bars
- [ ] Optional `textual-plotext` integration for proper time-series charts (btop-style)

### v0.2.1 — August 9, 2026 (current) — window-polish batch
- [x] Shortcuts window rewritten to the combined-key spec (`Ctrl+D or 1` …) with `Esc` / menu-navigation entries; key column auto-aligns (kit F-6)
- [x] **T** key restored: keyboard twin of the Dashboard's Test Miner button; **Ctrl+H** now toggles Shortcuts directly
- [x] Windows hug their content (kit F-7) and buttons live in a thin **fixed footer under a divider** — always visible even when Install & Setup scrolls (kit F-8; the ruling that reshaped every Forge dialog to come)
- [x] Requires forgekit ≥ 0.2.1

### v0.2.0 — August 8, 2026 — **first Forge app on forgekit**
- [x] Shell replaced by [forgekit](https://github.com/jetomev/forgekit) `ForgeApp` — menu bar with accelerators, section switcher, Help windows, Catppuccin theme, themed scrollbars, compact buttons (net −400 lines)
- [x] Design simplification (Javier's field review): three sections; **Start/Stop Miner** two-state button + **Test Miner** on the Dashboard; Setup screen → read-only **Help → Install & Setup** window; Miner menu and key legends retired
- [x] Muscle memory preserved: `1-3`, `M`, `R`, `?`, `q`
- [x] Headless pilot smoke suite (sections, dialogs, confirm flow, miner guard)
- Found for forgekit: modal dialogs swallow app-level character keys → apps needing extra close keys must subclass (kit finding #5, queued)

### Planned — next feature cycle (was v0.1.4)
- [ ] Wallet format validation (bech32 / legacy address shape check)
- [ ] Pool reachability probe (TCP connect with short timeout) + "Test connection" on Config
- [ ] Persistent log archive (rotating files in `~/.local/share/bitlaforge/sessions/`)
- [ ] Per-session lifetime totals across restarts (uptime + accepted/rejected accumulated)

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

## Changelog

### v0.2.1 — August 9, 2026

**Window-polish batch** — the second same-week field review, all landed in the right layers:

- **Shortcuts window**: content rewritten to Javier's spec — combined keys per line (`Ctrl+D or 1  Dashboard`), `T — Test the miner`, `Esc`, `Enter/↑↓` menu navigation; the key column auto-sizes (forgekit F-6).
- **Keys**: `T` restored as the keyboard twin of Test Miner; `Ctrl+H` toggles the Shortcuts window directly.
- **Window anatomy** (forgekit F-7 + F-8): panels hug their content instead of a fixed 80% height, long bodies (Install & Setup) cap and scroll, and **buttons moved to a thin fixed footer under a divider** — the Close button can never scroll out of view again. Footer kept tight: divider + button row + one breath of air.
- Requires **forgekit ≥ 0.2.1**.

### v0.2.0 — August 8, 2026

**The forgekit adoption** — BitlaForge becomes the first Forge Suite app on the shared TUI shell, and the release that road-tested forgekit 0.2.0 itself.

- **New shell**: `BitlaForgeApp` now subclasses `forgekit.ForgeApp`. The hand-rolled sidebar, `Header`, `Footer`, `HelpScreen`, and `ConfirmDialog` (~600 lines with their CSS) are deleted; the menu bar (`Dashboard  Log  Config  Help  Quit`), section switcher, floating Help windows, Catppuccin stylesheet, slim themed scrollbars, and compact one-row buttons all come from the kit.
- **Design simplification** (field review, same day): the Dashboard's "Quick Actions" key legend is replaced by real buttons — a two-state **Start Miner / Stop Miner** (label and color track the live subprocess) and **Test Miner** (`minerd --version` → toast). The Setup screen is retired as a section and reborn as **Help → Install & Setup**, a read-only floating window rebuilt on every open. The Config screen's helper text is gone. The Miner menu idea was cut in the same review — actions live where the state lives.
- **Kept**: every line of the miner engine (`miner_runner`, `process_stats`, `system_info`, `config_manager`), the Log section verbatim, `StatusMixin`, and the `1-3 / M / R / ? / q` muscle memory.
- **New dependency**: [forgekit ≥ 0.2.0](https://github.com/jetomev/forgekit) (GitHub; AUR packaging lands when AUR submissions reopen).
- **Testing**: new headless pilot smoke suite + live field test (mining session, both button states, all windows).
- Net: **−406 lines** while gaining the suite look.

### v0.1.3 — May 29, 2026
**First AUR release — Forge release machinery.**

The version that gets BitlaForge into the AUR alongside its three Forge siblings. With this release, the **complete Forge suite** (grubForge + alacrittyForge + nogForge + BitlaForge) is now installable on Arch via a single `yay -S <name>`.

No code-feature changes vs. v0.1.2 — pure release machinery + packaging:

- **`testing/`** — `RELEASE-CHECKLIST.md` mirroring the grubForge / alacrittyForge discipline (pre-dogfood snapshot, async-worker audit, `_render` shadowing audit, version sync gate, doc coverage, co-author credit gate, release-day flow). Plus the v0.1.3 Test Matrix covering the full v0.1.0 → v0.1.3 user-facing surface for the first AUR-ship dogfood.
- **Man page `bitlaforge.1`** — full keybindings, safety section, dependency listing (`cpuminer` variants as optdeps), files, authors with co-developer credit.
- **PKGBUILD** — `depends=(python python-textual python-rich python-tomli-w)`; `optdepends=()` for the three `cpuminer*` AUR providers (since `minerd` is itself AUR-only and pacman can't reference AUR packages in `depends`); hardened `check()` runs the headless `run_test()` mount smoke under `PYTHONDONTWRITEBYTECODE=1` with defensive `__pycache__` cleanup in `package()` so we never ship the `.pyc` install-conflict class that bit grubForge v1.0.2.
- The two hotfixes from v0.1.2 (`cfe1010` CPU display + `e3e3091` hashrate parser) are folded forward into v0.1.3 — first tagged version that includes them.

Now installable as:
```bash
yay -S bitlaforge
```

### v0.1.2 — May 29, 2026
**Resource awareness + live Dashboard.**

v0.1.1 made BitlaForge mine; v0.1.2 makes it feel alive while doing it. Five per-group commits, all stdlib (no new deps):

- **G1 — System info on Setup.** New `bitlaforge/system_info.py` reads `/proc/cpuinfo`, `/proc/meminfo`, `os.getloadavg()`, and `os.cpu_count()` into a `SystemInfo` dataclass. The Setup screen gains a block above the minerd status showing **CPU model**, **logical/physical cores**, **memory total/available with % used**, and **1/5/15-minute load averages** color-graded against your core count (green when comfortably idle, yellow as load approaches your ceiling, red over it). Refreshes on **R**.
- **G2 — Config gains miner name + niceness + threads hint.** Two new fields in `~/.config/bitlaforge/config.toml`: `miner_name` (defaults to `socket.gethostname()`) and `niceness` (0–19, default 19). The Thread count label dynamically shows "of N available" sourced from the same `os.cpu_count()`. Forward-compatible: a v0.1.1 TOML loads cleanly with new defaults silently applied.
- **G3 — `miner_runner` worker name + nice wrapper.** `_build_args` now joins the sanitized `miner_name` to the wallet as `wallet.workername` (standard Stratum convention — pools show per-rig stats on their dashboards). `start()` wraps the spawn with `nice -n N` when niceness > 0; niceness ≤ 0 spawns directly with no overhead.
- **G4 — Dashboard live tick + name in header.** App installs a 1-second `set_interval` timer when the miner starts, cancels on stop (and when the parser sees the process end on its own). Each tick re-renders the Dashboard so **uptime advances smoothly** between minerd hashmeter dumps. The Dashboard title now shows the miner name in mauve: `⚡ BitlaForge — Miner Overview — workstation-rig`.
- **G5 — Live `minerd` CPU% / RAM from `/proc/<pid>/`.** New `process_stats.py` parses `/proc/<pid>/stat` (utime + stime ticks) and `/proc/<pid>/status` (VmRSS) — stdlib only, no `psutil`. The App tick captures a baseline at start, computes deltas on each tick, and writes `cpu_pct` (htop-style: 100% = one logical core) + `mem_mb` into `MinerStats`. Dashboard shows them under Performance. Makes the **niceness setting observable**: with niceness=19, you'll see minerd's CPU% drop the moment any other process needs cycles.

No dependency changes. Same `python`, `python-textual`, `python-rich`, `python-tomli-w`.

### v0.1.1 — May 28, 2026
**Make it actually mine.**

v0.1.0 shipped the Forge-style TUI skeleton with sidebar nav, three screens, the help modal, and a confirm dialog — but `minerd` integration was a stub: pressing M just flipped a state flag and notified. v0.1.1 turns the skeleton into a working tool. Four per-group commits:

- **G1 — Config persistence.** New `config_manager.py` (tomllib for read + tomli_w for write) saves pool / wallet / algorithm / threads to `~/.config/bitlaforge/config.toml`. The values survive launches; the Config screen loads them on `__init__`. Schema is intentionally flat so future cycles can add `[ui]` / `[log]` sections without breaking the miner section.
- **G2 — Real minerd subprocess.** New `miner_runner.py` with a `MinerRunner` class that uses `asyncio.create_subprocess_exec`, streams stdout line-by-line via a background `asyncio.Task`, and parses hashrate / accepted / rejected / threads out of pooler-cpuminer-format lines via regex. Each line is forwarded to the Log screen's bounded buffer; each parsed stat update repaints the Dashboard. Stop is `SIGTERM` with a 3-second grace, `SIGKILL` fallback. App's `M` binding is now async and wraps it all.
- **G3 — Runtime check + Dashboard banner + friendly guards.** `shutil.which("minerd")` runs at launch and on every M press. When the binary is missing, the Dashboard shows a persistent red "⚠ minerd not detected" banner with install hint; pressing M produces "install one of: cpuminer / cpuminer-multi / cpuminer-opt from AUR (`yay -S cpuminer`)" instead of the bare "binary not found" error.
- **G4 — Setup screen (4th nav).** New screen at **4** with the minerd status block (path if installed, or ✗ not installed), an "about minerd" explainer, the three AUR providers with copy-pasteable `yay -S` commands, the config file path, and a **T** action that runs `minerd --version` to verify the install actually works.

Dependency note: `python-tomli-w` is now required (same dep alacrittyForge already pulled in).

### v0.1.0 — May 28, 2026
**Pivot from Qt to TUI; first Forge-era release.**

Repo was originally `BitLA` — a Qt6/Widgets desktop scaffold pushed in November 2025 with simulation-driven UI and no real `minerd` integration. Untouched for ~6 months. Pivoted to a Textual TUI on 2026-05-28 because solo mining is fundamentally a streaming-stdout activity on long-running headless rigs, which a TUI fits better. The Qt code (~700 LOC of mostly scaffolding) is preserved permanently as the `v0.1.0-qt-archived` git tag.

Inaugural v0.1.0 ships the Forge-suite skeleton:

- Three screens (Dashboard, Log, Config) under sidebar navigation via `ContentSwitcher`.
- `StatusMixin` for unified status-line + toast feedback, with `popup=False` for passive mount-time hints (so the app launches quietly).
- Toggleable `HelpScreen` modal (Esc / q / ? dismiss; q shadows app-quit while help is up).
- `ConfirmDialog` with `escape` → cancel, `enter` → confirm.
- `DEFAULT_FOCUS` per screen so bindings fire on entry without a panel click.

`minerd` subprocess wiring is the v0.1.1 headline work.

---

## Authors

**Javier ([@jetomev](https://github.com/jetomev))** — idea, direction, testing

**Claude (Anthropic)** — co-developer, architecture, implementation

BitlaForge is built as a real collaboration between a human with an idea and an AI that helps bring it to life — one commit at a time. The co-authorship is preserved in the [Forge-suite recognition thesis](https://github.com/jetomev/grubforge): public projects that demonstrate AI as a serious software collaborator, not a black-box code generator. Co-author credit appears in commits, README, man pages, PKGBUILD, and release notes.

---

## License

GPL v3. See [LICENSE](LICENSE).

---

## Contributing

This is alpha software — UI feedback, bug reports, and ideas welcome via GitHub Issues. If you find BitlaForge useful, consider starring the repo. The Forge-suite recognition thesis only works if these projects are visible.
