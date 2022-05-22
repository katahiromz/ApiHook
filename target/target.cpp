#include <windows.h>
#include <windowsx.h>
#include "../config.h"

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    DWORD dwProcessID = GetCurrentProcessId();
    SetDlgItemInt(hwnd, edt1, (UINT)dwProcessID, FALSE);
    return TRUE;
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
        MessageBox(hwnd, TEXT("Being Unhooked!"), TEXT("Being Unhooked!"), MB_ICONINFORMATION);
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

INT APIENTRY WinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    INT         nCmdShow)
{
    DialogBoxW(hInstance, MAKEINTRESOURCEW(1), NULL, DialogProc);
    return 0;
}
