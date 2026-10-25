#include "qkf/lexer.hpp"
#include <cctype>

namespace qkf {

std::string token_type_name(TokenType type) {
    switch (type) {
        case TokenType::Eof: return "Eof";
        case TokenType::Invalid: return "Invalid";
        case TokenType::Identifier: return "Identifier";
        case TokenType::NumberLiteral: return "NumberLiteral";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::BooleanLiteral: return "BooleanLiteral";
        case TokenType::Semicolon: return "Semicolon (';')";
        case TokenType::Colon: return "Colon (':')";
        case TokenType::Comma: return "Comma (',')";
        case TokenType::Dot: return "Dot ('.')";
        case TokenType::OpenBrace: return "OpenBrace ('{')";
        case TokenType::CloseBrace: return "CloseBrace ('}')";
        case TokenType::OpenParen: return "OpenParen ('(')";
        case TokenType::CloseParen: return "CloseParen (')')";
        case TokenType::OpenBracket: return "OpenBracket ('[')";
        case TokenType::CloseBracket: return "CloseBracket (']')";
        case TokenType::Equal: return "Equal ('=')";
        case TokenType::Plus: return "Plus ('+')";
        case TokenType::Minus: return "Minus ('-')";
        case TokenType::Star: return "Star ('*')";
        case TokenType::Slash: return "Slash ('/')";
        case TokenType::Percent: return "Percent ('%')";
        case TokenType::EqualEqual: return "EqualEqual ('==')";
        case TokenType::ExclamationEqual: return "ExclamationEqual ('!=')";
        case TokenType::LessThan: return "LessThan ('<')";
        case TokenType::GreaterThan: return "GreaterThan ('>')";
        case TokenType::LessEqual: return "LessEqual ('<=')";
        case TokenType::GreaterEqual: return "GreaterEqual ('>=')";
        case TokenType::AmpersandAmpersand: return "AmpersandAmpersand ('&&')";
        case TokenType::PipePipe: return "PipePipe ('||')";
        case TokenType::Exclamation: return "Exclamation ('!')";
        case TokenType::Arrow: return "Arrow ('->')";
        case TokenType::FatArrow: return "FatArrow ('=>')";
        case TokenType::ColonColon: return "ColonColon ('::')";
        case TokenType::KeywordCustom: return "KeywordCustom";
        case TokenType::KwF: return "Keyword 'f'";
        case TokenType::KwVoid: return "Keyword 'void'";
        case TokenType::KwLet: return "Keyword 'let'";
        case TokenType::KwTake: return "Keyword 'take'";
        case TokenType::KwFrom: return "Keyword 'from'";
        case TokenType::KwUse: return "Keyword 'use'";
        case TokenType::KwImport: return "Keyword 'import'";
        case TokenType::KwClass: return "Keyword 'class'";
        case TokenType::KwStruct: return "Keyword 'struct'";
        case TokenType::KwNamespace: return "Keyword 'namespace'";
        case TokenType::KwEnum: return "Keyword 'enum'";
        case TokenType::KwTemplate: return "Keyword 'template'";
        case TokenType::KwTypename: return "Keyword 'typename'";
        case TokenType::KwPublic: return "Keyword 'public'";
        case TokenType::KwPrivate: return "Keyword 'private'";
        case TokenType::KwProtected: return "Keyword 'protected'";
        case TokenType::KwStatic: return "Keyword 'static'";
        case TokenType::KwConst: return "Keyword 'const'";
        case TokenType::KwPage: return "Keyword 'page'";
        case TokenType::KwComponent: return "Keyword 'component'";
        case TokenType::KwState: return "Keyword 'state'";
        case TokenType::KwAction: return "Keyword 'action'";
        case TokenType::PlusPlus: return "PlusPlus ('++')";
        case TokenType::MinusMinus: return "MinusMinus ('--')";
        case TokenType::PlusEqual: return "PlusEqual ('+=')";
        case TokenType::MinusEqual: return "MinusEqual ('-=')";
        case TokenType::StarEqual: return "StarEqual ('*=')";
        case TokenType::SlashEqual: return "SlashEqual ('/=')";
        case TokenType::KwWhile: return "Keyword 'while'";
        case TokenType::KwDo: return "Keyword 'do'";
        case TokenType::KwBreak: return "Keyword 'break'";
        case TokenType::KwContinue: return "Keyword 'continue'";
        case TokenType::KwIn: return "Keyword 'in'";
        case TokenType::KwIf: return "Keyword 'if'";
        case TokenType::KwElse: return "Keyword 'else'";
        case TokenType::KwFor: return "Keyword 'for'";
        case TokenType::KwReturn: return "Keyword 'return'";
        case TokenType::KwTry: return "Keyword 'try'";
        case TokenType::KwCatch: return "Keyword 'catch'";
        case TokenType::KwFinally: return "Keyword 'finally'";
        case TokenType::KwThrow: return "Keyword 'throw'";
        case TokenType::KwWindow: return "Keyword 'window'";
    }
    return "Unknown";
}

KeywordRegistry::KeywordRegistry() {
    keywords_["f"] = TokenType::KwF;
    keywords_["void"] = TokenType::KwVoid;
    keywords_["let"] = TokenType::KwLet;
    keywords_["take"] = TokenType::KwTake;
    keywords_["from"] = TokenType::KwFrom;
    keywords_["use"] = TokenType::KwUse;
    keywords_["import"] = TokenType::KwImport;
    keywords_["class"] = TokenType::KwClass;
    keywords_["struct"] = TokenType::KwStruct;
    keywords_["namespace"] = TokenType::KwNamespace;
    keywords_["enum"] = TokenType::KwEnum;
    keywords_["template"] = TokenType::KwTemplate;
    keywords_["typename"] = TokenType::KwTypename;
    keywords_["public"] = TokenType::KwPublic;
    keywords_["private"] = TokenType::KwPrivate;
    keywords_["protected"] = TokenType::KwProtected;
    keywords_["static"] = TokenType::KwStatic;
    keywords_["const"] = TokenType::KwConst;
    keywords_["page"] = TokenType::KwPage;
    keywords_["component"] = TokenType::KwComponent;
    keywords_["state"] = TokenType::KwState;
    keywords_["action"] = TokenType::KwAction;
    keywords_["if"] = TokenType::KwIf;
    keywords_["else"] = TokenType::KwElse;
    keywords_["while"] = TokenType::KwWhile;
    keywords_["do"] = TokenType::KwDo;
    keywords_["for"] = TokenType::KwFor;
    keywords_["in"] = TokenType::KwIn;
    keywords_["break"] = TokenType::KwBreak;
    keywords_["continue"] = TokenType::KwContinue;
    keywords_["return"] = TokenType::KwReturn;
    keywords_["try"] = TokenType::KwTry;
    keywords_["catch"] = TokenType::KwCatch;
    keywords_["finally"] = TokenType::KwFinally;
    keywords_["throw"] = TokenType::KwThrow;
    keywords_["window"] = TokenType::KwWindow;
}

KeywordRegistry& KeywordRegistry::instance() {
    static KeywordRegistry reg;
    return reg;
}

void KeywordRegistry::register_keyword(const std::string& name, TokenType type) {
    keywords_[name] = type;
}

bool KeywordRegistry::is_keyword(const std::string& text, TokenType& out_type) const {
    auto it = keywords_.find(text);
    if (it != keywords_.end()) {
        out_type = it->second;
        return true;
    }
    return false;
}

const std::unordered_map<std::string, TokenType>& KeywordRegistry::get_all_keywords() const {
    return keywords_;
}

Lexer::Lexer(std::string source, std::string filename)
    : source_(std::move(source)), filename_(std::move(filename)) {}

bool Lexer::has_errors() const {
    for (const auto& d : diagnostics_) {
        if (d.level == DiagnosticLevel::Error) return true;
    }
    return false;
}

char Lexer::peek() const {
    if (is_at_end()) return '\0';
    return source_[current_pos_];
}

char Lexer::peek_next() const {
    if (current_pos_ + 1 >= source_.size()) return '\0';
    return source_[current_pos_ + 1];
}

char Lexer::advance() {
    char c = source_[current_pos_++];
    column_++;
    return c;
}

bool Lexer::match(char expected) {
    if (is_at_end() || source_[current_pos_] != expected) return false;
    current_pos_++;
    column_++;
    return true;
}

bool Lexer::is_at_end() const {
    return current_pos_ >= source_.size();
}

void Lexer::add_error(const std::string& message) {
    diagnostics_.push_back({
        DiagnosticLevel::Error,
        {filename_, line_, token_start_col_},
        message
    });
}

void Lexer::skip_whitespace_and_comments() {
    while (!is_at_end()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            line_++;
            column_ = 1;
            advance();
        } else if (c == '/' && peek_next() == '/') {
            // Single-line comment //
            while (!is_at_end() && peek() != '\n') {
                advance();
            }
        } else if (c == '/' && peek_next() == '*') {
            // Block comment /* ... */
            advance(); // '/'
            advance(); // '*'
            while (!is_at_end()) {
                if (peek() == '\n') {
                    line_++;
                    column_ = 1;
                }
                if (peek() == '*' && peek_next() == '/') {
                    advance(); // '*'
                    advance(); // '/'
                    break;
                }
                advance();
            }
        } else {
            break;
        }
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!is_at_end()) {
        skip_whitespace_and_comments();
        if (is_at_end()) break;

        start_pos_ = current_pos_;
        token_start_col_ = column_;
        tokens.push_back(scan_token());
    }

    tokens.push_back({TokenType::Eof, "", {filename_, line_, column_}});
    return tokens;
}

Token Lexer::scan_token() {
    char c = advance();

    switch (c) {
        case ';': return {TokenType::Semicolon, ";", {filename_, line_, token_start_col_}};
        case ':':
            if (match(':')) return {TokenType::ColonColon, "::", {filename_, line_, token_start_col_}};
            return {TokenType::Colon, ":", {filename_, line_, token_start_col_}};
        case ',': return {TokenType::Comma, ",", {filename_, line_, token_start_col_}};
        case '.': return {TokenType::Dot, ".", {filename_, line_, token_start_col_}};
        case '{': return {TokenType::OpenBrace, "{", {filename_, line_, token_start_col_}};
        case '}': return {TokenType::CloseBrace, "}", {filename_, line_, token_start_col_}};
        case '(': return {TokenType::OpenParen, "(", {filename_, line_, token_start_col_}};
        case ')': return {TokenType::CloseParen, ")", {filename_, line_, token_start_col_}};
        case '[': return {TokenType::OpenBracket, "[", {filename_, line_, token_start_col_}};
        case ']': return {TokenType::CloseBracket, "]", {filename_, line_, token_start_col_}};
        case '+':
            if (match('+')) return {TokenType::PlusPlus, "++", {filename_, line_, token_start_col_}};
            if (match('=')) return {TokenType::PlusEqual, "+=", {filename_, line_, token_start_col_}};
            return {TokenType::Plus, "+", {filename_, line_, token_start_col_}};
        case '-': 
            if (match('-')) return {TokenType::MinusMinus, "--", {filename_, line_, token_start_col_}};
            if (match('=')) return {TokenType::MinusEqual, "-=", {filename_, line_, token_start_col_}};
            if (match('>')) return {TokenType::Arrow, "->", {filename_, line_, token_start_col_}};
            return {TokenType::Minus, "-", {filename_, line_, token_start_col_}};
        case '*':
            if (match('=')) return {TokenType::StarEqual, "*=", {filename_, line_, token_start_col_}};
            return {TokenType::Star, "*", {filename_, line_, token_start_col_}};
        case '/':
            if (match('=')) return {TokenType::SlashEqual, "/=", {filename_, line_, token_start_col_}};
            return {TokenType::Slash, "/", {filename_, line_, token_start_col_}};
        case '%': return {TokenType::Percent, "%", {filename_, line_, token_start_col_}};
        case '=':
            if (match('=')) return {TokenType::EqualEqual, "==", {filename_, line_, token_start_col_}};
            if (match('>')) return {TokenType::FatArrow, "=>", {filename_, line_, token_start_col_}};
            return {TokenType::Equal, "=", {filename_, line_, token_start_col_}};
        case '!':
            if (match('=')) return {TokenType::ExclamationEqual, "!=", {filename_, line_, token_start_col_}};
            return {TokenType::Exclamation, "!", {filename_, line_, token_start_col_}};
        case '<':
            if (match('=')) return {TokenType::LessEqual, "<=", {filename_, line_, token_start_col_}};
            return {TokenType::LessThan, "<", {filename_, line_, token_start_col_}};
        case '>':
            if (match('=')) return {TokenType::GreaterEqual, ">=", {filename_, line_, token_start_col_}};
            return {TokenType::GreaterThan, ">", {filename_, line_, token_start_col_}};
        case '&':
            if (match('&')) return {TokenType::AmpersandAmpersand, "&&", {filename_, line_, token_start_col_}};
            break;
        case '|':
            if (match('|')) return {TokenType::PipePipe, "||", {filename_, line_, token_start_col_}};
            break;
        case '"':
        case '\'':
            return scan_string();
        default:
            if (std::isdigit(static_cast<unsigned char>(c))) {
                return scan_number();
            }
            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                return scan_identifier_or_keyword();
            }
            break;
    }

    add_error(std::string("Unexpected character: '") + c + "'");
    return {TokenType::Invalid, std::string(1, c), {filename_, line_, token_start_col_}};
}

Token Lexer::scan_number() {
    while (std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }
    if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peek_next()))) {
        advance(); // consume '.'
        while (std::isdigit(static_cast<unsigned char>(peek()))) {
            advance();
        }
    }
    std::string text = source_.substr(start_pos_, current_pos_ - start_pos_);
    return {TokenType::NumberLiteral, text, {filename_, line_, token_start_col_}};
}

Token Lexer::scan_string() {
    char quote_char = source_[start_pos_];
    std::string result;

    while (!is_at_end() && peek() != quote_char) {
        if (peek() == '\n') {
            line_++;
            column_ = 1;
        }
        if (peek() == '\\' && peek_next() != '\0') {
            advance(); // consume '\'
            char escaped = advance();
            switch (escaped) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += escaped; break;
            }
        } else {
            result += advance();
        }
    }

    if (is_at_end()) {
        add_error("Unterminated string literal.");
        return {TokenType::Invalid, result, {filename_, line_, token_start_col_}};
    }

    advance(); // Closing quote
    return {TokenType::StringLiteral, result, {filename_, line_, token_start_col_}};
}

Token Lexer::scan_identifier_or_keyword() {
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        advance();
    }
    std::string text = source_.substr(start_pos_, current_pos_ - start_pos_);

    if (text == "true" || text == "false") {
        return {TokenType::BooleanLiteral, text, {filename_, line_, token_start_col_}};
    }

    TokenType kw_type;
    if (KeywordRegistry::instance().is_keyword(text, kw_type)) {
        return {kw_type, text, {filename_, line_, token_start_col_}};
    }

    return {TokenType::Identifier, text, {filename_, line_, token_start_col_}};
}

} // namespace qkf
