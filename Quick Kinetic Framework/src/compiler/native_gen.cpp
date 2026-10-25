#include "qkf/native_gen.hpp"
#include <iostream>

namespace qkf {

NativeAppGenerator::NativeAppGenerator() = default;

void NativeAppGenerator::indent() {
    for (int i = 0; i < indent_level_; ++i) {
        out_ << "    ";
    }
}

std::string NativeAppGenerator::generate_cpp(ProgramNode& program) {
    out_.str("");
    out_.clear();
    indent_level_ = 0;
    menu_id_counter_ = 1000;
    registered_menu_actions_.clear();

    out_ << "// =====================================================================\n";
    out_ << "// Quick Kinetic Framework (QKF) - Generated Native Windows C++ App\n";
    out_ << "// =====================================================================\n\n";
    out_ << "#define WIN32_LEAN_AND_MEAN\n";
    out_ << "#include <windows.h>\n";
    out_ << "#include <windowsx.h>\n";
    out_ << "#include <iostream>\n";
    out_ << "#include <string>\n";
    out_ << "#include <vector>\n";
    out_ << "#include <functional>\n";
    out_ << "#include <memory>\n";
    out_ << "#include <sstream>\n\n";

    // Standard I/O helper functions for QKF apps
    out_ << "// Native QKF I/O & Logging Runtimes\n";
    out_ << "static void qkf_print(const std::string& msg) {\n";
    out_ << "    std::cout << \"[QKF APP] \" << msg << \"\\n\";\n";
    out_ << "    OutputDebugStringA((\"[QKF APP] \" + msg + \"\\n\").c_str());\n";
    out_ << "}\n\n";

    out_ << "static void qkf_warnlog(const std::string& msg) {\n";
    out_ << "    std::cout << \"[QKF WARN] \" << msg << \"\\n\";\n";
    out_ << "    OutputDebugStringA((\"[QKF WARN] \" + msg + \"\\n\").c_str());\n";
    out_ << "}\n\n";

    out_ << "static void qkf_errorlog(const std::string& msg) {\n";
    out_ << "    std::cerr << \"[QKF ERROR] \" << msg << \"\\n\";\n";
    out_ << "    OutputDebugStringA((\"[QKF ERROR] \" + msg + \"\\n\").c_str());\n";
    out_ << "    MessageBoxA(NULL, msg.c_str(), \"Application Error\", MB_ICONERROR | MB_OK);\n";
    out_ << "}\n\n";

    out_ << "static std::string qkf_input(const std::string& prompt) {\n";
    out_ << "    std::cout << prompt;\n";
    out_ << "    std::string line;\n";
    out_ << "    std::getline(std::cin, line);\n";
    out_ << "    return line;\n";
    out_ << "}\n\n";

    program.accept(*this);

    return out_.str();
}

void NativeAppGenerator::visit(LiteralExpr& node) {
    if (node.token.type == TokenType::StringLiteral) {
        out_ << "std::string(\"" << node.token.text << "\")";
    } else {
        out_ << node.token.text;
    }
}

void NativeAppGenerator::visit(IdentifierExpr& node) {
    out_ << node.name;
}

void NativeAppGenerator::visit(BinaryExpr& node) {
    out_ << "(";
    if (node.left) node.left->accept(*this);
    out_ << " " << node.op.text << " ";
    if (node.right) node.right->accept(*this);
    out_ << ")";
}

void NativeAppGenerator::visit(ScopeResolutionExpr& node) {
    out_ << node.scope << "::" << node.member;
}

void NativeAppGenerator::visit(FunctionCallExpr& node) {
    auto id = dynamic_cast<IdentifierExpr*>(node.callee.get());
    if (id) {
        if (id->name == "main") {
            out_ << "qkf_user_main(";
        } else if (id->name == "print") {
            out_ << "qkf_print(";
        } else if (id->name == "warnlog") {
            out_ << "qkf_warnlog(";
        } else if (id->name == "errorlog") {
            out_ << "qkf_errorlog(";
        } else if (id->name == "input") {
            out_ << "qkf_input(";
        } else {
            out_ << id->name << "(";
        }
    } else {
        if (node.callee) node.callee->accept(*this);
        out_ << "(";
    }

    for (size_t i = 0; i < node.args.size(); ++i) {
        if (node.args[i]) node.args[i]->accept(*this);
        if (i + 1 < node.args.size()) out_ << ", ";
    }
    out_ << ")";
}

void NativeAppGenerator::visit(ArrayLiteralExpr& node) {
    out_ << "{";
    for (size_t i = 0; i < node.elements.size(); ++i) {
        if (node.elements[i]) node.elements[i]->accept(*this);
        if (i + 1 < node.elements.size()) out_ << ", ";
    }
    out_ << "}";
}

void NativeAppGenerator::visit(IndexExpr& node) {
    if (node.target) node.target->accept(*this);
    out_ << "[";
    if (node.index) node.index->accept(*this);
    out_ << "]";
}

void NativeAppGenerator::visit(MemberAccessExpr& node) {
    if (node.target) node.target->accept(*this);
    out_ << "." << node.member;
}

void NativeAppGenerator::visit(PropertyNode& node) {
    indent();
    out_ << "// property: " << node.name << "\n";
}

void NativeAppGenerator::visit(UIElementNode& node) {
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void NativeAppGenerator::visit(ExprStmt& node) {
    indent();
    if (node.expr) node.expr->accept(*this);
    out_ << ";\n";
}

void NativeAppGenerator::visit(VarDeclStmt& node) {
    indent();
    std::string t = node.type_name == "any" ? "auto" : node.type_name;
    out_ << t << " " << node.name << " = ";
    if (node.init) node.init->accept(*this);
    out_ << ";\n";
}

void NativeAppGenerator::visit(BlockStmt& node) {
    indent();
    out_ << "{\n";
    indent_level_++;
    for (const auto& s : node.statements) {
        s->accept(*this);
    }
    if (node.tail_expr) {
        indent();
        out_ << "return ";
        node.tail_expr->accept(*this);
        out_ << ";\n";
    }
    indent_level_--;
    indent();
    out_ << "}\n";
}

void NativeAppGenerator::visit(IfStmt& node) {
    indent();
    out_ << "if (";
    if (node.condition) node.condition->accept(*this);
    out_ << ") ";
    if (node.then_branch) node.then_branch->accept(*this);
    if (node.else_branch) {
        indent();
        out_ << "else ";
        node.else_branch->accept(*this);
    }
}

void NativeAppGenerator::visit(WhileStmt& node) {
    indent();
    out_ << "while (";
    if (node.condition) node.condition->accept(*this);
    out_ << ") ";
    if (node.body) node.body->accept(*this);
}

void NativeAppGenerator::visit(DoWhileStmt& node) {
    indent();
    out_ << "do ";
    if (node.body) node.body->accept(*this);
    indent();
    out_ << "while (";
    if (node.condition) node.condition->accept(*this);
    out_ << ");\n";
}

void NativeAppGenerator::visit(ForStmt& node) {
    indent();
    if (node.is_for_in) {
        out_ << "for (auto& " << node.iterator_name << " : ";
        if (node.collection) node.collection->accept(*this);
        out_ << ") ";
    } else {
        out_ << "for (";
        if (node.init) {
            auto v = dynamic_cast<VarDeclStmt*>(node.init.get());
            if (v) {
                out_ << "auto " << v->name << " = ";
                if (v->init) v->init->accept(*this);
            }
        }
        out_ << "; ";
        if (node.condition) node.condition->accept(*this);
        out_ << "; ";
        if (node.increment) node.increment->accept(*this);
        out_ << ") ";
    }
    if (node.body) node.body->accept(*this);
}

void NativeAppGenerator::visit(BreakStmt&) {
    indent();
    out_ << "break;\n";
}

void NativeAppGenerator::visit(ContinueStmt&) {
    indent();
    out_ << "continue;\n";
}

void NativeAppGenerator::visit(TryCatchStmt& node) {
    indent();
    out_ << "try ";
    if (node.try_block) node.try_block->accept(*this);
    for (const auto& c : node.catch_clauses) {
        indent();
        out_ << "catch (const std::exception& " << (c.error_var.empty() ? "e" : c.error_var) << ") ";
        if (c.body) c.body->accept(*this);
    }
}

void NativeAppGenerator::visit(ThrowStmt& node) {
    indent();
    out_ << "throw std::runtime_error(";
    if (node.expr) node.expr->accept(*this);
    out_ << ");\n";
}

void NativeAppGenerator::visit(FunctionDecl& node) {
    indent();
    std::string fn_name = (node.name == "main") ? "qkf_user_main" : node.name;
    std::string ret = node.is_void ? "void" : (node.return_type.empty() ? "void" : node.return_type);
    out_ << ret << " " << fn_name << "(";
    for (size_t i = 0; i < node.params.size(); ++i) {
        std::string p_type = node.params[i].type_name == "any" ? "auto" : node.params[i].type_name;
        out_ << p_type << " " << node.params[i].name;
        if (i + 1 < node.params.size()) out_ << ", ";
    }
    out_ << ") {\n";
    indent_level_++;

    for (const auto& s : node.statements) {
        s->accept(*this);
    }
    if (node.return_expr) {
        indent();
        out_ << "return ";
        node.return_expr->accept(*this);
        out_ << ";\n";
    }

    indent_level_--;
    indent();
    out_ << "}\n\n";
}

void NativeAppGenerator::visit(ImportDecl& node) {
    out_ << "// import \"" << node.module_path << "\"\n";
}

void NativeAppGenerator::visit(TakeDecl& node) {
    out_ << "// take from \"" << node.source_file << "\"\n";
}

void NativeAppGenerator::visit(NamespaceDecl& node) {
    indent();
    out_ << "namespace " << node.name << " {\n";
    indent_level_++;
    for (const auto& d : node.declarations) {
        d->accept(*this);
    }
    indent_level_--;
    indent();
    out_ << "} // namespace " << node.name << "\n\n";
}

void NativeAppGenerator::visit(EnumDecl& node) {
    indent();
    out_ << "enum class " << node.name << " {\n";
    indent_level_++;
    for (size_t i = 0; i < node.items.size(); ++i) {
        indent();
        out_ << node.items[i].name;
        if (node.items[i].value) {
            out_ << " = ";
            node.items[i].value->accept(*this);
        }
        if (i + 1 < node.items.size()) out_ << ",";
        out_ << "\n";
    }
    indent_level_--;
    indent();
    out_ << "};\n\n";
}

void NativeAppGenerator::visit(ClassMemberDecl& node) {
    if (node.method) {
        node.method->accept(*this);
    } else {
        indent();
        if (node.is_static) out_ << "static ";
        std::string t = node.type_name == "any" ? "auto" : node.type_name;
        out_ << t << " " << node.member_name;
        if (node.default_val) {
            out_ << " = ";
            node.default_val->accept(*this);
        }
        out_ << ";\n";
    }
}

void NativeAppGenerator::visit(ClassDecl& node) {
    indent();
    out_ << "class " << node.name;
    if (!node.base_class.empty()) {
        out_ << " : public " << node.base_class;
    }
    out_ << " {\npublic:\n";
    indent_level_++;
    for (const auto& m : node.members) {
        m->accept(*this);
    }
    indent_level_--;
    indent();
    out_ << "};\n\n";
}

void NativeAppGenerator::visit(StateDecl& node) {
    indent();
    out_ << "auto state_" << node.name << " = ";
    if (node.initial_value) {
        node.initial_value->accept(*this);
    } else {
        out_ << "0";
    }
    out_ << ";\n";
}

static void collect_menu_items(const UIElementNode& elem, int& counter, 
                               std::vector<std::string>& menu_stmts,
                               std::string parent_hmenu,
                               std::vector<NativeAppGenerator::MenuAction>& actions) {
    for (const auto& child : elem.children) {
        std::string label = child->component_name;
        PropertyNode* onclick = nullptr;
        for (const auto& p : child->properties) {
            if (p->name == "label" || p->name == "title") {
                auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
                if (lit) label = lit->token.text;
            }
            if (p->name == "onClick" || p->name == "onclick") {
                onclick = p.get();
            }
        }

        if (child->children.empty()) {
            // Leaf menu item with action!
            int id = ++counter;
            menu_stmts.push_back("AppendMenuA(" + parent_hmenu + ", MF_STRING, " + std::to_string(id) + ", \"" + label + "\");");
            actions.push_back({id, label, onclick});
        } else {
            // Submenu
            std::string sub_name = "hSubMenu_" + std::to_string(++counter);
            menu_stmts.push_back("HMENU " + sub_name + " = CreatePopupMenu();");
            collect_menu_items(*child, counter, menu_stmts, sub_name, actions);
            menu_stmts.push_back("AppendMenuA(" + parent_hmenu + ", MF_POPUP, (UINT_PTR)" + sub_name + ", \"" + label + "\");");
        }
    }
}

void NativeAppGenerator::visit(WindowDecl& node) {
    std::string win_title = node.name;
    int win_width = 1000;
    int win_height = 700;
    std::string controls_style = "windows"; // "windows", "macos", "linux"

    for (const auto& p : node.properties) {
        if (p->name == "title") {
            auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
            if (lit) win_title = lit->token.text;
        } else if (p->name == "width") {
            auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
            if (lit) win_width = std::stoi(lit->token.text);
        } else if (p->name == "height") {
            auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
            if (lit) win_height = std::stoi(lit->token.text);
        }
    }

    // Inspect elements for WindowControls style and Toolbar menus
    std::vector<std::string> menu_build_stmts;
    for (const auto& elem : node.root_elements) {
        if (elem->component_name == "TitleBar" || elem->component_name == "titlebar") {
            for (const auto& c : elem->children) {
                if (c->component_name == "WindowControls") {
                    for (const auto& cp : c->properties) {
                        if (cp->name == "style" || cp->name == "preset") {
                            auto lit = dynamic_cast<LiteralExpr*>(cp->value.get());
                            if (lit) controls_style = lit->token.text;
                        }
                    }
                }
            }
        }
        if (elem->component_name == "WindowControls") {
            for (const auto& cp : elem->properties) {
                if (cp->name == "style" || cp->name == "preset") {
                    auto lit = dynamic_cast<LiteralExpr*>(cp->value.get());
                    if (lit) controls_style = lit->token.text;
                }
            }
        }
        if (elem->component_name == "Toolbar" || elem->component_name == "toolbar") {
            menu_build_stmts.push_back("HMENU hMenuBar = CreateMenu();");
            for (const auto& top_item : elem->children) {
                std::string top_label = top_item->component_name;
                for (const auto& p : top_item->properties) {
                    if (p->name == "label" || p->name == "title") {
                        auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
                        if (lit) top_label = lit->token.text;
                    }
                }
                std::string top_menu_var = "hMenu_" + std::to_string(++menu_id_counter_);
                menu_build_stmts.push_back("HMENU " + top_menu_var + " = CreatePopupMenu();");
                collect_menu_items(*top_item, menu_id_counter_, menu_build_stmts, top_menu_var, registered_menu_actions_);
                menu_build_stmts.push_back("AppendMenuA(hMenuBar, MF_POPUP, (UINT_PTR)" + top_menu_var + ", \"" + top_label + "\");");
            }
            menu_build_stmts.push_back("SetMenu(hwnd, hMenuBar);");
        }
    }

    out_ << "// Native Win32 Window Class for: " << node.name << "\n";
    out_ << "class Window_" << node.name << " {\n";
    out_ << "public:\n";
    out_ << "    HWND hwnd = NULL;\n";
    out_ << "    std::string title = \"" << win_title << "\";\n";
    out_ << "    int width = " << win_width << ";\n";
    out_ << "    int height = " << win_height << ";\n";
    out_ << "    std::string controls_style = \"" << controls_style << "\";\n\n";

    // User-declared custom methods
    for (const auto& d : node.declarations) {
        d->accept(*this);
    }

    out_ << "    void create() {\n";
    out_ << "        HINSTANCE hInst = GetModuleHandle(NULL);\n";
    out_ << "        WNDCLASSEXA wc = { sizeof(WNDCLASSEXA) };\n";
    out_ << "        wc.lpfnWndProc = s_WndProc;\n";
    out_ << "        wc.hInstance = hInst;\n";
    out_ << "        wc.hCursor = LoadCursor(NULL, IDC_ARROW);\n";
    out_ << "        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);\n";
    out_ << "        wc.lpszClassName = \"QkfWindow_" << node.name << "\";\n";
    out_ << "        RegisterClassExA(&wc);\n\n";

    if (controls_style == "macos") {
        out_ << "        // macOS-style borderless window with custom top-left traffic lights\n";
        out_ << "        DWORD style = WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;\n";
        out_ << "        hwnd = CreateWindowExA(WS_EX_APPWINDOW, wc.lpszClassName, title.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInst, this);\n";
    } else {
        out_ << "        // Windows native style window with native top-right Minimize, Maximize, Close buttons\n";
        out_ << "        DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;\n";
        out_ << "        hwnd = CreateWindowExA(0, wc.lpszClassName, title.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInst, this);\n";
    }

    out_ << "        SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)this);\n\n";

    // Build real native menus
    if (!menu_build_stmts.empty()) {
        out_ << "        // Native Menu Bar Construction\n";
        for (const auto& stmt : menu_build_stmts) {
            out_ << "        " << stmt << "\n";
        }
    }

    out_ << "    }\n\n";

    out_ << "    void start() {\n";
    out_ << "        if (!hwnd) create();\n";
    out_ << "        ShowWindow(hwnd, SW_SHOW);\n";
    out_ << "        UpdateWindow(hwnd);\n";
    out_ << "        MSG msg;\n";
    out_ << "        while (GetMessageA(&msg, NULL, 0, 0)) {\n";
    out_ << "            TranslateMessage(&msg);\n";
    out_ << "            DispatchMessageA(&msg);\n";
    out_ << "        }\n";
    out_ << "    }\n\n";

    out_ << "    void close() { if (hwnd) DestroyWindow(hwnd); }\n";
    out_ << "    void maximize() { if (hwnd) ShowWindow(hwnd, SW_MAXIMIZE); }\n";
    out_ << "    void minimize() { if (hwnd) ShowWindow(hwnd, SW_MINIMIZE); }\n";
    out_ << "    void restore() { if (hwnd) ShowWindow(hwnd, SW_RESTORE); }\n";
    out_ << "    void setTitle(const std::string& t) { title = t; if (hwnd) SetWindowTextA(hwnd, t.c_str()); }\n";
    out_ << "    void setSize(int w, int h) { width = w; height = h; if (hwnd) SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER); }\n";
    out_ << "    void setPosition(int x, int y) { if (hwnd) SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }\n";
    out_ << "    void center() {\n";
    out_ << "        int screenW = GetSystemMetrics(SM_CXSCREEN);\n";
    out_ << "        int screenH = GetSystemMetrics(SM_CYSCREEN);\n";
    out_ << "        setPosition((screenW - width) / 2, (screenH - height) / 2);\n";
    out_ << "    }\n\n";

    // Real Native Window Procedure
    out_ << "    static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {\n";
    out_ << "        Window_" << node.name << "* pThis = (Window_" << node.name << "*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);\n";
    out_ << "        switch (msg) {\n";
    out_ << "            case WM_COMMAND: {\n";
    out_ << "                int cmdId = LOWORD(wParam);\n";
    for (const auto& act : registered_menu_actions_) {
        out_ << "                if (cmdId == " << act.id << ") {\n";
        out_ << "                    qkf_print(\"Menu clicked: " << act.label << "\");\n";
        if (act.onclick && act.onclick->block_value) {
            for (const auto& s : act.onclick->block_value->statements) {
                auto expr_s = dynamic_cast<ExprStmt*>(s.get());
                if (expr_s && expr_s->expr) {
                    auto call = dynamic_cast<FunctionCallExpr*>(expr_s->expr.get());
                    auto id = call ? dynamic_cast<IdentifierExpr*>(call->callee.get()) : nullptr;
                    if (id && id->name == "print") {
                        out_ << "                    qkf_print(";
                        for (size_t a = 0; a < call->args.size(); ++a) {
                            auto lit_arg = dynamic_cast<LiteralExpr*>(call->args[a].get());
                            if (lit_arg) out_ << "\"" << lit_arg->token.text << "\"";
                            if (a + 1 < call->args.size()) out_ << " + ";
                        }
                        out_ << ");\n";
                    }
                }
            }
        }
        out_ << "                    return 0;\n";
        out_ << "                }\n";
    }
    out_ << "                break;\n";
    out_ << "            }\n";

    if (controls_style == "macos") {
        out_ << "            case WM_PAINT: {\n";
        out_ << "                PAINTSTRUCT ps;\n";
        out_ << "                HDC hdc = BeginPaint(hwnd, &ps);\n";
        out_ << "                // Draw macOS titlebar and traffic lights\n";
        out_ << "                RECT barRect = { 0, 0, pThis ? pThis->width : 800, 32 };\n";
        out_ << "                HBRUSH barBrush = CreateSolidBrush(RGB(30, 41, 59));\n";
        out_ << "                FillRect(hdc, &barRect, barBrush);\n";
        out_ << "                DeleteObject(barBrush);\n";
        out_ << "                // Traffic Lights on top-left: Red(Close), Yellow(Min), Green(Max)\n";
        out_ << "                HBRUSH red = CreateSolidBrush(RGB(255, 95, 86));\n";
        out_ << "                HBRUSH yellow = CreateSolidBrush(RGB(255, 189, 46));\n";
        out_ << "                HBRUSH green = CreateSolidBrush(RGB(39, 201, 63));\n";
        out_ << "                SelectObject(hdc, GetStockObject(NULL_PEN));\n";
        out_ << "                SelectObject(hdc, red); Ellipse(hdc, 12, 10, 24, 22);\n";
        out_ << "                SelectObject(hdc, yellow); Ellipse(hdc, 30, 10, 42, 22);\n";
        out_ << "                SelectObject(hdc, green); Ellipse(hdc, 48, 10, 60, 22);\n";
        out_ << "                DeleteObject(red); DeleteObject(yellow); DeleteObject(green);\n";
        out_ << "                EndPaint(hwnd, &ps);\n";
        out_ << "                return 0;\n";
        out_ << "            }\n";
        out_ << "            case WM_LBUTTONDOWN: {\n";
        out_ << "                int x = GET_X_LPARAM(lParam);\n";
        out_ << "                int y = GET_Y_LPARAM(lParam);\n";
        out_ << "                if (y >= 10 && y <= 22) {\n";
        out_ << "                    if (x >= 12 && x <= 24) { DestroyWindow(hwnd); return 0; }\n";
        out_ << "                    if (x >= 30 && x <= 42) { ShowWindow(hwnd, SW_MINIMIZE); return 0; }\n";
        out_ << "                    if (x >= 48 && x <= 60) { ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE); return 0; }\n";
        out_ << "                }\n";
        out_ << "                if (y < 32) { ReleaseCapture(); SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0); return 0; }\n";
        out_ << "                break;\n";
        out_ << "            }\n";
    }

    out_ << "            case WM_DESTROY:\n";
    out_ << "                PostQuitMessage(0);\n";
    out_ << "                return 0;\n";
    out_ << "        }\n";
    out_ << "        return DefWindowProcA(hwnd, msg, wParam, lParam);\n";
    out_ << "    }\n";
    out_ << "};\n\n";
    out_ << "static Window_" << node.name << " " << node.name << ";\n\n";
}

void NativeAppGenerator::visit(PageDecl& node) {
    for (const auto& d : node.declarations) d->accept(*this);
    for (const auto& e : node.root_elements) e->accept(*this);
}

void NativeAppGenerator::visit(ProgramNode& node) {
    for (const auto& d : node.declarations) {
        d->accept(*this);
    }

    // Windows GUI Subsystem entry point
    out_ << "int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {\n";
    out_ << "    // Initialize and run QKF main application\n";
    out_ << "    qkf_user_main();\n";
    out_ << "    return 0;\n";
    out_ << "}\n\n";

    // Console Subsystem entry point fallback
    out_ << "int main(int argc, char* argv[]) {\n";
    out_ << "    qkf_user_main();\n";
    out_ << "    return 0;\n";
    out_ << "}\n";
}

} // namespace qkf
