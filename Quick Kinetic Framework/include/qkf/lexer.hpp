#pragma once

#include "qkf/token.hpp"
#include <vector>
#include <string>

namespace qkf {

class Lexer {
public:
    Lexer(std::string source, std::string filename = "<input>");

    std::vector<Token> tokenize();
    const std::vector<Diagnostic>& diagnostics() const { return diagnostics_; }
    bool has_errors() const;

private:
    char peek() const;
    char peek_next() const;
    char advance();
    bool match(char expected);
    bool is_at_end() const;

    void skip_whitespace_and_comments();
    Token scan_token();
    Token scan_number();
    Token scan_string();
    Token scan_identifier_or_keyword();

    void add_error(const std::string& message);

    std::string source_;
    std::string filename_;
    size_t start_pos_ = 0;
    size_t current_pos_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    size_t token_start_col_ = 1;

    std::vector<Diagnostic> diagnostics_;
};

} // namespace qkf
