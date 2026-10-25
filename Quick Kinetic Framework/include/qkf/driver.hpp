#pragma once

#include "qkf/config_gpq.hpp"
#include "qkf/ast.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <filesystem>

namespace qkf {

struct CompiledModule {
    std::string filepath;
    std::string module_name;
    std::unique_ptr<ProgramNode> ast;
    std::vector<std::string> imported_files;
};

class CompilationDriver {
public:
    CompilationDriver() = default;

    // Compiles project from config.gpq file
    bool compile_project(const std::string& config_path);

    // Compiles a single entry point file directly
    bool compile_file(const std::string& entry_file);

    // Emits code generator output (HTML webview)
    bool emit_target(const std::string& out_path);

    // Emits native C++ Win32 application source code
    bool emit_native_cpp(const std::string& out_cpp_path);

    // Compiles native Windows executable (.exe) using MinGW GCC
    bool emit_native_app(const std::string& out_exe_path);

    const std::vector<Diagnostic>& diagnostics() const { return all_diagnostics_; }
    const std::unordered_map<std::string, CompiledModule>& modules() const { return modules_; }

private:
    bool process_file(const std::filesystem::path& file_path);
    std::filesystem::path resolve_import(const std::string& import_target, const std::filesystem::path& current_file);

    std::filesystem::path project_root_;
    GpqConfig config_;
    std::unordered_map<std::string, CompiledModule> modules_;
    std::unordered_set<std::string> visited_files_;
    std::vector<Diagnostic> all_diagnostics_;
};

} // namespace qkf
