#include "qkf/driver.hpp"
#include "qkf/token.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void print_help() {
    std::cout << "Quick Kinetic Framework (QKF) Native App Compiler v0.1.0\n\n";
    std::cout << "Usage:\n";
    std::cout << "  qkf                           Compile project in current dir using config.gpq\n";
    std::cout << "  qkf config.gpq                Compile project defined in config.gpq\n";
    std::cout << "  qkf build [file.qkf|config]   Build native Windows desktop app (.exe)\n";
    std::cout << "  qkf run [file.qkf|config]     Build and immediately launch the native desktop app\n";
    std::cout << "  qkf check [file.qkf|config]   Validate syntax & strict semicolons across all imports\n";
    std::cout << "  qkf keywords                  List all registered language keywords\n";
    std::cout << "  qkf --version                 Show version info\n";
    std::cout << "  qkf --help                    Show this help message\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <path>                     Specify output path (e.g. -o bin/MyApp.exe)\n";
    std::cout << "  --cpp <path>                  Emit generated C++ source code only\n";
}

int main(int argc, char* argv[]) {
    // 1. If called without args: check for config.gpq in current working directory
    if (argc == 1) {
        if (fs::exists("config.gpq")) {
            qkf::CompilationDriver driver;
            bool ok = driver.compile_project("config.gpq");
            if (ok) {
                std::cout << "\n[BUILD SUCCESSFUL] All modules and imports parsed with strict semicolons!\n";
                driver.emit_native_app("dist/app.exe");
                return 0;
            }
            return 1;
        } else {
            print_help();
            return 0;
        }
    }

    std::string arg1 = argv[1];

    if (arg1 == "--help" || arg1 == "-h") {
        print_help();
        return 0;
    }

    if (arg1 == "--version" || arg1 == "-v") {
        std::cout << "Quick Kinetic Framework (QKF) Native App Maker v0.1.0 (MinGW C++20)\n";
        return 0;
    }

    if (arg1 == "keywords") {
        std::cout << "Registered Keywords in QKF Engine:\n";
        for (const auto& [kw, type] : qkf::KeywordRegistry::instance().get_all_keywords()) {
            std::cout << "  - " << kw << " (" << qkf::token_type_name(type) << ")\n";
        }
        return 0;
    }

    // Determine command & target file
    std::string command = "build";
    std::string target_file = "";

    if (arg1 == "build" || arg1 == "run" || arg1 == "check") {
        command = arg1;
        if (argc >= 3 && argv[2][0] != '-') {
            target_file = argv[2];
        } else if (fs::exists("config.gpq")) {
            target_file = "config.gpq";
        } else {
            std::cerr << "Error: Please specify a file or config.gpq to " << command << ".\n";
            return 1;
        }
    } else {
        target_file = arg1;
    }

    std::string out_exe = "dist/app.exe";
    std::string out_cpp = "";
    std::string out_html = "";

    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "-o" && i + 1 < argc) {
            std::string val = argv[++i];
            if (val.ends_with(".html") || val.ends_with(".htm")) {
                out_html = val;
            } else if (val.ends_with(".cpp")) {
                out_cpp = val;
            } else {
                out_exe = val;
            }
        } else if (flag == "--cpp" && i + 1 < argc) {
            out_cpp = argv[++i];
        } else if (flag == "--html" && i + 1 < argc) {
            out_html = argv[++i];
        }
    }

    qkf::CompilationDriver driver;
    bool success = false;

    // Check whether the target is a .gpq or .qkf
    if (target_file.ends_with(".gpq") || target_file == "config.gpq" || fs::path(target_file).filename() == "config.gpq") {
        success = driver.compile_project(target_file);
    } else {
        success = driver.compile_file(target_file);
    }

    if (success) {
        std::cout << "\n[BUILD SUCCESSFUL] " << driver.modules().size() 
                  << " module(s) compiled. Strict semicolons fully verified!\n";

        if (command == "check") {
            return 0;
        }

        if (!out_cpp.empty()) {
            driver.emit_native_cpp(out_cpp);
            std::cout << "[EMITTED] Generated C++ source: " << fs::absolute(out_cpp).string() << "\n";
        }

        if (!out_html.empty()) {
            driver.emit_target(out_html);
            std::cout << "[EMITTED] Generated HTML artifact: " << fs::absolute(out_html).string() << "\n";
        }

        if (out_cpp.empty() && out_html.empty()) {
            bool built = driver.emit_native_app(out_exe);
            if (!built) {
                return 1;
            }

            if (command == "run") {
                std::cout << "[LAUNCHING] Starting Native Application: " << fs::absolute(out_exe).string() << "\n";
                std::string launch_cmd = "start \"\" \"" + fs::absolute(out_exe).string() + "\"";
                std::system(launch_cmd.c_str());
            }
        }
        return 0;
    } else {
        std::cerr << "\n[BUILD FAILED] One or more errors occurred during compilation.\n";
        return 1;
    }
}
