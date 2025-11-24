# BitLA

Bitcoin Lottery Application (BitLA) is a KDE/Qt desktop interface designed to wrap and monitor `minerd`. The initial version ships a scaffolded dashboard with simulated miner activity so the layout and controls can be exercised before wiring to a real process.

## Features (current state)
- Menu bar with File, View, Setup, and About sections
- Vertical action bar for **Start Miner** / **Stop Miner** (simulation-driven today)
- Dashboard pane for pool, wallet, CPU load, threads, uptime, hashrate, shares, and log line counts
- Graph & Stats placeholder with selectable refresh intervals (1s–1h)
- Log pane with search/filter input, auto-scroll toggle, and 5,000-line bounded buffer
- Theme switching between Light, Dark, and System palettes
- Configure Miner dialog capturing pool URL, wallet, algorithm, and thread count
- Install Minerd menu entry placeholder marked “coming soon”

## Building
```bash
mkdir -p build && cd build
cmake ..
make -j$(nproc)
./BitLA
```

Qt 6 with the Widgets module is required (on Arch: `sudo pacman -S qt6-base`).

## If you “don’t see anything” in Code-OSS or on GitHub

Follow these steps to make sure the project files show up locally and online:

1. **Confirm files exist locally**
   - In a terminal, from the project folder, run `ls` and you should see `CMakeLists.txt`, `README.md`, `src/`, etc.
   - If the folder is empty, you might be in the wrong directory—use `pwd` to confirm you are inside `BitLA`.

2. **Open the correct folder in Code-OSS**
   - In Code-OSS, go to **File → Open Folder…** and select the `BitLA` folder that contains `CMakeLists.txt` and `src/`.
   - In the Explorer pane (left sidebar), you should now see the files; if not, close and re-open the folder.

3. **Publish to GitHub (if your GitHub repo is empty)**
   - Create an empty repository on GitHub (e.g., `bitla`). Do **not** add a README or .gitignore there.
   - Back in the `BitLA` folder locally, run:
     ```bash
     git status            # verify files are tracked
     git branch -M main    # optional: rename current branch to main
     git remote add origin https://github.com/<your-username>/bitla.git
     git push -u origin main
     ```
   - Replace `<your-username>` with your GitHub username. After pushing, refresh the GitHub page and the files should appear.

4. **If push fails (common causes)**
   - Authentication prompt: log in with your GitHub credentials or a personal access token.
   - Remote already exists: use `git remote set-url origin https://github.com/<your-username>/bitla.git` to update it.
   - Branch name mismatch: if your local branch is `work`, push with `git push -u origin work` and then set that as the default branch on GitHub.

If any step fails, copy the exact error message—the wording points directly to the fix (e.g., wrong folder, missing remote, or auth issues).
