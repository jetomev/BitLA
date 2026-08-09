"""BitlaForge — the Install & Setup body text.

v0.2.0 design ruling: Setup is no longer a section — it's a read-only
window under Help → Install & Setup (a forgekit ``ForgePanelScreen``).
This module builds its body: system info, minerd status, AUR providers,
paths, and the safety note. Rebuilt on every open, so load averages and
the minerd PATH check are always current.
"""

from __future__ import annotations

from .config_manager import CONFIG_PATH


def build_setup_body() -> str:
    """System info + minerd status + providers + paths, as one Rich string."""
    from .miner_runner import find_minerd, MINERD_AUR_PROVIDERS
    from .system_info import get_system_info

    info = get_system_info()
    minerd_path = find_minerd()

    if info.physical_cores != info.logical_cores:
        cores_str = (
            f"{info.logical_cores} logical "
            f"([dim]{info.physical_cores} physical[/])"
        )
    else:
        cores_str = f"{info.logical_cores}"

    mem_pct_used = (
        int(100 * (1 - info.mem_available_mb / info.mem_total_mb))
        if info.mem_total_mb > 0 else 0
    )
    load_color = (
        "#a6e3a1" if info.load_1 < info.logical_cores * 0.5
        else "#f9e2af" if info.load_1 < info.logical_cores * 0.9
        else "#f38ba8"
    )

    system_block = (
        f"  [#89b4fa]Host[/]         [#cdd6f4]{info.hostname}[/]\n"
        f"  [#89b4fa]CPU[/]          [#cdd6f4]{info.cpu_model}[/]\n"
        f"  [#89b4fa]Cores[/]        [#cdd6f4]{cores_str}[/]\n"
        f"  [#89b4fa]Memory[/]       "
        f"[#cdd6f4]{info.mem_available_mb / 1024:.1f} GB free "
        f"of {info.mem_total_mb / 1024:.1f} GB[/]  "
        f"[dim]({mem_pct_used}% used)[/]\n"
        f"  [#89b4fa]Load avg[/]     "
        f"[{load_color}]{info.load_1:.2f}[/]  "
        f"[dim]{info.load_5:.2f} (5m)  {info.load_15:.2f} (15m)[/]\n"
    )

    if minerd_path:
        status_block = (
            "[bold #a6e3a1]✓ minerd is installed[/]\n"
            f"  [#89b4fa]Path:[/]  [#cdd6f4]{minerd_path}[/]\n"
            "\n"
            "[#a6adc8]Use [bold]Test Miner[/bold] on the Dashboard to verify "
            "the binary responds.[/]\n"
        )
    else:
        status_block = (
            "[bold #f38ba8]✗ minerd is not installed[/]\n"
            "  [#a6adc8]No `minerd` binary on PATH. BitlaForge can browse,\n"
            "  configure, and exercise its UI, but Start Miner won't actually\n"
            "  start anything until one of the AUR packages below is in.[/]\n"
        )

    providers_block = "\n".join(
        f"  [#cba6f7]{pkg:18s}[/]  [#a6adc8]{desc}[/]\n"
        f"    [#6c7086]install:[/]  [#cdd6f4]yay -S {pkg}[/]"
        for pkg, desc in MINERD_AUR_PROVIDERS
    )

    return (
        "[bold #cba6f7]── System info ────────────────────────────────────[/]\n"
        "\n"
        f"{system_block}"
        "\n"
        "[bold #cba6f7]── minerd status ──────────────────────────────────[/]\n"
        "\n"
        f"{status_block}"
        "\n"
        "[bold #cba6f7]── About minerd ───────────────────────────────────[/]\n"
        "\n"
        "  [#cdd6f4]minerd[/] is the CPU mining binary BitlaForge wraps.\n"
        "  It is [bold]not[/] in the official Arch repos — only on the AUR,\n"
        "  with several variants. BitlaForge calls whichever one provides\n"
        "  /usr/bin/minerd, so any of these works.\n"
        "\n"
        "[bold #cba6f7]── AUR providers ─────────────────────────────────[/]\n"
        "\n"
        f"{providers_block}\n"
        "\n"
        "[bold #cba6f7]── Paths ─────────────────────────────────────────[/]\n"
        "\n"
        f"  [#89b4fa]Config[/]      [#cdd6f4]{CONFIG_PATH}[/]\n"
        "  [#89b4fa]minerd[/]      [#cdd6f4]/usr/bin/minerd[/]  "
        "[#6c7086](provided by whichever cpuminer package above)[/]\n"
        "\n"
        "[bold #cba6f7]── Safety ────────────────────────────────────────[/]\n"
        "\n"
        "  Mining draws real CPU and electricity. BitlaForge will never\n"
        "  start [bold]minerd[/] on its own — only [bold]Start Miner[/] (or M)\n"
        "  starts it, and [bold]Stop Miner[/] always stops it cleanly.\n"
    )
