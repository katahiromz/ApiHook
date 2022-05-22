#include <windows.h>
#include "../config.h"
#include "../hackKit/hackKit.h"

EXTERN_C __declspec(dllexport)
INT WINAPI DetourMessageBoxA(HWND hwnd, LPCWSTR text, LPCWSTR title, UINT uType)
{
    return MessageBoxA(hwnd, "Being Hooked!", "Being Hooked!", uType);
}

EXTERN_C __declspec(dllexport)
INT WINAPI DetourMessageBoxW(HWND hwnd, LPCWSTR text, LPCWSTR title, UINT uType)
{
    return MessageBoxW(hwnd, L"Being Hooked!", L"Being Hooked!", uType);
}

EXTERN_C BOOL WINAPI
DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    static LPVOID s_fnOldMessageBoxA = NULL;
    static LPVOID s_fnOldMessageBoxW = NULL;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        s_fnOldMessageBoxA = doHookAPI(NULL, "user32.dll", "MessageBoxA", (LPVOID)DetourMessageBoxA);
        s_fnOldMessageBoxW = doHookAPI(NULL, "user32.dll", "MessageBoxW", (LPVOID)DetourMessageBoxW);
        break;

    case DLL_PROCESS_DETACH:
        doHookAPI(NULL, "user32.dll", "MessageBoxA", s_fnOldMessageBoxA);
        doHookAPI(NULL, "user32.dll", "MessageBoxW", s_fnOldMessageBoxW);
        break;
    }
    return TRUE;
}
