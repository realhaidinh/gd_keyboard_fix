# Grim Dawn Keyboard Hook Fix (`gd_keyboard_hook`)

A DLL proxy (`winmm.dll`) that fixes typing lag and keyboard slowdown caused by **Grim Dawn** when running in Windowed or Borderless Windowed mode.

---

## The Problem

When Grim Dawn runs in windowed or borderless mode, DirectInput calls Windows API `SetWindowsHookExA` to install a system-wide low-level keyboard hook (`WH_KEYBOARD_LL`).

Because this hook runs synchronously on the game's message thread, any performance hiccups or high frame times in the game introduce noticeable delays to keyboard input across your entire system.

---

## The Solution

This project builds a **`winmm.dll` proxy** which uses [MinHook](https://github.com/TsudaKageyu/minhook) to disable `WH_KEYBOARD_LL` hook. DirectInput gameplay controls (movement, skills, keybindings) are completely handled via DirectInput8 COM hardware device polling (`GetDeviceData` / `GetDeviceState`). Therefore, **disabling the hook installation completely eliminates the OS typing stall without affecting any gameplay controls**.

---

## Installation

1. Build the project or download from release.
2. Locate your Grim Dawn installation directory (e.g., `Steam\steamapps\common\Grim Dawn`).
3. Place `winmm.dll` into the folder containing the game executable:
   - For the 64-bit version: place inside the `x64/` subdirectory (next to `Grim Dawn.exe`).
   - For the 32-bit version: place in the root folder alongside `Grim Dawn.exe`.
4. Launch Grim Dawn.

To uninstall, simply delete `winmm.dll` from the folder.

---

## Building from Source

### Prerequisites

- Visual Studio 2022 (v143 / v145 toolset) with C++ Desktop Development.
- Git (with submodules enabled).

### Build Steps

```bash
# Clone with submodules
git clone --recursive https://github.com/haidinh/gd_keyboard_hook.git
```

### 1. Clone the Repository

```bash
git clone --recursive https://github.com/haidinh/gd_keyboard_hook.git
cd gd_keyboard_hook
```

_(If you already cloned without submodules, run `git submodule update --init --recursive`)_

### 2. Build via Visual Studio

1. Open `gd_keyboard_hook.slnx` (or `gd_keyboard_hook.vcxproj`) in Visual Studio.
2. Select your desired configuration:
   - **Release | x64** for the 64-bit build.
   - **Release | Win32** for the 32-bit build.
3. Press **Build Solution** (`Ctrl + Shift + B`).

### 3. Build via Command Line (MSBuild)

```powershell
# Build 64-bit (x64)
msbuild gd_keyboard_hook.vcxproj /p:Configuration=Release /p:Platform=x64

# Build 32-bit (x86 / Win32)
msbuild gd_keyboard_hook.vcxproj /p:Configuration=Release /p:Platform=Win32
```

Outputs will be generated at:

- 64-bit: `x64\Release\winmm.dll`
- 32-bit: `Release\winmm.dll`

## Acknowledgments & Third-Party Licenses

This project relies on the following third-party software:

- **[MinHook](https://github.com/TsudaKageyu/minhook)** — Copyright (c) 2009–2017 Tsuda Kageyu. Licensed under the [BSD 2-Clause License](deps/minhook/LICENSE.txt).
- **Hacker Disassembler Engine (HDE32/HDE64)** — Copyright (c) 2008–2009 Vyacheslav Patkov. Licensed under the [BSD 2-Clause License](deps/minhook/LICENSE.txt).
