# Agent Guidelines - gd_keyboard_hook

## Project Overview
`gd_keyboard_hook` is a lightweight Windows DLL proxy (`winmm.dll`) designed to fix system-wide keyboard typing lag caused by **Grim Dawn** when running in Windowed or Borderless Windowed mode.

### Problem Root Cause
Grim Dawn installs a low-level global keyboard hook (`WH_KEYBOARD_LL`) via `SetWindowsHookExA` when running in windowed modes. Because Windows dispatches low-level keyboard hooks synchronously across the message queue, any frame hitching or input polling lag in Grim Dawn introduces noticeable typing delays to external applications (e.g. Discord, browsers, text editors).

### Solution Architecture
1. **DLL Proxy (`winmm.dll`):** Grim Dawn imports multimedia functions from `winmm.dll` upon startup. Placing a custom `winmm.dll` in the game directory intercepts game loading without modifying the game binary.
2. **Export Forwarding (`winmm_exports.h`):** All 180 exports of `winmm.dll` are forwarded directly to `C:\Windows\System32\winmm.dll` using `#pragma comment(linker, "/export:...")` directives.
3. **API Hooking (`dllmain.cpp`):** Uses [MinHook](deps/minhook) to intercept `SetWindowsHookExA`. When Grim Dawn attempts to register a hook with `idHook == WH_KEYBOARD_LL`, the detour suppresses the hook and returns `nullptr` with `SetLastError(ERROR_SUCCESS)`.

---

## Repository Structure

```
gd_keyboard_hook/
├── .gitattributes
├── .gitignore
├── .gitmodules                 # Submodule definition for MinHook
├── README.md                   # User documentation
├── agents.md                   # AI agent instructions and repository context
├── dllmain.cpp                 # DLL entry point, MinHook initialization, and hook detour
├── winmm_exports.h             # 180 export forwarders to C:\Windows\System32\winmm.dll
├── gd_keyboard_hook.slnx       # Visual Studio Solution file
├── gd_keyboard_hook.vcxproj    # Visual Studio C++ Project file
├── gd_keyboard_hook.vcxproj.filters
└── deps/
    └── minhook/                # Submodule: TsudaKageyu/minhook
```

---

## Build System & Environment

- **Toolchain:** Visual Studio 2022 (v143 / v145) with C++20 support.
- **Architectures:** Supports both **32-bit (Win32 / x86)** and **64-bit (x64)**.
  - Grim Dawn 64-bit uses `<Grim Dawn>\x64\winmm.dll`.
  - Grim Dawn 32-bit uses `<Grim Dawn>\winmm.dll`.
- **Target Output Name:** Globally set to `winmm` (`<TargetName>winmm</TargetName>`).
- **Runtime Library:** Uses static CRT (`/MT` in Release, `/MTd` in Debug).
- **HDE Disassembler Engine Configuration:**
  - `deps/minhook/src/hde/hde32.c` is compiled for Win32 (excluded on x64).
  - `deps/minhook/src/hde/hde64.c` is compiled for x64 (excluded on Win32).

### Build Commands (MSBuild)
```powershell
# 64-bit Release
msbuild gd_keyboard_hook.vcxproj /p:Configuration=Release /p:Platform=x64

# 32-bit Release
msbuild gd_keyboard_hook.vcxproj /p:Configuration=Release /p:Platform=Win32
```

Outputs:
- `x64\Release\winmm.dll` (64-bit PE)
- `Release\winmm.dll` (32-bit PE)

---

## Key Rules & Guidelines for Agents

1. **Do NOT Use Module Definition (`.def`) Files for Forwarding:**
   - MSVC `link.exe` does not accept absolute file paths (`C:\...`) in `.def` files.
   - When a DEF file defines `func=winmm.func` inside a DLL named `winmm`, the linker treats `winmm.` as the current project, producing `LNK2001: unresolved external symbol` errors.
   - Always keep export forwarders in `winmm_exports.h` using `#pragma comment(linker, "/export:...")`.
2. **Preserve Dual Architecture Compatibility:**
   - Any modifications must compile cleanly for both `Release|x64` and `Release|Win32`.
   - Keep the platform exclusion conditions on `hde32.c` and `hde64.c` intact in `gd_keyboard_hook.vcxproj`.
3. **Submodule Integrity:**
   - Maintain `deps/minhook` as a Git submodule pointing to `https://github.com/TsudaKageyu/minhook.git`.
