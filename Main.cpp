#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_STANDARD 1002
#define ID_TRAY_SUPER 1003
#define ID_TRAY_INSANE 1004

const int DEFAULT_SPEED = 20;
const int DEFAULT_DELAY = 1;
const int SUPER_SPEED = 31;
const int INSANE_REPEAT = 10;
const int INSANE_DELAY = 150;

void disableFilterKeys() {
    FILTERKEYS fk = { sizeof(FILTERKEYS), 0 };
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

void setStandard(int speed, int delay) {
    disableFilterKeys();
    SystemParametersInfo(SPI_SETKEYBOARDSPEED, speed, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
    SystemParametersInfo(SPI_SETKEYBOARDDELAY, delay, 0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

void setInsane() {
    FILTERKEYS fk = { 0 };
    fk.cbSize = sizeof(FILTERKEYS);
    fk.dwFlags = FKF_FILTERKEYSON | FKF_AVAILABLE;
    fk.iWaitMSec = 0;
    fk.iDelayMSec = INSANE_DELAY;
    fk.iRepeatMSec = INSANE_REPEAT;
    fk.iBounceMSec = 0;
    SystemParametersInfo(SPI_SETFILTERKEYS, sizeof(FILTERKEYS), &fk, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_TRAYICON) {
        if (lParam == WM_RBUTTONUP) {
            POINT curPoint;
            GetCursorPos(&curPoint);
            HMENU hMenu = CreatePopupMenu();
            AppendMenuA(hMenu, MF_STRING, ID_TRAY_STANDARD, "Standard (20/1)");
            AppendMenuA(hMenu, MF_STRING, ID_TRAY_SUPER, "Super Fast (31/0)");
            AppendMenuA(hMenu, MF_STRING, ID_TRAY_INSANE, "INSANE (100Hz)");
            AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuA(hMenu, MF_STRING, ID_TRAY_EXIT, "Restore & Exit");
            SetForegroundWindow(hWnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, curPoint.x, curPoint.y, 0, hWnd, NULL);
            DestroyMenu(hMenu);
        }
    }
    else if (message == WM_COMMAND) {
        switch (LOWORD(wParam)) {
        case ID_TRAY_STANDARD: setStandard(DEFAULT_SPEED, DEFAULT_DELAY); Beep(440, 100); break;
        case ID_TRAY_SUPER:    setStandard(SUPER_SPEED, 0); Beep(880, 100); break;
        case ID_TRAY_INSANE:   setInsane(); Beep(1200, 150); break;
        case ID_TRAY_EXIT:     setStandard(DEFAULT_SPEED, DEFAULT_DELAY); PostQuitMessage(0); break;
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "KeyRateDockClass";
    RegisterClassA(&wc);

    HWND hWnd = CreateWindowA("KeyRateDockClass", NULL, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    NOTIFYICONDATAA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATAA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    lstrcpyA(nid.szTip, "Keyboard Overdrive");
    Shell_NotifyIconA(NIM_ADD, &nid);

    RegisterHotKey(hWnd, 1, MOD_ALT, VK_F9);
    RegisterHotKey(hWnd, 2, MOD_ALT, VK_F10);
    RegisterHotKey(hWnd, 3, MOD_ALT, VK_F11);
    RegisterHotKey(hWnd, 4, MOD_ALT, VK_F12);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Shell_NotifyIconA(NIM_DELETE, &nid);
    return 0;
}