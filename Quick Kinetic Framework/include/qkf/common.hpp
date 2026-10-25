#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <format>

namespace qkf {

struct SourceLocation {
    std::string filename;
    size_t line = 1;
    size_t column = 1;

    std::string to_string() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

enum class DiagnosticLevel {
    Info,
    Warning,
    Error
};

struct Diagnostic {
    DiagnosticLevel level;
    SourceLocation location;
    std::string message;

    void print() const {
        const char* prefix = "[INFO]";
        if (level == DiagnosticLevel::Warning) prefix = "[WARNING]";
        if (level == DiagnosticLevel::Error)   prefix = "[ERROR]";

        std::cerr << prefix << " " << location.to_string() << ": " << message << "\n";
    }
};

} // namespace qkf
