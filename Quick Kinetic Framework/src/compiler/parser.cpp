#include "qkf/parser.hpp"

namespace qkf {

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

bool Parser::has_errors() const {
    for (const auto& d : diagnostics_) {
        if (d.level == DiagnosticLevel::Error) return true;
    }
    return false;
}

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

bool Parser::is_at_end() const {
    return peek().type == TokenType::Eof;
}

const Token& Parser::advance() {
    if (!is_at_end()) current_++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (is_at_end()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& error_message) {
    if (check(type)) return advance();
    add_error(peek(), error_message);
    return peek();
}

void Parser::expect_semicolon(const std::string& context_name) {
    if (!match(TokenType::Semicolon)) {
        add_error(peek(), "Missing strict semicolon ';' after " + context_name + ".");
    }
}

void Parser::add_error(const Token& token, const std::string& message) {
    diagnostics_.push_back({
        DiagnosticLevel::Error,
        token.location,
        message
    });
}

void Parser::add_error(const SourceLocation& loc, const std::string& message) {
    diagnostics_.push_back({
        DiagnosticLevel::Error,
        loc,
        message
    });
}

void Parser::synchronize() {
    advance();
    while (!is_at_end()) {
        if (previous().type == TokenType::Semicolon) return;

        switch (peek().type) {
            case TokenType::KwImport:
            case TokenType::KwPage:
            case TokenType::KwComponent:
            case TokenType::KwState:
            case TokenType::KwAction:
            case TokenType::KwIf:
            case TokenType::KwFor:
            case TokenType::KwReturn:
                return;
            default:
                break;
        }
        advance();
    }
}

std::unique_ptr<ProgramNode> Parser::parse_program() {
    auto program = std::make_unique<ProgramNode>();
    program->location = {tokens_.empty() ? "<unknown>" : tokens_[0].location.filename, 1, 1};

    while (!is_at_end()) {
        try {
            auto decl = parse_declaration();
            if (decl) {
                program->declarations.push_back(std::move(decl));
            }
        } catch (...) {
            synchronize();
        }
    }

    return program;
}

static std::string infer_node_type(const ExprNode* expr) {
    if (!expr) return "void";
    auto lit = dynamic_cast<const LiteralExpr*>(expr);
    if (lit) {
        if (lit->token.type == TokenType::NumberLiteral) {
            if (lit->token.text.find('.') != std::string::npos) return "float";
            return "int";
        }
        if (lit->token.type == TokenType::StringLiteral) return "string";
        if (lit->token.type == TokenType::BooleanLiteral) return "bool";
    }
    auto bin = dynamic_cast<const BinaryExpr*>(expr);
    if (bin) {
        if (bin->op.type == TokenType::EqualEqual || bin->op.type == TokenType::ExclamationEqual ||
            bin->op.type == TokenType::LessThan || bin->op.type == TokenType::LessEqual ||
            bin->op.type == TokenType::GreaterThan || bin->op.type == TokenType::GreaterEqual) {
            return "bool";
        }
        std::string left_t = infer_node_type(bin->left.get());
        std::string right_t = infer_node_type(bin->right.get());
        if (left_t == "string" || right_t == "string") return "string";
        if (left_t == "float" || right_t == "float") return "float";
        if (left_t == "int" && right_t == "int") return "int";
    }
    return "any";
}

std::unique_ptr<DeclNode> Parser::parse_declaration() {
    if (match(TokenType::KwImport)) {
        return parse_import();
    }
    if (match(TokenType::KwTake)) {
        return parse_take();
    }
    if (match(TokenType::KwNamespace)) {
        return parse_namespace();
    }
    if (match(TokenType::KwEnum)) {
        return parse_enum();
    }
    if (match(TokenType::KwTemplate)) {
        auto template_params = parse_template_params();
        if (match(TokenType::KwClass) || match(TokenType::KwStruct)) {
            return parse_class(std::move(template_params));
        }
        if (match(TokenType::KwF)) {
            return parse_function(std::move(template_params));
        }
        add_error(peek(), "Expected 'class', 'struct', or 'f' after template declaration.");
        return nullptr;
    }
    if (match(TokenType::KwClass) || match(TokenType::KwStruct)) {
        return parse_class();
    }
    if (match(TokenType::KwF)) {
        return parse_function();
    }
    if (match(TokenType::KwWindow)) {
        return parse_window();
    }
    if (match(TokenType::KwPage)) {
        return parse_page();
    }
    if (match(TokenType::KwState)) {
        return parse_state();
    }

    // Default error recovery
    add_error(peek(), "Unexpected token '" + peek().text + "' at top-level declaration.");
    advance();
    return nullptr;
}

std::unique_ptr<FunctionDecl> Parser::parse_function(std::vector<TemplateParam> template_params) {
    SourceLocation loc = previous().location;
    const Token& name_token = consume(TokenType::Identifier, "Expected function name after 'f'.");
    std::string func_name = name_token.text;

    consume(TokenType::OpenParen, "Expected '(' after function name.");
    std::vector<FunctionParam> params;
    if (!check(TokenType::CloseParen)) {
        do {
            const Token& p_name = consume(TokenType::Identifier, "Expected parameter name.");
            std::string p_type = "any";
            if (match(TokenType::Colon)) {
                const Token& type_tok = consume(TokenType::Identifier, "Expected parameter type after ':'.");
                p_type = type_tok.text;
            }
            params.push_back({p_name.text, p_type, p_name.location});
        } while (match(TokenType::Comma));
    }
    consume(TokenType::CloseParen, "Expected ')' after parameters.");

    bool is_void = false;
    std::string return_type;

    if (match(TokenType::KwReturn)) {
        consume(TokenType::Equal, "Expected '=' after 'return' (syntax: return = datatype).");
        const Token& type_tok = consume(TokenType::Identifier, "Expected return datatype.");
        return_type = type_tok.text;
    } else if (match(TokenType::KwVoid)) {
        is_void = true;
        return_type = "void";
    } else {
        add_error(peek(), "Expected 'return = <datatype>' or 'void' for function '" + func_name + "'.");
    }

    consume(TokenType::OpenBrace, "Expected '{' to begin function body.");

    auto func = std::make_unique<FunctionDecl>(func_name, std::move(params), is_void, return_type, loc);
    func->template_params = std::move(template_params);

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        if (match(TokenType::KwIf)) {
            func->statements.push_back(parse_if());
            continue;
        }
        if (match(TokenType::KwWhile)) {
            func->statements.push_back(parse_while());
            continue;
        }
        if (match(TokenType::KwDo)) {
            func->statements.push_back(parse_do_while());
            continue;
        }
        if (match(TokenType::KwFor)) {
            func->statements.push_back(parse_for());
            continue;
        }
        if (match(TokenType::KwBreak)) {
            func->statements.push_back(parse_break());
            continue;
        }
        if (match(TokenType::KwContinue)) {
            func->statements.push_back(parse_continue());
            continue;
        }
        if (match(TokenType::KwTry)) {
            func->statements.push_back(parse_try_catch());
            continue;
        }
        if (match(TokenType::KwThrow)) {
            func->statements.push_back(parse_throw());
            continue;
        }

        // Variable declaration inside function: let x = ...;
        if (match(TokenType::KwLet)) {
            SourceLocation let_loc = previous().location;
            std::string v_name = "var";
            if (check(TokenType::Identifier) || check(TokenType::KwWindow)) {
                v_name = advance().text;
            } else {
                add_error(peek(), "Expected variable name after 'let'.");
            }
            std::string v_type = "any";
            if (match(TokenType::Colon)) {
                const Token& t_tok = consume(TokenType::Identifier, "Expected type after ':'.");
                v_type = t_tok.text;
            }
            consume(TokenType::Equal, "Expected '=' in variable declaration.");
            auto init_val = parse_expression();
            expect_semicolon("variable declaration '" + v_name + "'");
            func->statements.push_back(std::make_unique<VarDeclStmt>(v_name, v_type, std::move(init_val), let_loc));
            continue;
        }

        // Expression or Return line
        SourceLocation expr_loc = peek().location;
        auto expr = parse_expression();
        if (!expr) {
            advance();
            continue;
        }

        // If followed by semicolon, it's a regular statement!
        if (match(TokenType::Semicolon)) {
            func->statements.push_back(std::make_unique<ExprStmt>(std::move(expr), expr_loc));
        } else {
            // No semicolon! This is the RETURN expression!
            if (check(TokenType::CloseBrace)) {
                if (func->return_expr != nullptr) {
                    add_error(expr->location, "Multiple return expressions without ';' found in function '" + func_name + "'.");
                }
                func->return_expr = std::move(expr);
            } else {
                add_error(peek(), "Missing strict semicolon ';' after statement in function '" + func_name + "'.");
            }
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' after function body.");
    // Allow optional semicolon after function closing brace
    match(TokenType::Semicolon);

    // Strict validation of return semantics!
    if (is_void) {
        if (func->return_expr != nullptr) {
            add_error(func->return_expr->location,
                      "Function '" + func_name + "' is declared 'void' and cannot return a value (the line without ';' is invalid here).");
        }
    } else {
        if (func->return_expr == nullptr) {
            add_error(loc,
                      "Function '" + func_name + "' declared 'return = " + return_type +
                      "' must have a return expression without ';' before '}'.");
        } else {
            // Strict type checking
            std::string inferred = infer_node_type(func->return_expr.get());
            if (inferred != "any" && inferred != return_type) {
                add_error(func->return_expr->location,
                          "Strict type mismatch in function '" + func_name + "': declared 'return = " +
                          return_type + "', but returns value of type '" + inferred + "'.");
            }
        }
    }

    return func;
}

std::unique_ptr<ImportDecl> Parser::parse_import() {
    SourceLocation loc = previous().location;
    std::string mod_path;

    if (check(TokenType::StringLiteral)) {
        mod_path = advance().text;
    } else if (check(TokenType::Identifier)) {
        const Token& path_token = advance();
        mod_path = path_token.text;
        while (match(TokenType::Dot)) {
            const Token& part = consume(TokenType::Identifier, "Expected identifier after '.' in import.");
            mod_path += "." + part.text;
        }
    } else {
        add_error(peek(), "Expected module path (string or identifier) after 'import'.");
        return nullptr;
    }

    expect_semicolon("import statement");
    return std::make_unique<ImportDecl>(mod_path, loc);
}

std::unique_ptr<TakeDecl> Parser::parse_take() {
    SourceLocation loc = previous().location;
    std::vector<TakeItem> items;

    do {
        std::string sym_name;
        if (check(TokenType::StringLiteral)) {
            sym_name = advance().text;
        } else if (check(TokenType::Identifier)) {
            sym_name = advance().text;
        } else {
            add_error(peek(), "Expected function or symbol name (identifier or string) after 'take'.");
            return nullptr;
        }

        std::string alias = "";
        if (match(TokenType::KwUse)) {
            const Token& a_tok = consume(TokenType::Identifier, "Expected alias name after 'use'.");
            alias = a_tok.text;
        }

        items.push_back({sym_name, alias});
    } while (match(TokenType::Comma));

    consume(TokenType::KwFrom, "Expected 'from' after items in 'take' statement.");

    std::string source_file;
    if (check(TokenType::StringLiteral)) {
        source_file = advance().text;
    } else if (check(TokenType::Identifier)) {
        source_file = advance().text;
    } else {
        add_error(peek(), "Expected source file name (string or identifier) after 'from'.");
        return nullptr;
    }

    // Support trailing 'use alias;' syntax: take "function" from "file" use np;
    if (match(TokenType::KwUse)) {
        const Token& a_tok = consume(TokenType::Identifier, "Expected alias name after 'use'.");
        if (!items.empty()) {
            items[0].alias = a_tok.text;
        }
    }

    expect_semicolon("'take ... from ...' statement");
    return std::make_unique<TakeDecl>(std::move(items), source_file, loc);
}

std::vector<TemplateParam> Parser::parse_template_params() {
    consume(TokenType::LessThan, "Expected '<' after 'template'.");
    std::vector<TemplateParam> params;
    do {
        std::string kind = "typename";
        if (match(TokenType::KwTypename)) {
            kind = "typename";
        } else if (match(TokenType::KwClass)) {
            kind = "class";
        } else {
            add_error(peek(), "Expected 'typename' or 'class' in template parameter.");
        }
        const Token& name_tok = consume(TokenType::Identifier, "Expected template parameter identifier.");
        params.push_back({kind, name_tok.text, name_tok.location});
    } while (match(TokenType::Comma));
    consume(TokenType::GreaterThan, "Expected '>' after template parameters.");
    return params;
}

std::unique_ptr<NamespaceDecl> Parser::parse_namespace() {
    SourceLocation loc = previous().location;
    const Token& name_tok = consume(TokenType::Identifier, "Expected namespace name.");
    consume(TokenType::OpenBrace, "Expected '{' to begin namespace body.");

    auto ns = std::make_unique<NamespaceDecl>(name_tok.text, loc);

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        auto decl = parse_declaration();
        if (decl) {
            ns->declarations.push_back(std::move(decl));
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' to close namespace body.");
    // Optional semicolon after namespace block
    match(TokenType::Semicolon);
    return ns;
}

std::unique_ptr<EnumDecl> Parser::parse_enum() {
    SourceLocation loc = previous().location;
    const Token& name_tok = consume(TokenType::Identifier, "Expected enum name.");
    consume(TokenType::OpenBrace, "Expected '{' to begin enum body.");

    std::vector<EnumItem> items;
    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        const Token& item_tok = consume(TokenType::Identifier, "Expected enum enumerator identifier.");
        std::unique_ptr<ExprNode> val = nullptr;
        if (match(TokenType::Equal)) {
            val = parse_expression();
        }
        items.push_back({item_tok.text, std::move(val), item_tok.location});
        if (!match(TokenType::Comma)) {
            break;
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' to close enum body.");
    expect_semicolon("enum declaration");
    return std::make_unique<EnumDecl>(name_tok.text, std::move(items), loc);
}

std::unique_ptr<ClassDecl> Parser::parse_class(std::vector<TemplateParam> template_params) {
    SourceLocation loc = previous().location;
    const Token& name_tok = consume(TokenType::Identifier, "Expected class name.");
    auto cls = std::make_unique<ClassDecl>(name_tok.text, loc);
    cls->template_params = std::move(template_params);

    // Optional inheritance: class Derived : public Base
    if (match(TokenType::Colon)) {
        if (match(TokenType::KwPublic)) {
            cls->base_access = AccessLevel::Public;
        } else if (match(TokenType::KwPrivate)) {
            cls->base_access = AccessLevel::Private;
        } else if (match(TokenType::KwProtected)) {
            cls->base_access = AccessLevel::Protected;
        }
        const Token& base_tok = consume(TokenType::Identifier, "Expected base class name.");
        cls->base_class = base_tok.text;
    }

    consume(TokenType::OpenBrace, "Expected '{' to begin class body.");

    AccessLevel current_access = AccessLevel::Private; // default in C++ class

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        // Access specifiers: public:, private:, protected:
        if (match(TokenType::KwPublic)) {
            consume(TokenType::Colon, "Expected ':' after 'public'.");
            current_access = AccessLevel::Public;
            continue;
        }
        if (match(TokenType::KwPrivate)) {
            consume(TokenType::Colon, "Expected ':' after 'private'.");
            current_access = AccessLevel::Private;
            continue;
        }
        if (match(TokenType::KwProtected)) {
            consume(TokenType::Colon, "Expected ':' after 'protected'.");
            current_access = AccessLevel::Protected;
            continue;
        }

        // Method: f name(params) ...
        if (match(TokenType::KwF)) {
            auto method = parse_function();
            auto member = std::make_unique<ClassMemberDecl>();
            member->access = current_access;
            member->method = std::move(method);
            cls->members.push_back(std::move(member));
            continue;
        }

        // Member variable: [static] [const] type name [= default];
        bool is_static = match(TokenType::KwStatic);
        bool is_const = match(TokenType::KwConst);

        const Token& type_tok = consume(TokenType::Identifier, "Expected member type or function in class.");
        const Token& member_name_tok = consume(TokenType::Identifier, "Expected member name in class.");

        std::unique_ptr<ExprNode> def_val = nullptr;
        if (match(TokenType::Equal)) {
            def_val = parse_expression();
        }

        expect_semicolon("class member variable '" + member_name_tok.text + "'");

        auto member = std::make_unique<ClassMemberDecl>();
        member->access = current_access;
        member->is_static = is_static;
        member->is_const = is_const;
        member->type_name = type_tok.text;
        member->member_name = member_name_tok.text;
        member->default_val = std::move(def_val);
        cls->members.push_back(std::move(member));
    }

    consume(TokenType::CloseBrace, "Expected '}' to close class body.");
    expect_semicolon("class declaration");
    return cls;
}

std::unique_ptr<StateDecl> Parser::parse_state() {
    SourceLocation loc = previous().location;
    const Token& id_token = consume(TokenType::Identifier, "Expected state variable name.");
    consume(TokenType::Equal, "Expected '=' in state declaration.");
    auto val = parse_expression();
    expect_semicolon("state declaration");
    return std::make_unique<StateDecl>(id_token.text, std::move(val), loc);
}

std::unique_ptr<WindowDecl> Parser::parse_window() {
    SourceLocation loc = previous().location;
    std::string win_name = "MainWindow";
    if (check(TokenType::Identifier)) {
        win_name = advance().text;
    }
    consume(TokenType::OpenBrace, "Expected '{' to begin window body.");

    auto win = std::make_unique<WindowDecl>(win_name, loc);

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        if (match(TokenType::KwF)) {
            win->declarations.push_back(parse_function());
        } else if (match(TokenType::KwState)) {
            win->declarations.push_back(parse_state());
        } else if (check(TokenType::Identifier)) {
            if (current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::Colon) {
                win->properties.push_back(parse_property());
            } else {
                win->root_elements.push_back(parse_ui_element());
            }
        } else {
            add_error(peek(), "Unexpected token in window body: " + peek().text);
            advance();
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' to close window body.");
    expect_semicolon("window declaration");
    return win;
}

std::unique_ptr<PageDecl> Parser::parse_page() {
    SourceLocation loc = previous().location;
    const Token& id_token = consume(TokenType::Identifier, "Expected page name.");
    consume(TokenType::OpenBrace, "Expected '{' to begin page body.");

    auto page = std::make_unique<PageDecl>(id_token.text, loc);

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        if (match(TokenType::KwState)) {
            page->declarations.push_back(parse_state());
        } else if (match(TokenType::KwF)) {
            page->declarations.push_back(parse_function());
        } else if (check(TokenType::Identifier)) {
            // UI Element or Property
            page->root_elements.push_back(parse_ui_element());
        } else {
            add_error(peek(), "Unexpected token in page body: " + peek().text);
            advance();
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' to close page body.");
    expect_semicolon("page declaration");
    return page;
}

std::unique_ptr<UIElementNode> Parser::parse_ui_element() {
    const Token& tag_token = consume(TokenType::Identifier, "Expected component/element name.");
    auto element = std::make_unique<UIElementNode>(tag_token.text, tag_token.location);

    consume(TokenType::OpenBrace, "Expected '{' after component name.");

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        if (check(TokenType::Identifier)) {
            // Lookahead: is it "prop: val;" or nested "Child { ... };"?
            if (current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::Colon) {
                element->properties.push_back(parse_property());
            } else {
                element->children.push_back(parse_ui_element());
            }
        } else {
            add_error(peek(), "Unexpected token in component body: " + peek().text);
            advance();
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' after component body.");
    expect_semicolon("component definition");
    return element;
}

std::unique_ptr<PropertyNode> Parser::parse_property() {
    const Token& key_token = consume(TokenType::Identifier, "Expected property key.");
    consume(TokenType::Colon, "Expected ':' after property key.");

    if (match(TokenType::OpenBrace)) {
        auto block = parse_block();
        expect_semicolon("action block for '" + key_token.text + "'");
        return std::make_unique<PropertyNode>(key_token.text, std::move(block), key_token.location);
    } else {
        auto val = parse_expression();
        expect_semicolon("property assignment '" + key_token.text + "'");
        return std::make_unique<PropertyNode>(key_token.text, std::move(val), key_token.location);
    }
}

std::unique_ptr<StmtNode> Parser::parse_statement() {
    if (match(TokenType::KwIf)) {
        return parse_if();
    }
    if (match(TokenType::KwWhile)) {
        return parse_while();
    }
    if (match(TokenType::KwDo)) {
        return parse_do_while();
    }
    if (match(TokenType::KwFor)) {
        return parse_for();
    }
    if (match(TokenType::KwBreak)) {
        return parse_break();
    }
    if (match(TokenType::KwContinue)) {
        return parse_continue();
    }
    if (match(TokenType::KwTry)) {
        return parse_try_catch();
    }
    if (match(TokenType::KwThrow)) {
        return parse_throw();
    }
    if (match(TokenType::OpenBrace)) {
        return parse_block();
    }
    if (match(TokenType::KwLet)) {
        SourceLocation let_loc = previous().location;
        std::string v_name = "var";
        if (check(TokenType::Identifier) || check(TokenType::KwWindow)) {
            v_name = advance().text;
        } else {
            add_error(peek(), "Expected variable name after 'let'.");
        }
        std::string v_type = "any";
        if (match(TokenType::Colon)) {
            const Token& t_tok = consume(TokenType::Identifier, "Expected type after ':'.");
            v_type = t_tok.text;
        }
        consume(TokenType::Equal, "Expected '=' in variable declaration.");
        auto init_val = parse_expression();
        expect_semicolon("variable declaration '" + v_name + "'");
        return std::make_unique<VarDeclStmt>(v_name, v_type, std::move(init_val), let_loc);
    }

    // Expression statement
    SourceLocation expr_loc = peek().location;
    auto expr = parse_expression();
    expect_semicolon("expression statement");
    return std::make_unique<ExprStmt>(std::move(expr), expr_loc);
}

std::unique_ptr<BlockStmt> Parser::parse_block() {
    auto block = std::make_unique<BlockStmt>();
    block->location = previous().location;

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        block->statements.push_back(parse_statement());
    }

    consume(TokenType::CloseBrace, "Expected '}' to close block.");
    return block;
}

std::unique_ptr<IfStmt> Parser::parse_if() {
    SourceLocation loc = previous().location;
    consume(TokenType::OpenParen, "Expected '(' after 'if'.");
    auto condition = parse_expression();
    consume(TokenType::CloseParen, "Expected ')' after if condition.");

    std::unique_ptr<StmtNode> then_branch;
    if (match(TokenType::OpenBrace)) {
        then_branch = parse_block();
    } else {
        then_branch = parse_statement();
    }

    std::unique_ptr<StmtNode> else_branch = nullptr;
    if (match(TokenType::KwElse)) {
        if (match(TokenType::KwIf)) {
            else_branch = parse_if();
        } else if (match(TokenType::OpenBrace)) {
            else_branch = parse_block();
        } else {
            else_branch = parse_statement();
        }
    }

    return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch), loc);
}

std::unique_ptr<WhileStmt> Parser::parse_while() {
    SourceLocation loc = previous().location;
    consume(TokenType::OpenParen, "Expected '(' after 'while'.");
    auto condition = parse_expression();
    consume(TokenType::CloseParen, "Expected ')' after while condition.");

    std::unique_ptr<StmtNode> body;
    if (match(TokenType::OpenBrace)) {
        body = parse_block();
    } else {
        body = parse_statement();
    }

    return std::make_unique<WhileStmt>(std::move(condition), std::move(body), loc);
}

std::unique_ptr<DoWhileStmt> Parser::parse_do_while() {
    SourceLocation loc = previous().location;
    std::unique_ptr<StmtNode> body;
    if (match(TokenType::OpenBrace)) {
        body = parse_block();
    } else {
        body = parse_statement();
    }

    consume(TokenType::KwWhile, "Expected 'while' after do block.");
    consume(TokenType::OpenParen, "Expected '(' after 'while'.");
    auto condition = parse_expression();
    consume(TokenType::CloseParen, "Expected ')' after do-while condition.");
    expect_semicolon("do-while statement");

    return std::make_unique<DoWhileStmt>(std::move(body), std::move(condition), loc);
}

std::unique_ptr<ForStmt> Parser::parse_for() {
    SourceLocation loc = previous().location;
    consume(TokenType::OpenParen, "Expected '(' after 'for'.");

    auto for_stmt = std::make_unique<ForStmt>();
    for_stmt->location = loc;

    // Check if it's for-in: for (item in collection)
    if (check(TokenType::Identifier) && current_ + 1 < tokens_.size() && tokens_[current_ + 1].type == TokenType::KwIn) {
        for_stmt->is_for_in = true;
        for_stmt->iterator_name = advance().text; // item
        advance(); // consume 'in'
        for_stmt->collection = parse_expression();
        consume(TokenType::CloseParen, "Expected ')' after for-in expression.");
    } else {
        for_stmt->is_for_in = false;
        // Init
        if (match(TokenType::Semicolon)) {
            for_stmt->init = nullptr;
        } else if (match(TokenType::KwLet)) {
            SourceLocation let_loc = previous().location;
            const Token& v_name = consume(TokenType::Identifier, "Expected variable name after 'let'.");
            std::string v_type = "any";
            if (match(TokenType::Colon)) {
                const Token& t_tok = consume(TokenType::Identifier, "Expected type after ':'.");
                v_type = t_tok.text;
            }
            consume(TokenType::Equal, "Expected '=' in variable declaration.");
            auto init_val = parse_expression();
            expect_semicolon("for loop variable initializer");
            for_stmt->init = std::make_unique<VarDeclStmt>(v_name.text, v_type, std::move(init_val), let_loc);
        } else {
            auto init_expr = parse_expression();
            expect_semicolon("for loop initializer expression");
            for_stmt->init = std::make_unique<ExprStmt>(std::move(init_expr), loc);
        }

        // Condition
        if (!check(TokenType::Semicolon)) {
            for_stmt->condition = parse_expression();
        }
        consume(TokenType::Semicolon, "Expected ';' after for loop condition.");

        // Increment
        if (!check(TokenType::CloseParen)) {
            for_stmt->increment = parse_expression();
        }
        consume(TokenType::CloseParen, "Expected ')' after for loop increment.");
    }

    if (match(TokenType::OpenBrace)) {
        for_stmt->body = parse_block();
    } else {
        for_stmt->body = parse_statement();
    }

    return for_stmt;
}

std::unique_ptr<BreakStmt> Parser::parse_break() {
    SourceLocation loc = previous().location;
    expect_semicolon("'break' statement");
    return std::make_unique<BreakStmt>(loc);
}

std::unique_ptr<ContinueStmt> Parser::parse_continue() {
    SourceLocation loc = previous().location;
    expect_semicolon("'continue' statement");
    return std::make_unique<ContinueStmt>(loc);
}

std::unique_ptr<TryCatchStmt> Parser::parse_try_catch() {
    SourceLocation loc = previous().location;
    consume(TokenType::OpenBrace, "Expected '{' to begin try block.");
    auto try_block = parse_block();

    std::vector<CatchClause> catches;
    while (match(TokenType::KwCatch)) {
        SourceLocation catch_loc = previous().location;
        std::string err_var = "e";
        std::string err_type = "any";

        if (match(TokenType::OpenParen)) {
            const Token& v_tok = consume(TokenType::Identifier, "Expected exception variable name.");
            err_var = v_tok.text;
            if (match(TokenType::Colon)) {
                const Token& t_tok = consume(TokenType::Identifier, "Expected exception type.");
                err_type = t_tok.text;
            }
            consume(TokenType::CloseParen, "Expected ')' after catch parameter.");
        }

        consume(TokenType::OpenBrace, "Expected '{' to begin catch block.");
        auto catch_body = parse_block();
        catches.push_back({err_var, err_type, std::move(catch_body), catch_loc});
    }

    std::unique_ptr<BlockStmt> finally_block = nullptr;
    if (match(TokenType::KwFinally)) {
        consume(TokenType::OpenBrace, "Expected '{' to begin finally block.");
        finally_block = parse_block();
    }

    if (catches.empty() && !finally_block) {
        add_error(peek(), "Expected at least one 'catch' or 'finally' block after 'try'.");
    }

    return std::make_unique<TryCatchStmt>(std::move(try_block), std::move(catches), std::move(finally_block), loc);
}

std::unique_ptr<ThrowStmt> Parser::parse_throw() {
    SourceLocation loc = previous().location;
    auto expr = parse_expression();
    expect_semicolon("'throw' statement");
    return std::make_unique<ThrowStmt>(std::move(expr), loc);
}

std::unique_ptr<ArrayLiteralExpr> Parser::parse_array_literal() {
    SourceLocation loc = previous().location;
    std::vector<std::unique_ptr<ExprNode>> elements;

    if (!check(TokenType::CloseBracket)) {
        do {
            elements.push_back(parse_expression());
        } while (match(TokenType::Comma));
    }
    consume(TokenType::CloseBracket, "Expected ']' after array elements.");
    return std::make_unique<ArrayLiteralExpr>(std::move(elements), loc);
}

std::unique_ptr<ExprNode> Parser::parse_expression() {
    auto expr = parse_equality();
    if (match(TokenType::Equal)) {
        Token op = previous();
        auto right = parse_expression();
        return std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parse_equality() {
    auto expr = parse_comparison();

    while (match(TokenType::EqualEqual) || match(TokenType::ExclamationEqual)) {
        Token op = previous();
        auto right = parse_comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parse_comparison() {
    auto expr = parse_term();

    while (match(TokenType::GreaterThan) || match(TokenType::GreaterEqual) ||
           match(TokenType::LessThan) || match(TokenType::LessEqual)) {
        Token op = previous();
        auto right = parse_term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parse_term() {
    auto expr = parse_factor();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        Token op = previous();
        auto right = parse_factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parse_factor() {
    auto expr = parse_postfix();

    while (match(TokenType::Star) || match(TokenType::Slash) || match(TokenType::Percent)) {
        Token op = previous();
        auto right = parse_postfix();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExprNode> Parser::parse_postfix() {
    auto expr = parse_primary();

    while (true) {
        // C++ Scope Resolution: namespace::func or Enum::Value or Class::member
        if (match(TokenType::ColonColon)) {
            const Token& member_tok = consume(TokenType::Identifier, "Expected member or function identifier after '::'.");
            auto id = dynamic_cast<IdentifierExpr*>(expr.get());
            std::string scope_name = id ? id->name : "<unknown>";
            expr = std::make_unique<ScopeResolutionExpr>(scope_name, member_tok.text, member_tok.location);
        }
        // Function Call: callee(arg1, arg2...) e.g. print("msg"), input(), errorlog(), warnlog()
        else if (match(TokenType::OpenParen)) {
            SourceLocation call_loc = previous().location;
            std::vector<std::unique_ptr<ExprNode>> args;
            if (!check(TokenType::CloseParen)) {
                do {
                    args.push_back(parse_expression());
                } while (match(TokenType::Comma));
            }
            consume(TokenType::CloseParen, "Expected ')' after function arguments.");
            expr = std::make_unique<FunctionCallExpr>(std::move(expr), std::move(args), call_loc);
        }
        // Array Subscript: target[index]
        else if (match(TokenType::OpenBracket)) {
            SourceLocation idx_loc = previous().location;
            auto index_expr = parse_expression();
            consume(TokenType::CloseBracket, "Expected ']' after array index.");
            expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index_expr), idx_loc);
        }
        // Member Access / Method Call: target.member or target.method(...)
        else if (match(TokenType::Dot)) {
            const Token& mem = consume(TokenType::Identifier, "Expected member name after '.'.");
            expr = std::make_unique<MemberAccessExpr>(std::move(expr), mem.text, mem.location);
        }
        else {
            break;
        }
    }

    return expr;
}

std::unique_ptr<ExprNode> Parser::parse_primary() {
    if (match(TokenType::OpenBracket)) {
        return parse_array_literal();
    }

    if (match(TokenType::NumberLiteral) || match(TokenType::StringLiteral) || match(TokenType::BooleanLiteral)) {
        return std::make_unique<LiteralExpr>(previous());
    }

    if (match(TokenType::Identifier) || match(TokenType::KwWindow)) {
        return std::make_unique<IdentifierExpr>(previous());
    }

    if (match(TokenType::OpenParen)) {
        auto expr = parse_expression();
        consume(TokenType::CloseParen, "Expected ')' after grouped expression.");
        return expr;
    }

    add_error(peek(), "Expected expression, got: " + peek().text);
    advance();
    return nullptr;
}

} // namespace qkf
