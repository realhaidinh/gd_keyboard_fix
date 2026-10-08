#include "winmm_exports.h"
#include <windows.h>
#include <MinHook.h>

typedef HHOOK(WINAPI* PFN_SetWindowsHookExA)(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId);

PFN_SetWindowsHookExA fpSetWindowsHookExA = nullptr;

HHOOK WINAPI DetourSetWindowsHookExA(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) {
    if (idHook == WH_KEYBOARD_LL) {

        SetLastError(ERROR_SUCCESS);
        return nullptr;
    }
    return fpSetWindowsHookExA(idHook, lpfn, hmod, dwThreadId);
}

void InstallHook() {
    if (MH_Initialize() != MH_OK) return;

    HMODULE hUser32 = GetModuleHandleA("user32.dll");
    if (!hUser32) LoadLibraryA("user32.dll");

    void* target = reinterpret_cast<void*>(GetProcAddress(hUser32, "SetWindowsHookExA"));
    if (target) {
        MH_CreateHook(target, reinterpret_cast<void*>(&DetourSetWindowsHookExA), reinterpret_cast<void**>(&fpSetWindowsHookExA));
        MH_EnableHook(target);
    }
}

void UninstallHook() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        InstallHook();
        break;
    case DLL_PROCESS_DETACH:
        UninstallHook();
        break;
    }
    return TRUE;
}
