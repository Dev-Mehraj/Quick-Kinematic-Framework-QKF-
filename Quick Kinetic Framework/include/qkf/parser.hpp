#pragma once

#include "qkf/token.hpp"
#include "qkf/ast.hpp"
#include <vector>
#include <memory>

namespace qkf {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<ProgramNode> parse_program();
    const std::vector<Diagnostic>& diagnostics() const { return diagnostics_; }
    bool has_errors() const;

private:
    const Token& peek() const;
    const Token& previous() const;
    bool is_at_end() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& error_message);

    // Parsing routines
    std::unique_ptr<DeclNode> parse_declaration();
    std::unique_ptr<FunctionDecl> parse_function(std::vector<TemplateParam> template_params = {});
    std::unique_ptr<NamespaceDecl> parse_namespace();
    std::unique_ptr<EnumDecl> parse_enum();
    std::unique_ptr<ClassDecl> parse_class(std::vector<TemplateParam> template_params = {});
    std::vector<TemplateParam> parse_template_params();
    std::unique_ptr<ImportDecl> parse_import();
    std::unique_ptr<TakeDecl> parse_take();
    std::unique_ptr<StateDecl> parse_state();
    std::unique_ptr<WindowDecl> parse_window();
    std::unique_ptr<PageDecl> parse_page();
    std::unique_ptr<UIElementNode> parse_ui_element();
    std::unique_ptr<PropertyNode> parse_property();

    // Statements
    std::unique_ptr<StmtNode> parse_statement();
    std::unique_ptr<BlockStmt> parse_block();
    std::unique_ptr<IfStmt> parse_if();
    std::unique_ptr<WhileStmt> parse_while();
    std::unique_ptr<DoWhileStmt> parse_do_while();
    std::unique_ptr<ForStmt> parse_for();
    std::unique_ptr<BreakStmt> parse_break();
    std::unique_ptr<ContinueStmt> parse_continue();
    std::unique_ptr<TryCatchStmt> parse_try_catch();
    std::unique_ptr<ThrowStmt> parse_throw();

    // Expressions
    std::unique_ptr<ExprNode> parse_expression();
    std::unique_ptr<ArrayLiteralExpr> parse_array_literal();
    std::unique_ptr<ExprNode> parse_equality();
    std::unique_ptr<ExprNode> parse_comparison();
    std::unique_ptr<ExprNode> parse_term();
    std::unique_ptr<ExprNode> parse_factor();
    std::unique_ptr<ExprNode> parse_postfix();
    std::unique_ptr<ExprNode> parse_primary();

    // Semicolon enforcement helper
    void expect_semicolon(const std::string& context_name);

    void synchronize();
    void add_error(const Token& token, const std::string& message);
    void add_error(const SourceLocation& loc, const std::string& message);

    std::vector<Token> tokens_;
    size_t current_ = 0;
    std::vector<Diagnostic> diagnostics_;
};

} // namespace qkf
