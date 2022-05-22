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

void OnStart(HWND hwnd)
{
    TCHAR szDllFile[MAX_PATH];
    getSameFolderPathName(szDllFile, PAYLOAD_NAME TEXT(".dll"));

    static TCHAR s_szText[MAX_PATH * 3];
    GetDlgItemText(hwnd, edt1, s_szText, _countof(s_szText));

    PROCESS_INFORMATION pi = { NULL };
    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    BOOL ret = startProcess(s_szText, si, pi, CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL);
    if (!ret)
    {
        MessageBox(hwnd, TEXT("Cannot start process!"), TEXT("FAILED"), MB_ICONERROR);
        return;
    }

    if (!doInjectDll(szDllFile, pi.dwProcessId))
        MessageBox(hwnd, TEXT("Cannot inject!"), TEXT("FAILED"), MB_ICONERROR);

    ResumeThread(pi.hThread);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}

void OnBrowse(HWND hwnd)
{
    TCHAR szFile[MAX_PATH] = TEXT("");
    OPENFILENAME ofn = { sizeof(ofn), hwnd };
    ofn.lpstrFilter = TEXT("Executable (*.exe)\0*.exe\0");
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = _countof(szFile);
    ofn.lpstrTitle = TEXT("Choose executable");
    ofn.Flags = OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_ENABLESIZING |
                OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = TEXT("ext");
    if (GetOpenFileName(&ofn))
    {
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
