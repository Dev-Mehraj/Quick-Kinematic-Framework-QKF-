// =====================================================================
// Quick Kinetic Framework (QKF) - Generated Native Windows C++ App
// =====================================================================

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
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
    inline static Window_AppWindow* s_instance = nullptr;
    std::string title = "Quick Kinetic - Native Layout & Widget Showcase";
    int width = 820;
    int height = 580;
    std::string controls_style = "windows";
    HFONT hFont = NULL;
    std::unordered_map<std::string, HWND> controls_by_id;
    std::unordered_map<std::string, HWND> controls_by_name;

    void register_control(const std::string& id, const std::string& name, HWND h) {
        if (!id.empty()) controls_by_id[id] = h;
        if (!name.empty()) controls_by_name[name] = h;
    }

    HWND getControl(const std::string& id_or_name) {
        auto it = controls_by_id.find(id_or_name);
        if (it != controls_by_id.end()) return it->second;
        auto it2 = controls_by_name.find(id_or_name);
        if (it2 != controls_by_name.end()) return it2->second;
        return NULL;
    }

    std::string getText(const std::string& id_or_name) {
        HWND h = getControl(id_or_name);
        if (!h) return "";
        int len = GetWindowTextLengthA(h);
        if (len <= 0) return "";
        std::vector<char> buf(len + 1, 0);
        GetWindowTextA(h, buf.data(), len + 1);
        return std::string(buf.data());
    }

    void setText(const std::string& id_or_name, const std::string& val) {
        HWND h = getControl(id_or_name);
        if (h) SetWindowTextA(h, val.c_str());
    }

    void clear(const std::string& id_or_name) {
        setText(id_or_name, "");
    }

    bool isChecked(const std::string& id_or_name) {
        HWND h = getControl(id_or_name);
        if (!h) return false;
        return SendMessageA(h, BM_GETCHECK, 0, 0) == BST_CHECKED;
    }

    void setChecked(const std::string& id_or_name, bool chk) {
        HWND h = getControl(id_or_name);
        if (h) SendMessageA(h, BM_SETCHECK, chk ? BST_CHECKED : BST_UNCHECKED, 0);
    }

    void enable(const std::string& id_or_name, bool en) {
        HWND h = getControl(id_or_name);
        if (h) EnableWindow(h, en ? TRUE : FALSE);
    }

    void show(const std::string& id_or_name, bool vis) {
        HWND h = getControl(id_or_name);
        if (h) ShowWindow(h, vis ? SW_SHOW : SW_HIDE);
    }

    void create() {
        HINSTANCE hInst = GetModuleHandle(NULL);
        INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES };
        InitCommonControlsEx(&icex);

        WNDCLASSEXA wc = { sizeof(WNDCLASSEXA) };
        wc.lpfnWndProc = s_WndProc;
        wc.hInstance = hInst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = "QkfWindow_AppWindow";
        RegisterClassExA(&wc);

        DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        hwnd = CreateWindowExA(0, wc.lpszClassName, title.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInst, this);
        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)this);
        s_instance = this;

        hFont = CreateFontA(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        if (!hFont) hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        // Native Menu Bar Construction
        HMENU hMenuBar = CreateMenu();
        HMENU hMenu_1001 = CreatePopupMenu();
        HMENU hSubMenu_1002 = CreatePopupMenu();
        AppendMenuA(hSubMenu_1002, MF_STRING, 1003, "Quick Save");
        AppendMenuA(hMenu_1001, MF_POPUP, (UINT_PTR)hSubMenu_1002, "Save");
        AppendMenuA(hMenu_1001, MF_STRING, 1004, "Exit Application");
        AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hMenu_1001, "File");
        HMENU hMenu_1005 = CreatePopupMenu();
        AppendMenuA(hMenu_1005, MF_STRING, 1006, "Clear All Fields");
        AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)hMenu_1005, "Edit");
        SetMenu(hwnd, hMenuBar);
        // Native Layout Widgets Construction (Column/Row/Buttons/Inputs)
        HWND hCtrl_2001 = CreateWindowExA(0, "STATIC", "Welcome to Quick Kinetic Desktop!", WS_CHILD | WS_VISIBLE | SS_LEFT, 24, 24, 500, 28, hwnd, (HMENU)(UINT_PTR)2001, hInst, NULL);
        if (hCtrl_2001) SendMessageA(hCtrl_2001, WM_SETFONT, (WPARAM)hFont, TRUE);
        register_control("heading_title", "AppHeading", hCtrl_2001);
        HWND hCtrl_2002 = CreateWindowExA(0, "STATIC", "Type your name below and click 'Say Hello' to interact with native Win32 controls:", WS_CHILD | WS_VISIBLE | SS_LEFT, 24, 68, 700, 22, hwnd, (HMENU)(UINT_PTR)2002, hInst, NULL);
        if (hCtrl_2002) SendMessageA(hCtrl_2002, WM_SETFONT, (WPARAM)hFont, TRUE);
        register_control("subtitle", "AppSubtitle", hCtrl_2002);
        HWND hCtrl_2003 = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, 24, 106, 280, 30, hwnd, (HMENU)(UINT_PTR)2003, hInst, NULL);
        if (hCtrl_2003) SendMessageA(hCtrl_2003, WM_SETFONT, (WPARAM)hFont, TRUE);
        if (hCtrl_2003) SendMessageW(hCtrl_2003, EM_SETCUEBANNER, TRUE, (LPARAM)L"Enter your name...");
        register_control("user_input", "UserNameField", hCtrl_2003);
        HWND hCtrl_2004 = CreateWindowExA(0, "BUTTON", "Say Hello", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 316, 106, 140, 30, hwnd, (HMENU)(UINT_PTR)2004, hInst, NULL);
        if (hCtrl_2004) SendMessageA(hCtrl_2004, WM_SETFONT, (WPARAM)hFont, TRUE);
        register_control("greet_btn", "GreetButton", hCtrl_2004);
        HWND hCtrl_2005 = CreateWindowExA(0, "BUTTON", "Clear", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 468, 106, 100, 30, hwnd, (HMENU)(UINT_PTR)2005, hInst, NULL);
        if (hCtrl_2005) SendMessageA(hCtrl_2005, WM_SETFONT, (WPARAM)hFont, TRUE);
        register_control("reset_btn", "ResetButton", hCtrl_2005);
        HWND hCtrl_2006 = CreateWindowExA(0, "STATIC", "Status: Ready.", WS_CHILD | WS_VISIBLE | SS_LEFT, 24, 152, 600, 24, hwnd, (HMENU)(UINT_PTR)2006, hInst, NULL);
        if (hCtrl_2006) SendMessageA(hCtrl_2006, WM_SETFONT, (WPARAM)hFont, TRUE);
        register_control("greeting_label", "GreetingDisplay", hCtrl_2006);
        HWND hCtrl_2007 = CreateWindowExA(0, "BUTTON", "Enable desktop notification sounds", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 24, 192, 320, 26, hwnd, (HMENU)(UINT_PTR)2007, hInst, NULL);
        if (hCtrl_2007) SendMessageA(hCtrl_2007, WM_SETFONT, (WPARAM)hFont, TRUE);
        if (hCtrl_2007) SendMessageA(hCtrl_2007, BM_SETCHECK, BST_CHECKED, 0);
        register_control("notifications_chk", "EnableNotifications", hCtrl_2007);
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
        if (!pThis) pThis = s_instance;
        Window_AppWindow& AppWindow = *pThis;
        switch (msg) {
            case WM_COMMAND: {
                int cmdId = LOWORD(wParam);
                int notifyCode = HIWORD(wParam);
                if (cmdId == 1003) {
                    qkf_print("Menu item clicked: Quick Save");
                    qkf_print(std::string("Menu: Quick Save executed!"));
                    return 0;
                }
                if (cmdId == 1004) {
                    qkf_print("Menu item clicked: Exit Application");
                    qkf_print(std::string("Exiting application."));
                    AppWindow.close();
                    return 0;
                }
                if (cmdId == 1006) {
                    qkf_print("Menu item clicked: Clear All Fields");
                    AppWindow.setText(std::string("user_input"), std::string(""));
                    AppWindow.setText(std::string("greeting_label"), std::string("Form cleared."));
                    AppWindow.setChecked(std::string("notifications_chk"), false);
                    return 0;
                }
                if (cmdId == 2004) {
                    qkf_print("Action triggered on Button: greet_btn (GreetButton)");
                    auto entered_name = AppWindow.getText(std::string("user_input"));
                    qkf_print((std::string("User entered: ") + entered_name));
                    AppWindow.setText(std::string("greeting_label"), ((std::string("Hello, ") + entered_name) + std::string("! Welcome to native QKF desktop.")));
                    return 0;
                }
                if (cmdId == 2005) {
                    qkf_print("Action triggered on Button: reset_btn (ResetButton)");
                    AppWindow.setText(std::string("user_input"), std::string(""));
                    AppWindow.setText(std::string("greeting_label"), std::string("Input cleared."));
                    return 0;
                }
                if (cmdId == 2007) {
                    qkf_print("Action triggered on Checkbox: notifications_chk (EnableNotifications)");
                    auto enabled = AppWindow.isChecked(std::string("notifications_chk"));
                    if (enabled)                     {
                        qkf_print(std::string("Notifications enabled!"));
                    }
                    else                     {
                        qkf_print(std::string("Notifications disabled!"));
                    }
                    return 0;
                }
                break;
            }
            case WM_DESTROY:
                if (pThis && pThis->hFont) DeleteObject(pThis->hFont);
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
};

static Window_AppWindow AppWindow;

void qkf_user_main() {
    qkf_print(std::string("Starting Quick Kinetic Interactive Native Desktop App..."));
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

