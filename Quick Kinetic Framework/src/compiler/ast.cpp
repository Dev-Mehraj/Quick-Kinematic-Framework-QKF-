#include "qkf/ast.hpp"

namespace qkf {

void LiteralExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IdentifierExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BinaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ScopeResolutionExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void FunctionCallExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ArrayLiteralExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IndexExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void MemberAccessExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void PropertyNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void UIElementNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ExprStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void VarDeclStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BlockStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IfStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void WhileStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void DoWhileStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ForStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BreakStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ContinueStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void TryCatchStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ThrowStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void FunctionDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ImportDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void TakeDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void NamespaceDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void EnumDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ClassMemberDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ClassDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void StateDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void WindowDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void PageDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ProgramNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }

} // namespace qkf
