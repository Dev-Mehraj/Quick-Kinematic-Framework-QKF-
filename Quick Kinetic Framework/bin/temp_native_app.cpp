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

// Native Win32 Window Class for: MacWindow
class Window_MacWindow {
public:
    HWND hwnd = NULL;
    std::string title = "Quick Kinetic macOS Style Window";
    int width = 900;
    int height = 550;
    std::string controls_style = "macos";

    void create() {
        HINSTANCE hInst = GetModuleHandle(NULL);
        WNDCLASSEXA wc = { sizeof(WNDCLASSEXA) };
        wc.lpfnWndProc = s_WndProc;
        wc.hInstance = hInst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "QkfWindow_MacWindow";
        RegisterClassExA(&wc);

        // macOS-style borderless window with custom top-left traffic lights
        DWORD style = WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
        hwnd = CreateWindowExA(WS_EX_APPWINDOW, wc.lpszClassName, title.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInst, this);
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)this);

        // Native Menu Bar Construction
        HMENU hMenuBar = CreateMenu();
        HMENU hMenu_1001 = CreatePopupMenu();
        HMENU hSubMenu_1002 = CreatePopupMenu();
        AppendMenuA(hSubMenu_1002, MF_STRING, 1003, "Quick Save");
        AppendMenuA(hMenu_1001, MF_POPUP, (UINT_PTR)hSubMenu_1002, "Save");
        AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hMenu_1001, "File");
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
        Window_MacWindow* pThis = (Window_MacWindow*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
        switch (msg) {
            case WM_COMMAND: {
                int cmdId = LOWORD(wParam);
                if (cmdId == 1003) {
                    qkf_print("Menu clicked: Quick Save");
                    qkf_print("Quick Save on macOS style window executed!");
                    return 0;
                }
                break;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                // Draw macOS titlebar and traffic lights
                RECT barRect = { 0, 0, pThis ? pThis->width : 800, 32 };
                HBRUSH barBrush = CreateSolidBrush(RGB(30, 41, 59));
                FillRect(hdc, &barRect, barBrush);
                DeleteObject(barBrush);
                // Traffic Lights on top-left: Red(Close), Yellow(Min), Green(Max)
                HBRUSH red = CreateSolidBrush(RGB(255, 95, 86));
                HBRUSH yellow = CreateSolidBrush(RGB(255, 189, 46));
                HBRUSH green = CreateSolidBrush(RGB(39, 201, 63));
                SelectObject(hdc, GetStockObject(NULL_PEN));
                SelectObject(hdc, red); Ellipse(hdc, 12, 10, 24, 22);
                SelectObject(hdc, yellow); Ellipse(hdc, 30, 10, 42, 22);
                SelectObject(hdc, green); Ellipse(hdc, 48, 10, 60, 22);
                DeleteObject(red); DeleteObject(yellow); DeleteObject(green);
                EndPaint(hwnd, &ps);
                return 0;
            }
            case WM_LBUTTONDOWN: {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                if (y >= 10 && y <= 22) {
                    if (x >= 12 && x <= 24) { DestroyWindow(hwnd); return 0; }
                    if (x >= 30 && x <= 42) { ShowWindow(hwnd, SW_MINIMIZE); return 0; }
                    if (x >= 48 && x <= 60) { ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE); return 0; }
                }
                if (y < 32) { ReleaseCapture(); SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); return 0; }
                break;
            }
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
};

static Window_MacWindow MacWindow;

void qkf_user_main() {
    qkf_print(std::string("Launching macOS style desktop application..."));
    MacWindow.create();
    MacWindow.center();
    MacWindow.start();
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

