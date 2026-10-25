#include "qkf/driver.hpp"
#include "qkf/lexer.hpp"
#include "qkf/parser.hpp"
#include "qkf/codegen.hpp"
#include "qkf/native_gen.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

namespace qkf {

bool CompilationDriver::compile_project(const std::string& config_path) {
    fs::path cfg_file = fs::absolute(config_path);
    if (!fs::exists(cfg_file)) {
        std::cerr << "[QKF ERROR] Config file not found: " << config_path << "\n";
        return false;
    }

    project_root_ = cfg_file.parent_path();
    config_ = GpqConfig::parse_file(cfg_file.string());

    for (const auto& d : config_.diagnostics()) {
        d.print();
    }

    std::string entry = config_.get("project", "entry", "");
    if (entry.empty()) {
        std::cerr << "[QKF ERROR] Missing 'entry' property under [project] section in " << config_path << "\n";
        return false;
    }

    std::string proj_name = config_.get("project", "name", "UnnamedProject");
    std::string version = config_.get("project", "version", "1.0.0");

    std::cout << "\n=========================================\n";
    std::cout << " Quick Kinetic Framework (QKF) Compiler\n";
    std::cout << " Building: " << proj_name << " (v" << version << ")\n";
    std::cout << " Config:   " << cfg_file.string() << "\n";
    std::cout << " Entry:    " << entry << "\n";
    std::cout << "=========================================\n\n";

    fs::path entry_full_path = project_root_ / entry;
    if (!fs::exists(entry_full_path) && fs::exists(entry)) {
        entry_full_path = fs::absolute(entry);
    }
    return process_file(entry_full_path);
}

bool CompilationDriver::compile_file(const std::string& entry_file) {
    fs::path file_path = fs::absolute(entry_file);
    project_root_ = file_path.parent_path();
    return process_file(file_path);
}

bool CompilationDriver::emit_target(const std::string& out_path) {
    std::ofstream out(out_path);
    if (!out.is_open()) {
        std::cerr << "[ERROR] Cannot open output file for writing: " << out_path << "\n";
        return false;
    }

    CodeGenerator codegen(CodeGenerator::Target::HtmlWebview);
    for (auto& [path, mod] : modules_) {
        if (mod.ast) {
            std::string code = codegen.generate(*mod.ast);
            out << code << "\n";
        }
    }
    return true;
}

bool CompilationDriver::emit_native_cpp(const std::string& out_cpp_path) {
    fs::path p = fs::path(out_cpp_path).parent_path();
    if (!p.empty() && !fs::exists(p)) {
        fs::create_directories(p);
    }

    std::ofstream out(out_cpp_path);
    if (!out.is_open()) {
        std::cerr << "[ERROR] Cannot open output C++ file: " << out_cpp_path << "\n";
        return false;
    }

    NativeAppGenerator generator;
    for (auto& [path, mod] : modules_) {
        if (mod.ast) {
            std::string code = generator.generate_cpp(*mod.ast);
            out << code << "\n";
        }
    }
    return true;
}

bool CompilationDriver::emit_native_app(const std::string& out_exe_path) {
    fs::path exe_p = fs::path(out_exe_path);
    if (!exe_p.parent_path().empty() && !fs::exists(exe_p.parent_path())) {
        fs::create_directories(exe_p.parent_path());
    }

    // Write temp C++ source file
    fs::path temp_cpp = exe_p.parent_path().empty() ? fs::path("temp_native_app.cpp") : (exe_p.parent_path() / "temp_native_app.cpp");
    if (!emit_native_cpp(temp_cpp.string())) {
        return false;
    }

    // Compile with MinGW g++ using Windows GUI subsystem (-mwindows) and Common Controls
    std::string cmd = "g++ -std=c++20 -O2 \"" + temp_cpp.string() + "\" -mwindows -lcomctl32 -o \"" + exe_p.string() + "\"";
    std::cout << "[NATIVE BUILD] Compiling native Windows binary: " << exe_p.string() << " ...\n";
    int ret = std::system(cmd.c_str());

    if (ret != 0) {
        std::cerr << "[QKF ERROR] Failed to compile native executable with g++ (Exit code: " << ret << ")\n";
        return false;
    }

    std::cout << "[SUCCESS] Native Desktop Application built: " << fs::absolute(exe_p).string() << "\n";
    return true;
}

std::filesystem::path CompilationDriver::resolve_import(const std::string& import_target, const std::filesystem::path& current_file) {
    // If it's a dot notation like "components.header", convert to "components/header.qkf"
    std::string path_str = import_target;
    for (char& c : path_str) {
        if (c == '.') c = '/';
    }

    // Try relative to current file directory
    fs::path cand1 = current_file.parent_path() / (path_str + ".qkf");
    if (fs::exists(cand1)) return fs::canonical(cand1);

    // Try relative to project root
    fs::path cand2 = project_root_ / (path_str + ".qkf");
    if (fs::exists(cand2)) return fs::canonical(cand2);

    // Try relative to project root / src
    fs::path cand3 = project_root_ / "src" / (path_str + ".qkf");
    if (fs::exists(cand3)) return fs::canonical(cand3);

    // Direct filename if user imported with quotes e.g. "my_comp.qkf"
    fs::path cand4 = current_file.parent_path() / import_target;
    if (fs::exists(cand4)) return fs::canonical(cand4);

    return {};
}

bool CompilationDriver::process_file(const std::filesystem::path& file_path) {
    std::string canonical_str;
    try {
        if (!fs::exists(file_path)) {
            std::cerr << "[ERROR] Source file not found: " << file_path.string() << "\n";
            return false;
        }
        canonical_str = fs::canonical(file_path).string();
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Invalid path: " << file_path.string() << " (" << e.what() << ")\n";
        return false;
    }

    // Check if already processed (prevent cyclic imports)
    if (visited_files_.find(canonical_str) != visited_files_.end()) {
        return true;
    }
    visited_files_.insert(canonical_str);

    std::cout << "[COMPILE] Scanning: " << file_path.filename().string() 
              << " (" << canonical_str << ")...\n";

    std::ifstream file(canonical_str);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open: " << canonical_str << "\n";
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source_code = buffer.str();

    // 1. Lexing
    Lexer lexer(source_code, file_path.filename().string());
    auto tokens = lexer.tokenize();

    if (lexer.has_errors()) {
        std::cerr << "\n[Lexer Errors in " << file_path.filename().string() << "]:\n";
        for (const auto& d : lexer.diagnostics()) {
            d.print();
            all_diagnostics_.push_back(d);
        }
        return false;
    }

    // 2. Parsing with strict semicolons
    Parser parser(tokens);
    auto ast = parser.parse_program();

    if (parser.has_errors()) {
        std::cerr << "\n[Strict Semicolon / Syntax Errors in " << file_path.filename().string() << "]:\n";
        for (const auto& d : parser.diagnostics()) {
            d.print();
            all_diagnostics_.push_back(d);
        }
        return false;
    }

    std::cout << "          -> OK: Syntax & strict semicolons verified.\n";

    // 3. Inspect Imports and recurse
    std::vector<std::string> imports_to_visit;
    for (const auto& decl : ast->declarations) {
        auto import_decl = dynamic_cast<ImportDecl*>(decl.get());
        if (import_decl) {
            std::string mod_name = import_decl->module_path;

            // Built-in standard library modules (e.g. ui.core)
            if (mod_name.rfind("ui.", 0) == 0 || mod_name.rfind("std.", 0) == 0) {
                std::cout << "          -> [Stdlib Import]: " << mod_name << "\n";
                continue;
            }

            fs::path resolved = resolve_import(mod_name, file_path);
            if (resolved.empty()) {
                std::cerr << "[ERROR] Cannot resolve imported module '" << mod_name 
                          << "' referenced in " << file_path.filename().string() << "\n";
                return false;
            }

            imports_to_visit.push_back(resolved.string());
        }

        auto take_decl = dynamic_cast<TakeDecl*>(decl.get());
        if (take_decl) {
            std::string src_file = take_decl->source_file;
            std::cout << "          -> [Take Selective]: '";
            for (size_t i = 0; i < take_decl->items.size(); ++i) {
                std::cout << take_decl->items[i].symbol_name;
                if (!take_decl->items[i].alias.empty()) {
                    std::cout << " as " << take_decl->items[i].alias;
                }
                if (i + 1 < take_decl->items.size()) std::cout << ", ";
            }
            std::cout << "' from \"" << src_file << "\"\n";

            // If not stdlib, resolve file
            if (src_file.rfind("ui.", 0) != 0 && src_file.rfind("std.", 0) != 0) {
                fs::path resolved = resolve_import(src_file, file_path);
                if (resolved.empty()) {
                    std::cerr << "[ERROR] Cannot resolve module in take statement '" << src_file
                              << "' referenced in " << file_path.filename().string() << "\n";
                    return false;
                }
                imports_to_visit.push_back(resolved.string());
            }
        }
    }

    CompiledModule mod;
    mod.filepath = canonical_str;
    mod.module_name = file_path.stem().string();
    mod.ast = std::move(ast);
    mod.imported_files = imports_to_visit;
    modules_[canonical_str] = std::move(mod);

    // Recursively process imported modules
    for (const auto& next_file : imports_to_visit) {
        if (!process_file(fs::path(next_file))) {
            return false;
        }
    }

    return true;
}

} // namespace qkf
