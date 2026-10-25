#pragma once

#include "qkf/config_gpq.hpp"
#include <string>

namespace qkf {

class Cloner {
public:
    Cloner() = default;

    // Scaffolds a new QKF project in target_dir
    bool init_project(const std::string& project_name, const std::string& target_dir, const std::string& template_type = "default");

    // Clones remote or local template into existing project
    bool clone_template(const std::string& source, const std::string& destination);

    // Validates config.gpq in directory
    bool validate_config(const std::string& dir_path);

private:
    void create_default_gpq(const std::string& project_name, const std::string& target_dir);
    void create_default_qkf(const std::string& project_name, const std::string& target_dir);
};

} // namespace qkf
