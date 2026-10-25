#include "qkf/cloner.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace qkf {

bool Cloner::init_project(const std::string& project_name, const std::string& target_dir, const std::string& template_type) {
    fs::path base_path = target_dir.empty() ? fs::current_path() / project_name : fs::path(target_dir);

    try {
        if (!fs::exists(base_path)) {
            fs::create_directories(base_path);
        }

        fs::create_directories(base_path / "src");
        fs::create_directories(base_path / "assets");

        create_default_gpq(project_name, base_path.string());
        create_default_qkf(project_name, base_path.string());

        std::cout << "[CLONER] Successfully scaffolded new QKF project: " << project_name << "\n";
        std::cout << "         Path: " << fs::absolute(base_path).string() << "\n";
        std::cout << "         Template: " << template_type << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[CLONER ERROR] Failed to initialize project: " << e.what() << "\n";
        return false;
    }
}

void Cloner::create_default_gpq(const std::string& project_name, const std::string& target_dir) {
    fs::path gpq_path = fs::path(target_dir) / "config.gpq";
    if (fs::exists(gpq_path)) return;

    std::ofstream out(gpq_path);
    out << "# Quick Kinetic Framework Project Configuration\n";
    out << "[project]\n";
    out << "    name = \"" << project_name << "\";\n";
    out << "    version = \"0.1.0\";\n";
    out << "    entry = \"src/main.qkf\";\n\n";
    out << "[build]\n";
    out << "    target = \"webview\";\n";
    out << "    strict_semicolon = \"true\";\n\n";
    out << "[cloner]\n";
    out << "    template = \"standard-app\";\n";
    out << "    scaffold_version = \"1.0.0\";\n";
}

void Cloner::create_default_qkf(const std::string& project_name, const std::string& target_dir) {
    fs::path qkf_path = fs::path(target_dir) / "src" / "main.qkf";
    if (fs::exists(qkf_path)) return;

    std::ofstream out(qkf_path);
    out << "// " << project_name << " - Entry Point\n";
    out << "import ui.core;\n\n";
    out << "page MainWindow {\n";
    out << "    state count = 0;\n\n";
    out << "    Column {\n";
    out << "        spacing: 12;\n";
    out << "        padding: 24;\n\n";
    out << "        Text {\n";
    out << "            content: \"Welcome to " << project_name << " (QKF)\";\n";
    out << "        };\n\n";
    out << "        Button {\n";
    out << "            label: \"Click Me\";\n";
    out << "        };\n";
    out << "    };\n";
    out << "};\n";
}

bool Cloner::clone_template(const std::string& source, const std::string& destination) {
    std::cout << "[CLONER] Cloning template from " << source << " to " << destination << "...\n";
    return init_project("ClonedApp", destination, source);
}

bool Cloner::validate_config(const std::string& dir_path) {
    fs::path gpq_path = fs::path(dir_path) / "config.gpq";
    if (!fs::exists(gpq_path)) {
        std::cerr << "[CLONER ERROR] config.gpq not found in " << dir_path << "\n";
        return false;
    }

    auto cfg = GpqConfig::parse_file(gpq_path.string());
    std::cout << "[CLONER] Validating project: " << cfg.get("project", "name", "<unnamed>") << "\n";
    for (const auto& d : cfg.diagnostics()) {
        d.print();
    }
    return true;
}

} // namespace qkf
