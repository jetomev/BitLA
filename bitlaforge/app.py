"""BitlaForge — main TUI application, on the forgekit shell.

v0.2.0: the hand-rolled sidebar / Header / Footer / HelpScreen chrome is
replaced by ``forgekit.ForgeApp`` (menu bar + section switcher + Help
windows + Catppuccin theme) — BitlaForge is the Forge Suite's first
forgekit adopter. The four screens and the whole minerd subprocess layer
(MinerRunner, process stats, config manager) carry over unchanged.

Keyboard surface keeps the BitlaForge muscle memory (1-4 sections, M
start/stop, R refresh, ? shortcuts, Q quit) and adds the forgekit menu
accelerators (Ctrl+letter opens/switches; submenu letters pick items).
"""

import asyncio

from textual.binding import Binding
from textual.widgets import Static

from forgekit import (
    ConfirmDialog, ForgeApp, ForgePanelScreen, ShortcutsDialog,
    FORGE_CSS, GPL3_NOTICE,
)

from . import __version__
from .config_manager import load_config
from .miner_runner import MinerRunner, MinerStats, find_minerd
from .process_stats import read_process_sample, compute_cpu_pct
from .screens.dashboard import DashboardScreen
from .screens.log import LogScreen
from .screens.config import ConfigScreen
from .setup_info import build_setup_body


MENU = [
    {"id": "dashboard", "title": "Dashboard", "kind": "section"},
    {"id": "log",       "title": "Log",       "kind": "section"},
    {"id": "config",    "title": "Config",    "kind": "section"},
    {"id": "help",      "title": "Help",      "kind": "menu", "items": [
        ("Shortcuts",       "s", "shortcuts"),
        ("Install & Setup", "i", "install"),
        ("License",         "l", "license"),
        ("About",           "a", "about"),
    ]},
    {"id": "quit",      "title": "Quit",      "kind": "action", "action": "quit"},
]

SHORTCUTS = [
    ("1-3",    "Switch section (Dashboard / Log / Config)"),
    ("M",      "Start / stop the miner"),
    ("R",      "Refresh the current section"),
    ("?",      "Toggle this shortcuts window"),
    ("Q",      "Quit"),
    ("Ctrl+…", "Menu accelerators (underlined letters in the menu bar)"),
    ("/",      "Log: focus the filter input"),
    ("C",      "Log: clear the buffer"),
    ("E",      "Config: begin editing (focus first field)"),
    ("S",      "Config: save configuration"),
]

ABOUT = {
    "name": "BitlaForge",
    "version": __version__,
    "tagline": "Solo Bitcoin mining, the lottery way — a Textual TUI over minerd.",
    "description": (
        "BitlaForge is a Catppuccin-Mocha terminal UI for solo mining to a "
        "pool like ckpool: configure wallet, pool and threads, launch the "
        "miner, and watch the log. Part of the Forge Suite for KognogOS."
    ),
    "authors": "Javier (jetomev) + Claude (Anthropic)",
    "license": "GPL-3.0-or-later",
    "links": [
        ("GitHub", "https://github.com/jetomev/bitlaforge"),
        ("AUR",    "https://aur.archlinux.org/packages/bitlaforge"),
    ],
}

# App-specific styling on top of the forgekit base. Form-widget rules
# (Input/Select/Checkbox) stay app-side for now — promotion candidate for
# a future forgekit once a second app needs them.
BITLA_CSS = """
.main-area { height: 100%; }
.section-title { color: #cba6f7; text-style: bold; margin-bottom: 1; }
.detail-muted { color: #a6adc8; margin-bottom: 1; }
.field-label { color: #89b4fa; margin-top: 1; }

#dashboard-content { color: #cdd6f4; height: auto; }

.log-controls { layout: horizontal; height: 3; margin-bottom: 1; }
#log-search { width: 1fr; margin-right: 1; }
#log-autoscroll { width: 18; }
#log-view-container { background: #181825; border: round #313244; padding: 0 1; height: 1fr; }
#log-view { background: #181825; color: #cdd6f4; height: auto; }
#log-status, #config-status {
    color: #cdd6f4; padding: 0 1; height: 1; margin-top: 1;
}

/* Dashboard action buttons sit under the stats, left-aligned. */
.dash-buttons { align-horizontal: left; }
.dash-buttons Button { margin: 0 2 0 0; }

Input { background: #313244; color: #cdd6f4; border: solid #45475a; }
Input:focus { border: solid #89b4fa; }
Select { background: #313244; color: #cdd6f4; border: solid #45475a; }
Select:focus { border: solid #89b4fa; }
Checkbox { background: #1e1e2e; color: #cdd6f4; }
"""


class InstallSetupDialog(ForgePanelScreen):
    """Help → Install & Setup: system info, minerd status, AUR providers.
    Body is rebuilt on every open so PATH checks and load stay current."""

    panel_title = "Install & Setup — minerd"

    def compose_body(self):
        yield Static(build_setup_body())


class BitlaShortcuts(ShortcutsDialog):
    """Shortcuts window that also closes on ? and q (BitlaForge muscle
    memory — app-level character bindings don't reach through a modal)."""

    BINDINGS = [
        Binding("question_mark", "close", "", show=False),
        Binding("q", "close", "", show=False),
    ]


class BitlaForgeApp(ForgeApp):
    """BitlaForge — solo Bitcoin mining TUI, on the forgekit shell."""

    APP_NAME = "⚡ BitlaForge"
    MENU = MENU
    SHORTCUTS = SHORTCUTS
    ABOUT = ABOUT
    LICENSE_NAME = "GPL-3.0-or-later"
    LICENSE_NOTICE = GPL3_NOTICE
    CSS = FORGE_CSS + BITLA_CSS

    BINDINGS = [
        # BitlaForge muscle memory (screens keep focus on containers, not
        # Inputs, so bare character keys reach the app — the v0.1.0 lesson).
        Binding("1", "activate('dashboard')", show=False),
        Binding("2", "activate('log')",       show=False),
        Binding("3", "activate('config')",    show=False),
        Binding("m", "toggle_miner",          show=False),
        Binding("r", "refresh_active",        show=False),
        Binding("q", "activate('quit')",      show=False),
        Binding("question_mark", "toggle_shortcuts", show=False),
        # forgekit-convention menu accelerators.
        Binding("ctrl+d", "activate('dashboard')", show=False, priority=True),
        Binding("ctrl+l", "activate('log')",       show=False, priority=True),
        Binding("ctrl+c", "activate('config')",    show=False, priority=True),
    ]

    miner_stats: MinerStats = MinerStats()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # Single runner instance per App. Callbacks forward stdout lines
        # to the Log screen and pump stats updates through to Dashboard.
        self._runner = MinerRunner(
            on_line=self._on_miner_line,
            on_stats=self._on_miner_stats,
        )
        # 1-second tick while mining (G4 v0.1.2) + previous /proc sample
        # for the CPU% delta (G5 v0.1.2).
        self._tick_timer = None
        self._proc_prev = None

    @property
    def miner_running(self) -> bool:
        return self.miner_stats.running

    # ── forgekit hooks ────────────────────────────────────────────────────

    def compose_sections(self):
        yield DashboardScreen(id="sec-dashboard")
        yield LogScreen(id="sec-log")
        yield ConfigScreen(id="sec-config")

    def on_mount(self) -> None:
        super().on_mount()
        # Land initial focus on the Dashboard so bare character keys reach
        # the app bindings instead of the Log search Input (v0.1.0 lesson).
        try:
            self.query_one("#sec-dashboard").focus()
        except Exception:
            pass

    def on_section_shown(self, section_id: str) -> None:
        """Refresh-on-show + focus the section's primary widget."""
        try:
            screen = self.query_one(f"#sec-{section_id}")
        except Exception:
            return
        if hasattr(screen, "on_show"):
            try:
                screen.on_show()
            except Exception:
                pass
        focus_id = getattr(screen, "DEFAULT_FOCUS", None)
        if focus_id:
            try:
                self.query_one(focus_id).focus()
            except Exception:
                pass

    def on_action(self, action_id: str) -> None:
        """Dispatch the app's own menu action ids."""
        if action_id == "install":
            self.push_screen(InstallSetupDialog())

    def action_act(self, action_id: str) -> None:
        # Route the Help menu's Shortcuts item to the ?-closable variant.
        if action_id == "shortcuts":
            self.push_screen(BitlaShortcuts(self.SHORTCUTS))
        else:
            super().action_act(action_id)

    def action_toggle_shortcuts(self) -> None:
        """`?` toggles the shortcuts window (pops if open, pushes otherwise)."""
        if isinstance(self.screen, ShortcutsDialog):
            self.pop_screen()
        else:
            self.push_screen(BitlaShortcuts(self.SHORTCUTS))

    # ── Miner control ─────────────────────────────────────────────────────

    async def action_toggle_miner(self) -> None:
        """Toggle the minerd subprocess (guards from G3 v0.1.1)."""
        if self._runner.is_running:
            await self._runner.stop()
            self._stop_live_tick()
            self.notify("Miner stopped.", severity="information", timeout=4)
            return

        if find_minerd() is None:
            self.notify(
                "minerd not found on PATH — install one of: cpuminer / "
                "cpuminer-multi / cpuminer-opt from AUR (e.g. "
                "`yay -S cpuminer`). Then press M again.",
                severity="warning", timeout=8,
            )
            try:
                self.query_one("#sec-dashboard")._reload_view()
            except Exception:
                pass
            return

        config = load_config()
        missing = [k for k in ("pool", "wallet") if not config.get(k)]
        if missing:
            self.notify(
                f"Configure {', '.join(missing)} first (press 3).",
                severity="warning", timeout=5,
            )
            return

        ok, msg = await self._runner.start(config)
        if ok:
            self.notify(f"Miner started — {msg}", severity="information", timeout=4)
            self._start_live_tick()
        else:
            self.notify(f"Failed to start miner: {msg}", severity="error", timeout=6)

    async def action_test_minerd(self) -> None:
        """Run `minerd --version` and surface the result as a toast.

        Lived on the Setup screen through v0.1.x; app-level since the
        v0.2.0 ruling put Test Miner on the Dashboard."""
        minerd_path = find_minerd()
        if minerd_path is None:
            self.notify(
                "Can't test — minerd is not on PATH. See Help → Install & Setup.",
                severity="warning", timeout=6,
            )
            return
        try:
            proc = await asyncio.create_subprocess_exec(
                minerd_path, "--version",
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.STDOUT,
            )
            out, _ = await asyncio.wait_for(proc.communicate(), timeout=5.0)
            text = out.decode("utf-8", errors="replace").strip()
            first_line = text.splitlines()[0] if text else "(no output)"
            self.notify(f"minerd {first_line[:120]}", severity="information", timeout=6)
        except asyncio.TimeoutError:
            self.notify("minerd --version timed out after 5s.", severity="error", timeout=6)
        except OSError as e:
            self.notify(f"Couldn't exec minerd: {e}", severity="error", timeout=6)

    # ── MinerRunner callbacks ─────────────────────────────────────────────

    def _on_miner_line(self, line: str) -> None:
        try:
            self.query_one("#sec-log").append_line(line)
        except Exception:
            pass

    def _on_miner_stats(self, stats: MinerStats) -> None:
        self.miner_stats = stats
        try:
            self.query_one("#sec-dashboard")._reload_view()
        except Exception:
            pass
        if not stats.running and self._tick_timer is not None:
            self._stop_live_tick()

    # ── Live Dashboard tick (G4/G5 v0.1.2) ────────────────────────────────

    def _start_live_tick(self) -> None:
        if self._tick_timer is not None:
            return
        pid = self._runner.pid
        if pid is not None:
            self._proc_prev = read_process_sample(pid)
        self._tick_timer = self.set_interval(1.0, self._on_tick)

    def _stop_live_tick(self) -> None:
        if self._tick_timer is not None:
            try:
                self._tick_timer.stop()
            except Exception:
                pass
            self._tick_timer = None
        self._proc_prev = None

    def _on_tick(self) -> None:
        pid = self._runner.pid
        if pid is not None:
            sample = read_process_sample(pid)
            if sample is not None:
                self.miner_stats.mem_mb = sample.rss_mb
                if self._proc_prev is not None:
                    self.miner_stats.cpu_pct = compute_cpu_pct(self._proc_prev, sample)
                self._proc_prev = sample
        try:
            self.query_one("#sec-dashboard")._reload_view()
        except Exception:
            pass

    def action_refresh_active(self) -> None:
        """R — dispatch refresh to the active section if it has action_refresh."""
        current = self.query_one("#forge-work").current or ""
        try:
            screen = self.query_one(f"#{current}")
            fn = getattr(screen, "action_refresh", None)
            if callable(fn):
                fn()
        except Exception:
            pass
