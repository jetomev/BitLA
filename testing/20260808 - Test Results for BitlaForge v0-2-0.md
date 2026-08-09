# Test Results — BitlaForge v0.2.0 (2026-08-08)

## Leg 1 — headless pilot suite

**ALL PASSED** (run twice: post-migration and post-design-round):

```
sections OK
dashboard buttons OK
install & setup window OK
shortcuts toggle OK
help menu + about OK
config confirm OK
miner toggle guard OK
ALL SMOKE TESTS PASSED
```

One defect caught by the suite mid-development: the second `?` press did not
close the Shortcuts window — app-level character bindings don't reach through
a modal screen. Fixed via `BitlaShortcuts` subclass binding `?`/`q` to close;
recorded as **forgekit finding #5** (kit dialogs should let apps declare
extra close keys without subclassing).

## Leg 2 — field test (Javier, live desktop)

Round 1 (straight migration): **"I think it looks amazing"** — plus a
7-point design review that reshaped the app (buttons on Dashboard, Setup →
Help window, Miner menu cut, helper texts trimmed).

Round 2 (post-redesign): all rulings verified implemented; blessed for
release.

## Findings carried forward

- **forgekit #5** — modal extra-close-keys (see above); queued for the next
  kit cycle.
- **forgekit candidate** — Input/Select/Checkbox form styling still lives
  app-side (`BITLA_CSS`); promote to the kit when a second app needs forms
  (welcomeforge or the config-heavy migrations).
- **Packaging** — v0.2.0 depends on forgekit, which has no AUR package yet;
  decision pending (AUR `python-forgekit` vs vendoring) and moot until the
  AUR freeze lifts. GitHub release ships now; AUR update follows later.

---

## Addendum — v0.2.1 window-polish batch (2026-08-09)

Second field review (Javier): shortcuts content spec, T + Ctrl+H keys,
window heights. Kit findings F-6 (dynamic key column), F-7 (content
hugging), F-8 (fixed thin button footer — born from the Close button
vanishing below the fold on Install & Setup after F-7).

Headless geometry verification (new): footer-inside-panel asserted on
both short (About, 22 rows) and long (Install & Setup, capped, body
scrolls 26/41) windows; Ctrl+H toggle; T guard path. Full smoke suite
re-passed. Field: "it's perfect! we can declare it done!"
