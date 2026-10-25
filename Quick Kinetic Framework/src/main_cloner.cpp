#include "qkf/cloner.hpp"
#include <iostream>

void print_cloner_help() {
    std::cout << "Quick Kinetic Framework (QKF) Project Cloner & Scaffolder v0.1.0\n\n";
    std::cout << "Usage:\n";
    std::cout << "  qkf-cloner init <project_name>   Create a new QKF project with config.gpq\n";
    std::cout << "  qkf-cloner validate [dir]        Validate an existing project's config.gpq\n";
    std::cout << "  qkf-cloner clone <src> <dest>    Clone template or component pack\n";
    std::cout << "  qkf-cloner --help                Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_cloner_help();
        return 0;
    }

    std::string cmd = argv[1];
    qkf::Cloner cloner;

    if (cmd == "--help" || cmd == "-h") {
        print_cloner_help();
        return 0;
    }

    if (cmd == "init") {
        std::string proj_name = (argc >= 3) ? argv[2] : "my_kinetic_app";
        bool ok = cloner.init_project(proj_name, proj_name);
        return ok ? 0 : 1;
    }

    if (cmd == "validate") {
        std::string dir = (argc >= 3) ? argv[2] : ".";
        bool ok = cloner.validate_config(dir);
        return ok ? 0 : 1;
    }

    if (cmd == "clone") {
        if (argc < 4) {
            std::cerr << "Usage: qkf-cloner clone <source> <destination>\n";
            return 1;
        }
        bool ok = cloner.clone_template(argv[2], argv[3]);
        return ok ? 0 : 1;
    }

    std::cerr << "Unknown command: " << cmd << ". Run 'qkf-cloner --help' for options.\n";
    return 1;
}
