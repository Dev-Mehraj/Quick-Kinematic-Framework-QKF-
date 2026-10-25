#pragma once

#include "qkf/ast.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <memory>

namespace qkf {

class NativeAppGenerator : public ASTVisitor {
public:
    NativeAppGenerator();

    std::string generate_cpp(ProgramNode& program);

    void visit(LiteralExpr& node) override;
    void visit(IdentifierExpr& node) override;
    void visit(BinaryExpr& node) override;
    void visit(ScopeResolutionExpr& node) override;
    void visit(FunctionCallExpr& node) override;
    void visit(ArrayLiteralExpr& node) override;
    void visit(IndexExpr& node) override;
    void visit(MemberAccessExpr& node) override;
    void visit(PropertyNode& node) override;
    void visit(UIElementNode& node) override;
    void visit(ExprStmt& node) override;
    void visit(VarDeclStmt& node) override;
    void visit(BlockStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(WhileStmt& node) override;
    void visit(DoWhileStmt& node) override;
    void visit(ForStmt& node) override;
    void visit(BreakStmt& node) override;
    void visit(ContinueStmt& node) override;
    void visit(TryCatchStmt& node) override;
    void visit(ThrowStmt& node) override;
    void visit(FunctionDecl& node) override;
    void visit(ImportDecl& node) override;
    void visit(TakeDecl& node) override;
    void visit(NamespaceDecl& node) override;
    void visit(EnumDecl& node) override;
    void visit(ClassMemberDecl& node) override;
    void visit(ClassDecl& node) override;
    void visit(StateDecl& node) override;
    void visit(WindowDecl& node) override;
    void visit(PageDecl& node) override;
    void visit(ProgramNode& node) override;

    struct MenuAction {
        int id;
        std::string label;
        PropertyNode* onclick;
    };

private:
    void indent();
    std::string build_menu_code(const UIElementNode& toolbar);

    std::ostringstream out_;
    int indent_level_ = 0;
    int menu_id_counter_ = 1000;
    std::vector<MenuAction> registered_menu_actions_;
};

} // namespace qkf
