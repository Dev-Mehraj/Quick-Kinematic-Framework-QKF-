// =====================================================================
// Quick Kinetic Framework (QKF) - Generated Native Windows C++ App
// =====================================================================

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <sstream>

// Native QKF I/O & Logging Runtimes
static void qkf_print(const std::string& msg) {
    std::cout << "[QKF APP] " << msg << "\n";
    OutputDebugStringA(("[QKF APP] " + msg + "\n").c_str());
}

static void qkf_warnlog(const std::string& msg) {
    std::cout << "[QKF WARN] " << msg << "\n";
    OutputDebugStringA(("[QKF WARN] " + msg + "\n").c_str());
}

static void qkf_errorlog(const std::string& msg) {
    std::cerr << "[QKF ERROR] " << msg << "\n";
    OutputDebugStringA(("[QKF ERROR] " + msg + "\n").c_str());
    MessageBoxA(NULL, msg.c_str(), "Application Error", MB_ICONERROR | MB_OK);
}

static std::string qkf_input(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

// Native Win32 Window Class for: AppWindow
class Window_AppWindow {
public:
    HWND hwnd = NULL;
    std::string title = "Quick Kinetic Native App";
    int width = 960;
    int height = 640;
    std::string controls_style = "windows";

    void create() {
        HINSTANCE hInst = GetModuleHandle(NULL);
        WNDCLASSEXA wc = { sizeof(WNDCLASSEXA) };
        wc.lpfnWndProc = s_WndProc;
        wc.hInstance = hInst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "QkfWindow_AppWindow";
        RegisterClassExA(&wc);

        // Windows native style window with native top-right Minimize, Maximize, Close buttons
        DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        hwnd = CreateWindowExA(0, wc.lpszClassName, title.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInst, this);
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)this);

        // Native Menu Bar Construction
        HMENU hMenuBar = CreateMenu();
        HMENU hMenu_1001 = CreatePopupMenu();
        HMENU hSubMenu_1002 = CreatePopupMenu();
        AppendMenuA(hSubMenu_1002, MF_STRING, 1003, "Quick Save");
        AppendMenuA(hSubMenu_1002, MF_STRING, 1004, "Export All...");
        AppendMenuA(hMenu_1001, MF_POPUP, (UINT_PTR)hSubMenu_1002, "Save");
        AppendMenuA(hMenu_1001, MF_STRING, 1005, "Exit Application");
        AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hMenu_1001, "File");
        HMENU hMenu_1006 = CreatePopupMenu();
        AppendMenuA(hMenu_1006, MF_STRING, 1007, "Undo");
        AppendMenuA(hMenu_1006, MF_STRING, 1008, "Redo");
        AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hMenu_1006, "Edit");
        HMENU hMenu_1009 = CreatePopupMenu();
        AppendMenuA(hMenu_1009, MF_STRING, 1010, "Preferences...");
        AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hMenu_1009, "Settings");
        SetMenu(hwnd, hMenuBar);
    }

    void start() {
        if (!hwnd) create();
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        MSG msg;
        while (GetMessageA(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    void close() { if (hwnd) DestroyWindow(hwnd); }
    void maximize() { if (hwnd) ShowWindow(hwnd, SW_MAXIMIZE); }
    void minimize() { if (hwnd) ShowWindow(hwnd, SW_MINIMIZE); }
    void restore() { if (hwnd) ShowWindow(hwnd, SW_RESTORE); }
    void setTitle(const std::string& t) { title = t; if (hwnd) SetWindowTextA(hwnd, t.c_str()); }
    void setSize(int w, int h) { width = w; height = h; if (hwnd) SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); }
    void setPosition(int x, int y) { if (hwnd) SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }
    void center() {
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);
        setPosition((screenW - width) / 2, (screenH - height) / 2);
    }

    static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window_AppWindow* pThis = (Window_AppWindow*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
        switch (msg) {
            case WM_COMMAND: {
                int cmdId = LOWORD(wParam);
                if (cmdId == 1003) {
                    qkf_print("Menu clicked: Quick Save");
                    qkf_print("Quick Save action executed natively!");
                    return 0;
                }
                if (cmdId == 1004) {
                    qkf_print("Menu clicked: Export All...");
                    qkf_print("Export All triggered!");
                    return 0;
                }
                if (cmdId == 1005) {
                    qkf_print("Menu clicked: Exit Application");
                    qkf_print("Exiting application.");
                    return 0;
                }
                if (cmdId == 1007) {
                    qkf_print("Menu clicked: Undo");
                    qkf_print("Undo action executed!");
                    return 0;
                }
                if (cmdId == 1008) {
                    qkf_print("Menu clicked: Redo");
                    qkf_print("Redo action executed!");
                    return 0;
                }
                if (cmdId == 1010) {
                    qkf_print("Menu clicked: Preferences...");
                    qkf_print("Preferences opened.");
                    return 0;
                }
                break;
            }
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
};

static Window_AppWindow AppWindow;

void qkf_user_main() {
    qkf_print(std::string("Launching Quick Kinetic Native Application..."));
    AppWindow.create();
    AppWindow.center();
    AppWindow.start();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize and run QKF main application
    qkf_user_main();
    return 0;
}

int main(int argc, char* argv[]) {
    qkf_user_main();
    return 0;
}

