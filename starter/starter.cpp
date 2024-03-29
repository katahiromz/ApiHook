#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <psapi.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <cassert>
#include "../config.h"
#include "../hackKit/hackKit.h"

DWORD g_dwProcessId = 0;
TCHAR g_szText[MAX_PATH * 3];

void OnStart(HWND hwnd)
{
    TCHAR szDllFile[MAX_PATH];
    getSameFolderPathName(szDllFile, PAYLOAD_NAME TEXT(".dll"));

    GetDlgItemText(hwnd, edt1, g_szText, _countof(g_szText));

    PROCESS_INFORMATION pi = { NULL };
    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    BOOL ret = startProcess(g_szText, si, pi, CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL);
    if (!ret)
    {
        MessageBox(hwnd, TEXT("Cannot start process!"), TEXT("FAILED"), MB_ICONERROR);
        return;
    }

    g_dwProcessId = pi.dwProcessId;
    if (!doInjectDll(szDllFile, g_dwProcessId))
        MessageBox(hwnd, TEXT("Cannot inject!"), TEXT("FAILED"), MB_ICONERROR);

    ResumeThread(pi.hThread);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void OnUninject(HWND hwnd)
{
    TCHAR szDllFile[MAX_PATH];
    getSameFolderPathName(szDllFile, PAYLOAD_NAME TEXT(".dll"));

    if (!doUninjectDll(szDllFile, g_dwProcessId))
        MessageBox(hwnd, TEXT("Cannot uninject!"), TEXT("FAILED"), MB_ICONERROR);
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    TCHAR szExeFile[MAX_PATH];
    szExeFile[0] = TEXT('"');
    getSameFolderPathName(&szExeFile[1], TARGET_NAME TEXT(".exe"));
    lstrcat(szExeFile, TEXT("\""));
    SetDlgItemText(hwnd, edt1, szExeFile);
    return TRUE;
}

void OnBrowse(HWND hwnd)
{
    TCHAR szFile[MAX_PATH + 2] = TEXT("\"");
    OPENFILENAME ofn = { sizeof(ofn), hwnd };
    ofn.lpstrFilter = TEXT("Executable (*.exe)\0*.exe\0");
    ofn.lpstrFile = &szFile[1];
    ofn.nMaxFile = _countof(szFile);
    ofn.lpstrTitle = TEXT("Choose executable");
    ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_ENABLESIZING |
                OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = TEXT("ext");
    if (GetOpenFileName(&ofn))
    {
        lstrcat(szFile, TEXT("\""));
        SetDlgItemText(hwnd, edt1, szFile);
    }
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    case psh1:
        OnStart(hwnd);
        break;
    case psh2:
        OnBrowse(hwnd);
        break;
    case psh3:
        OnUninject(hwnd);
        break;
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    enableProcessPriviledge(SE_DEBUG_NAME);
    DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
    return 0;
}
