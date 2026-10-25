#pragma once

#include "qkf/common.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace qkf {

struct GpqSection {
    std::string name;
    std::unordered_map<std::string, std::string> entries;
};

class GpqConfig {
public:
    GpqConfig() = default;

    static GpqConfig parse_file(const std::string& filepath);
    static GpqConfig parse_string(const std::string& content);

    bool has_section(const std::string& section) const;
    std::string get(const std::string& section, const std::string& key, const std::string& default_val = "") const;
    void set(const std::string& section, const std::string& key, const std::string& val);

    void save(const std::string& filepath) const;
    std::string serialize() const;

    const std::vector<Diagnostic>& diagnostics() const { return diagnostics_; }

private:
    std::unordered_map<std::string, GpqSection> sections_;
    std::vector<Diagnostic> diagnostics_;
};

} // namespace qkf
