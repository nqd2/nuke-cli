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
  <img src="https://img.shields.io/badge/version-1.0.0-purple?style=flat-square" alt="Version">
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

- Windows 10/11
- Visual Studio 2022 (v143 toolset) or CLion
- vcpkg
- C++20

### Setup

1. **Clone the repository:**
   ```powershell
   git clone https://github.com/user/nuke-em.git
   cd nuke-em
   ```

2. **Install vcpkg** (if not already):
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
   C:\vcpkg\bootstrap-vcpkg.bat
   C:\vcpkg\vcpkg integrate install
   ```

3. **Build with CMake:**
   ```powershell
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build build --config Release
   ```

### Dependencies

| Package | Description |
|---------|-------------|
| [CLI11](https://github.com/CLIUtils/CLI11) | Command line parser |
| [fmt](https://github.com/fmtlib/fmt) | Modern formatting library |
| [yaml-cpp](https://github.com/jbeder/yaml-cpp) | YAML parser |

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

| Rank | Threshold |
|------|-----------|
| 🥉 Noob | 0 GB |
| 🧹 Janitor | 10 GB |
| 🧽 Cleaner | 100 GB |
| 💣 Demolition Man | 1 TB |
| ☢️ Duke Nukem | 1+ TB |

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
