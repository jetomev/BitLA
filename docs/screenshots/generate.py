#!/usr/bin/env python3
"""Regenerate the README screenshot gallery — pixel-perfect Textual SVGs.

Run from the repo root:  python docs/screenshots/generate.py
Dashboard stats are staged demo values (no minerd is spawned).
"""
import asyncio
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from bitlaforge.app import BitlaForgeApp, BitlaShortcuts, InstallSetupDialog  # noqa: E402
from bitlaforge.miner_runner import MinerStats  # noqa: E402

OUT = Path(__file__).resolve().parent
SIZE = (120, 36)


def shot(app, name: str) -> None:
    app.save_screenshot(filename=f"{name}.svg", path=str(OUT))
    print(f"  {name}.svg")


async def main() -> None:
    app = BitlaForgeApp()
    async with app.run_test(size=SIZE) as pilot:
        await pilot.pause()

        # A lively dashboard: staged demo stats, miner "running"
        stats = MinerStats(
            running=True, hashrate_khs=184_320.0, accepted=37, rejected=0,
            threads=16, started_at=time.time() - (3 * 3600 + 24 * 60 + 9),
            cpu_pct=1520.0, mem_mb=182,
            pool="stratum+tcp://solo.ckpool.org:3333",
            wallet="bc1qexampleexampleexampleexampleexamplex0", algorithm="sha256d",
        )
        app.miner_stats = stats
        dash = app.query_one("#sec-dashboard")
        dash._reload_view(); await pilot.pause()
        shot(app, "01-dashboard-running")

        app.miner_stats = MinerStats()
        dash._reload_view(); await pilot.pause()
        shot(app, "02-dashboard-stopped")

        await pilot.press("3"); await pilot.pause()
        shot(app, "03-config")

        app.push_screen(InstallSetupDialog()); await pilot.pause()
        shot(app, "04-install-setup")
        await pilot.press("escape"); await pilot.pause()

        app.push_screen(BitlaShortcuts(app.SHORTCUTS)); await pilot.pause()
        shot(app, "05-shortcuts")

    print("BitlaForge gallery done.")


asyncio.run(main())
