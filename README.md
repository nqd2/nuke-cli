<p align="center">
  <pre align="center">
    ███╗   ██╗██╗   ██╗██╗  ██╗███████╗
    ████╗  ██║██║   ██║██║ ██╔╝██╔════╝
    ██╔██╗ ██║██║   ██║█████╔╝ █████╗  
    ██║╚██╗██║██║   ██║██╔═██╗ ██╔══╝  
    ██║ ╚████║╚██████╔╝██║  ██╗███████╗
    ╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝╚══════╝
  </pre>
  <h3 align="center">☢️ The Nuclear Option for Project Hygiene</h3>
  <p align="center">
    Blazing fast cleanup tool for <code>node_modules</code>, <code>target</code>, <code>venv</code>, and other project junk.
  </p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-Windows-blue?style=flat-square" alt="Platform">
  <img src="https://img.shields.io/badge/C%2B%2B-20-orange?style=flat-square" alt="C++20">
  <img src="https://img.shields.io/badge/license-MIT-green?style=flat-square" alt="License">
  <img src="https://img.shields.io/badge/version-1.0.2-purple?style=flat-square" alt="Version">
</p>

---

## ✨ Features

- **⚡ Blazing Fast** — Uses Windows `robocopy` trick for up to 10x faster deletion than native methods
- **🔍 Smart Scanning** — Multi-threaded directory scanning with configurable depth
- **🎯 Configurable Targets** — Define what to delete in `nuke.config.yaml`
- **🛡️ Safety First** — Built-in protection against deleting system folders
- **📊 Statistics** — Track how much disk space you've reclaimed
- **🎮 Gamification** — Earn ranks from *Noob* to *Duke Nukem* based on total cleanup

## 📦 Installation

### Pre-built Binary (Recommended)

1. Download the latest release: `nuke-v1.0.0-win64.zip`
2. Extract to a folder (e.g., `C:\Tools\nuke`)
3. Add to PATH (optional):
   ```powershell
   [Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\Tools\nuke", "User")
   ```

### Files Included

```
nuke-v1.0.0-win64/
├── nuke.exe           # Main executable
├── fmt.dll            # Formatting library
├── yaml-cpp.dll       # YAML parser
├── nuke.config.yaml   # Configuration file
├── README.md
└── LICENSE
```

## 🚀 Usage

### Basic Commands

```powershell
# Clean current directory (interactive)
nuke clean

# Clean specific path
nuke clean D:\Projects\my-app

# Clean without confirmation
nuke clean --instant

# Dry run (preview what would be deleted)
nuke clean --dry-run

# Only delete folders older than 30 days
nuke clean --older-than 30d
```

### List Targets

```powershell
# List all target folders without deleting
nuke list

# Sort by name or date
nuke list --sort name
nuke list --sort date
```

### Scout Mode

```powershell
# Deep scan to find forgotten projects
nuke scout --root D:\Projects --depth 5
```

### View Statistics

```powershell
# Show your cleanup stats and rank
nuke stats
```

### Verbosity Levels

```powershell
nuke clean -v quiet      # Silent (exit code only)
nuke clean -v minimal    # Final result only
nuke clean -v normal     # Progress + summary (default)
nuke clean -v detailed   # List all folders
nuke clean -v diagnostic # Debug mode
```

## ⚙️ Configuration

Create or edit `nuke.config.yaml` in the same directory as `nuke.exe`:

```yaml
# Folders to be deleted
targets:
  - node_modules
  - .next
  - .nuxt
  - dist
  - build
  - .cache
  - target
  - __pycache__
  - .venv
  - venv
  - .parcel-cache
  - .turbo
  - coverage
  - .nyc_output

# Folders to skip (never scan inside these)
ignore:
  - .git
  - .svn
  - .vscode
  - .idea
  - System32
  - Windows
  - Program Files
  - Program Files (x86)

# Settings
settings:
  # Deletion strategy: "os-fast" (robocopy) or "native" (filesystem API)
  strategy: os-fast
  # Number of parallel scan threads
  scan_threads: 8
```

## 🏗️ Build from Source

### Requirements

- **Windows 10/11** (required - code uses Windows-specific APIs)
- **Visual Studio 2022** (recommended) with "Desktop development with C++" workload
- **CLion** (recommended IDE) or any CMake-compatible IDE
- **vcpkg** (for dependency management)
- **C++20** support

> **⚠️ Important:** This project uses Windows-specific libraries (`<Windows.h>`, `<ShlObj.h>`) and system commands (`robocopy`). You **must build on Windows** using the **Visual Studio (MSVC)** toolchain, not MinGW.

### Step 1: Prepare Environment

1. **Install Visual Studio 2022:**

   - Ensure you have "Desktop development with C++" installed to get the MSVC compiler and Windows SDK.
2. **Install vcpkg:**

   - Install vcpkg in a root directory (e.g., `C:\vcpkg`) to avoid long path issues:

   ```powershell
   git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
   cd C:\vcpkg
   .\bootstrap-vcpkg.bat
   ```

### Step 2: Open Project in CLion

1. Open CLion.
2. Select **Open** and navigate to the `nuke-em` folder (the directory containing `CMakeLists.txt`).
3. Select **Trust Project** if prompted.

### Step 3: Configure CMake in CLion

This is the most important step to ensure CLion recognizes the libraries from `vcpkg.json` (`cli11`, `fmt`, `yaml-cpp`).

1. Go to **File** > **Settings** (or `Ctrl+Alt+S`).
2. Navigate to **Build, Execution, Deployment** > **CMake**.
3. In the **CMake options** field, add the following line to point to vcpkg's toolchain file:

   ```text
   -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

   > **Note:** Replace `C:/vcpkg` with your actual vcpkg installation path. Use forward slashes `/` instead of backslashes `\` to avoid errors.
   >
4. In the **Toolchain** section (above CMake options), ensure you have **Visual Studio** selected (not MinGW).

   - If Visual Studio is not available, go to **Toolchains** (left sidebar), click the `+` button, and select Visual Studio. CLion will automatically detect your VS installation.
5. **Build type:** Create a `Release` profile for optimal performance. Click the `+` button in the Profiles list to add `Release`.

### Step 4: Build the Project

1. After configuration, click **OK** or **Apply**.
2. Check the status bar at the bottom (CMake tab). CLion will start "Loading CMake project".

   - At this point, vcpkg will automatically download and compile the libraries (`fmt`, `yaml-cpp`, `cli11`) based on `vcpkg.json`. This may take several minutes on the first run.
3. Once CMake loads successfully (shows `[Finished]`), look at the top-right corner and select **nuke | Release** (or Debug) from the configuration dropdown.
4. Click the Build icon (hammer) or press `Ctrl+F9`.

### Step 5: Run/Test

1. Since this is a CLI tool, clicking Run (green triangle) may cause the program to run and exit immediately or show help and exit.
2. To test specific commands (e.g., `nuke list`), configure program arguments:
   - Click the dropdown next to the Run button (where `nuke` is displayed), select **Edit Configurations...**
   - In the **Program arguments** field, enter the arguments you want to test, e.g., `list` or `scout --root .`
   - Click **OK** and then **Run**.

### Alternative: Command Line Build

If you prefer building from the command line:

```powershell
# Clone the repository
git clone https://github.com/nqd2/nuke-em.git
cd nuke-em

# Configure and build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release

# The executable will be in build/Release/nuke.exe (or build/nuke.exe depending on generator)
```

### Troubleshooting

- **`find_package` failed / Libraries not found:**

  - Verify the `-DCMAKE_TOOLCHAIN_FILE` path is correct.
  - Ensure `vcpkg.json` is in the same directory as `CMakeLists.txt`.
- **`Windows.h` not found:**

  - You're using MinGW or Cygwin toolchain. Go to **Settings** > **Build** > **Toolchains** and switch to **Visual Studio**.
- **C++ Standard errors:**

  - `CMakeLists.txt` requires CMake 3.20+ and C++20. Ensure your Visual Studio is recent enough (VS 2019 v16.11+ or VS 2022).

### Dependencies

| Package                                     | Description               |
| ------------------------------------------- | ------------------------- |
| [CLI11](https://github.com/CLIUtils/CLI11)     | Command line parser       |
| [fmt](https://github.com/fmtlib/fmt)           | Modern formatting library |
| [yaml-cpp](https://github.com/jbeder/yaml-cpp) | YAML parser               |

## 📁 Project Structure

```
nuke-em/
├── src/
│   ├── main.cpp              # Entry point & CLI setup
│   ├── core/
│   │   ├── config.cpp        # Configuration loader
│   │   ├── scanner.cpp       # Directory scanner
│   │   └── destroyer.cpp     # Deletion engine
│   ├── ui/
│   │   ├── display.cpp       # Terminal UI & colors
│   │   └── logger.cpp        # Logging system
│   └── utils/
│       ├── safety.cpp        # Path safety checks
│       └── stats.cpp         # Statistics tracking
├── include/nuke/
│   └── [headers]
├── CMakeLists.txt
├── vcpkg.json
├── nuke.config.yaml
└── README.md
```

## 🎖️ Ranks

| Rank              | Threshold |
| ----------------- | --------- |
| 🥉 Noob           | 0 GB      |
| 🧹 Janitor        | 10 GB     |
| 🧽 Cleaner        | 100 GB    |
| 💣 Demolition Man | 1 TB      |
| ☢️ Duke Nukem   | 1+ TB     |

## ⚠️ Safety

NUKE includes built-in safety features:

- **Protected paths**: System folders like `Windows`, `System32`, `Program Files` are always ignored
- **Confirmation prompt**: Large deletions require confirmation or CAPTCHA
- **Dry run mode**: Preview what would be deleted before committing

## 📄 License

MIT License © 2026 阮貴德

See [LICENSE](LICENSE) for details.

---

<p align="center">
  <i>Made with ☢️ for developers who hate waiting for <code>rm -rf node_modules</code></i>
</p>
