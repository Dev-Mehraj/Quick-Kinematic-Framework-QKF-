#pragma once

#include "qkf/token.hpp"
#include <vector>
#include <memory>
#include <string>

namespace qkf {

class ASTVisitor;

struct ASTNode {
    SourceLocation location;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// Expressions
struct ExprNode : public ASTNode {};

struct LiteralExpr : public ExprNode {
    Token token;
    explicit LiteralExpr(Token t) : token(std::move(t)) { location = token.location; }
    void accept(ASTVisitor& visitor) override;
};

struct IdentifierExpr : public ExprNode {
    std::string name;
    explicit IdentifierExpr(Token t) : name(t.text) { location = t.location; }
    void accept(ASTVisitor& visitor) override;
};

struct BinaryExpr : public ExprNode {
    std::unique_ptr<ExprNode> left;
    Token op;
    std::unique_ptr<ExprNode> right;

    BinaryExpr(std::unique_ptr<ExprNode> l, Token o, std::unique_ptr<ExprNode> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {
        location = left ? left->location : op.location;
    }
    void accept(ASTVisitor& visitor) override;
};

// C++ Scope Resolution: namespace::func or Enum::Value or Class::member
struct ScopeResolutionExpr : public ExprNode {
    std::string scope;
    std::string member;

    ScopeResolutionExpr(std::string s, std::string m, SourceLocation loc)
        : scope(std::move(s)), member(std::move(m)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// Function Call: callee(args...)
struct FunctionCallExpr : public ExprNode {
    std::unique_ptr<ExprNode> callee;
    std::vector<std::unique_ptr<ExprNode>> args;

    FunctionCallExpr(std::unique_ptr<ExprNode> c, std::vector<std::unique_ptr<ExprNode>> a, SourceLocation loc)
        : callee(std::move(c)), args(std::move(a)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// Array Literal: [1, 2, 3]
struct ArrayLiteralExpr : public ExprNode {
    std::vector<std::unique_ptr<ExprNode>> elements;

    ArrayLiteralExpr(std::vector<std::unique_ptr<ExprNode>> elems, SourceLocation loc)
        : elements(std::move(elems)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// Array Subscript / Index: target[index]
struct IndexExpr : public ExprNode {
    std::unique_ptr<ExprNode> target;
    std::unique_ptr<ExprNode> index;

    IndexExpr(std::unique_ptr<ExprNode> t, std::unique_ptr<ExprNode> idx, SourceLocation loc)
        : target(std::move(t)), index(std::move(idx)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// Member Access: target.member or target.method(...)
struct MemberAccessExpr : public ExprNode {
    std::unique_ptr<ExprNode> target;
    std::string member;

    MemberAccessExpr(std::unique_ptr<ExprNode> t, std::string m, SourceLocation loc)
        : target(std::move(t)), member(std::move(m)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct BlockStmt;

// Property assignment: key: value; or key: { ... };
struct PropertyNode : public ASTNode {
    std::string name;
    std::unique_ptr<ExprNode> value;
    std::unique_ptr<BlockStmt> block_value;

    PropertyNode(std::string n, std::unique_ptr<ExprNode> v, SourceLocation loc)
        : name(std::move(n)), value(std::move(v)), block_value(nullptr) {
        location = loc;
    }

    PropertyNode(std::string n, std::unique_ptr<BlockStmt> b, SourceLocation loc)
        : name(std::move(n)), value(nullptr), block_value(std::move(b)) {
        location = loc;
    }

    void accept(ASTVisitor& visitor) override;
};

// UI Element Node: ComponentName { prop: val; Child { ... }; }
struct UIElementNode : public ASTNode {
    std::string component_name;
    std::vector<std::unique_ptr<PropertyNode>> properties;
    std::vector<std::unique_ptr<UIElementNode>> children;

    UIElementNode(std::string name, SourceLocation loc)
        : component_name(std::move(name)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// Statements inside functions
struct StmtNode : public ASTNode {};

struct ExprStmt : public StmtNode {
    std::unique_ptr<ExprNode> expr;
    explicit ExprStmt(std::unique_ptr<ExprNode> e, SourceLocation loc)
        : expr(std::move(e)) { location = loc; }
    void accept(ASTVisitor& visitor) override;
};

struct VarDeclStmt : public StmtNode {
    std::string name;
    std::string type_name;
    std::unique_ptr<ExprNode> init;
    VarDeclStmt(std::string n, std::string t, std::unique_ptr<ExprNode> i, SourceLocation loc)
        : name(std::move(n)), type_name(std::move(t)), init(std::move(i)) { location = loc; }
    void accept(ASTVisitor& visitor) override;
};

// Block statement: { stmt1; stmt2; }
struct BlockStmt : public StmtNode {
    std::vector<std::unique_ptr<StmtNode>> statements;
    std::unique_ptr<ExprNode> tail_expr; // optional tail expression without ';'
    void accept(ASTVisitor& visitor) override;
};

// Conditional: if (cond) then_stmt else else_stmt
struct IfStmt : public StmtNode {
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> then_branch;
    std::unique_ptr<StmtNode> else_branch; // optional

    IfStmt(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> then_b, std::unique_ptr<StmtNode> else_b, SourceLocation loc)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// Loops
struct WhileStmt : public StmtNode {
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<StmtNode> body;

    WhileStmt(std::unique_ptr<ExprNode> cond, std::unique_ptr<StmtNode> b, SourceLocation loc)
        : condition(std::move(cond)), body(std::move(b)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct DoWhileStmt : public StmtNode {
    std::unique_ptr<StmtNode> body;
    std::unique_ptr<ExprNode> condition;

    DoWhileStmt(std::unique_ptr<StmtNode> b, std::unique_ptr<ExprNode> cond, SourceLocation loc)
        : body(std::move(b)), condition(std::move(cond)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct ForStmt : public StmtNode {
    bool is_for_in = false;
    // C-style for: for (init; cond; inc) body
    std::unique_ptr<StmtNode> init;
    std::unique_ptr<ExprNode> condition;
    std::unique_ptr<ExprNode> increment;

    // Range / For-in: for (item in collection) body
    std::string iterator_name;
    std::unique_ptr<ExprNode> collection;

    std::unique_ptr<StmtNode> body;

    void accept(ASTVisitor& visitor) override;
};

struct BreakStmt : public StmtNode {
    explicit BreakStmt(SourceLocation loc) { location = loc; }
    void accept(ASTVisitor& visitor) override;
};

struct ContinueStmt : public StmtNode {
    explicit ContinueStmt(SourceLocation loc) { location = loc; }
    void accept(ASTVisitor& visitor) override;
};

// Try / Catch / Finally / Throw
struct CatchClause {
    std::string error_var;
    std::string error_type;
    std::unique_ptr<BlockStmt> body;
    SourceLocation location;
};

struct TryCatchStmt : public StmtNode {
    std::unique_ptr<BlockStmt> try_block;
    std::vector<CatchClause> catch_clauses;
    std::unique_ptr<BlockStmt> finally_block; // optional

    TryCatchStmt(std::unique_ptr<BlockStmt> t, std::vector<CatchClause> c, std::unique_ptr<BlockStmt> f, SourceLocation loc)
        : try_block(std::move(t)), catch_clauses(std::move(c)), finally_block(std::move(f)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct ThrowStmt : public StmtNode {
    std::unique_ptr<ExprNode> expr;
    ThrowStmt(std::unique_ptr<ExprNode> e, SourceLocation loc)
        : expr(std::move(e)) { location = loc; }
    void accept(ASTVisitor& visitor) override;
};

// Function parameter: name: datatype or just name
struct FunctionParam {
    std::string name;
    std::string type_name;
    SourceLocation location;
};

// Top-level declarations
struct DeclNode : public ASTNode {};

// C++ Style Template Parameter
struct TemplateParam {
    std::string kind; // "typename" or "class"
    std::string name; // "T"
    SourceLocation location;
};

// Function declaration:
// f name(params) return = datatype { ... }
// or f name(params) void { ... }
struct FunctionDecl : public DeclNode {
    std::string name;
    std::vector<FunctionParam> params;
    std::vector<TemplateParam> template_params;
    bool is_void = false;
    std::string return_type; // e.g. "int", "string", "void"
    std::vector<std::unique_ptr<StmtNode>> statements; // Each required strict ';'
    std::unique_ptr<ExprNode> return_expr; // The tail expression WITHOUT ';'

    FunctionDecl(std::string n, std::vector<FunctionParam> p, bool v, std::string ret, SourceLocation loc)
        : name(std::move(n)), params(std::move(p)), is_void(v), return_type(std::move(ret)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct ImportDecl : public DeclNode {
    std::string module_path;
    ImportDecl(std::string path, SourceLocation loc) : module_path(std::move(path)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// take "function" from "file" use np;
// take func1, func2 from "file";
struct TakeItem {
    std::string symbol_name;
    std::string alias; // empty if no 'use'
};

struct TakeDecl : public DeclNode {
    std::vector<TakeItem> items;
    std::string source_file;

    TakeDecl(std::vector<TakeItem> it, std::string src, SourceLocation loc)
        : items(std::move(it)), source_file(std::move(src)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// C++ Style Namespace
struct NamespaceDecl : public DeclNode {
    std::string name;
    std::vector<std::unique_ptr<DeclNode>> declarations;

    NamespaceDecl(std::string n, SourceLocation loc) : name(std::move(n)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// C++ Style Enum
struct EnumItem {
    std::string name;
    std::unique_ptr<ExprNode> value; // optional '= 0'
    SourceLocation location;
};

struct EnumDecl : public DeclNode {
    std::string name;
    std::vector<EnumItem> items;

    EnumDecl(std::string n, std::vector<EnumItem> it, SourceLocation loc)
        : name(std::move(n)), items(std::move(it)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// C++ Style Class & Access Specifiers
enum class AccessLevel {
    Public,
    Private,
    Protected
};

struct ClassMemberDecl : public ASTNode {
    AccessLevel access = AccessLevel::Private;
    bool is_static = false;
    bool is_const = false;
    std::string type_name;
    std::string member_name;
    std::unique_ptr<ExprNode> default_val;
    std::unique_ptr<FunctionDecl> method;

    void accept(ASTVisitor& visitor) override;
};

struct ClassDecl : public DeclNode {
    std::string name;
    std::vector<TemplateParam> template_params;
    std::string base_class;
    AccessLevel base_access = AccessLevel::Public;
    std::vector<std::unique_ptr<ClassMemberDecl>> members;

    ClassDecl(std::string n, SourceLocation loc) : name(std::move(n)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct StateDecl : public DeclNode {
    std::string name;
    std::unique_ptr<ExprNode> initial_value;
    StateDecl(std::string n, std::unique_ptr<ExprNode> val, SourceLocation loc)
        : name(std::move(n)), initial_value(std::move(val)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

// UI Window declaration:
// window MainWindow { ... };
struct WindowDecl : public DeclNode {
    std::string name;
    std::vector<std::unique_ptr<PropertyNode>> properties;
    std::vector<std::unique_ptr<DeclNode>> declarations;
    std::vector<std::unique_ptr<UIElementNode>> root_elements;

    WindowDecl(std::string n, SourceLocation loc) : name(std::move(n)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct PageDecl : public DeclNode {
    std::string name;
    std::vector<std::unique_ptr<DeclNode>> declarations;
    std::vector<std::unique_ptr<UIElementNode>> root_elements;

    PageDecl(std::string n, SourceLocation loc) : name(std::move(n)) {
        location = loc;
    }
    void accept(ASTVisitor& visitor) override;
};

struct ProgramNode : public ASTNode {
    std::vector<std::unique_ptr<DeclNode>> declarations;
    void accept(ASTVisitor& visitor) override;
};

// Visitor Interface
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(LiteralExpr& node) = 0;
    virtual void visit(IdentifierExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(ScopeResolutionExpr& node) = 0;
    virtual void visit(FunctionCallExpr& node) = 0;
    virtual void visit(ArrayLiteralExpr& node) = 0;
    virtual void visit(IndexExpr& node) = 0;
    virtual void visit(MemberAccessExpr& node) = 0;
    virtual void visit(PropertyNode& node) = 0;
    virtual void visit(UIElementNode& node) = 0;
    virtual void visit(ExprStmt& node) = 0;
    virtual void visit(VarDeclStmt& node) = 0;
    virtual void visit(BlockStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(DoWhileStmt& node) = 0;
    virtual void visit(ForStmt& node) = 0;
    virtual void visit(BreakStmt& node) = 0;
    virtual void visit(ContinueStmt& node) = 0;
    virtual void visit(TryCatchStmt& node) = 0;
    virtual void visit(ThrowStmt& node) = 0;
    virtual void visit(FunctionDecl& node) = 0;
    virtual void visit(ImportDecl& node) = 0;
    virtual void visit(TakeDecl& node) = 0;
    virtual void visit(NamespaceDecl& node) = 0;
    virtual void visit(EnumDecl& node) = 0;
    virtual void visit(ClassMemberDecl& node) = 0;
    virtual void visit(ClassDecl& node) = 0;
    virtual void visit(StateDecl& node) = 0;
    virtual void visit(WindowDecl& node) = 0;
    virtual void visit(PageDecl& node) = 0;
    virtual void visit(ProgramNode& node) = 0;
};

} // namespace qkf
