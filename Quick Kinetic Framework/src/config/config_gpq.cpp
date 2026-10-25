#include "qkf/config_gpq.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace qkf {

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static std::string strip_quotes_and_semicolon(std::string val) {
    val = trim(val);
    if (!val.empty() && val.back() == ';') {
        val.pop_back();
        val = trim(val);
    }
    if (val.size() >= 2 && ((val.front() == '"' && val.back() == '"') || (val.front() == '\'' && val.back() == '\''))) {
        val = val.substr(1, val.size() - 2);
    }
    return val;
}

GpqConfig GpqConfig::parse_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        GpqConfig cfg;
        cfg.diagnostics_.push_back({
            DiagnosticLevel::Error,
            {filepath, 1, 1},
            "Failed to open config.gpq file: " + filepath
        });
        return cfg;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parse_string(buffer.str());
}

GpqConfig GpqConfig::parse_string(const std::string& content) {
    GpqConfig cfg;
    std::istringstream stream(content);
    std::string line;
    std::string current_section = "global";
    size_t line_num = 0;

    while (std::getline(stream, line)) {
        line_num++;
        std::string trimmed = trim(line);

        if (trimmed.empty() || trimmed.rfind("#", 0) == 0 || trimmed.rfind("//", 0) == 0) {
            continue; // comment or empty
        }

        // Section header [section_name]
        if (trimmed.front() == '[' && trimmed.back() == ']') {
            current_section = trim(trimmed.substr(1, trimmed.size() - 2));
            cfg.sections_[current_section].name = current_section;
            continue;
        }

        // Key = Value;
        auto eq_pos = trimmed.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = trim(trimmed.substr(0, eq_pos));
            std::string raw_val = trimmed.substr(eq_pos + 1);

            // Semicolon check
            if (raw_val.find(';') == std::string::npos) {
                cfg.diagnostics_.push_back({
                    DiagnosticLevel::Warning,
                    {"config.gpq", line_num, 1},
                    "Missing semicolon ';' after value for key '" + key + "'."
                });
            }

            std::string val = strip_quotes_and_semicolon(raw_val);
            cfg.sections_[current_section].entries[key] = val;
        }
    }

    return cfg;
}

bool GpqConfig::has_section(const std::string& section) const {
    return sections_.find(section) != sections_.end();
}

std::string GpqConfig::get(const std::string& section, const std::string& key, const std::string& default_val) const {
    auto sec_it = sections_.find(section);
    if (sec_it != sections_.end()) {
        auto key_it = sec_it->second.entries.find(key);
        if (key_it != sec_it->second.entries.end()) {
            return key_it->second;
        }
    }
    return default_val;
}

void GpqConfig::set(const std::string& section, const std::string& key, const std::string& val) {
    sections_[section].name = section;
    sections_[section].entries[key] = val;
}

std::string GpqConfig::serialize() const {
    std::ostringstream ss;
    for (const auto& [sec_name, sec] : sections_) {
        ss << "[" << sec_name << "]\n";
        for (const auto& [key, val] : sec.entries) {
            ss << "    " << key << " = \"" << val << "\";\n";
        }
        ss << "\n";
    }
    return ss.str();
}

void GpqConfig::save(const std::string& filepath) const {
    std::ofstream out(filepath);
    out << serialize();
}

} // namespace qkf
