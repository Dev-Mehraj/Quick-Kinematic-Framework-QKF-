#include "qkf/codegen.hpp"
#include <iostream>
#include <sstream>

namespace qkf {

CodeGenerator::CodeGenerator(Target target) : target_(target) {}

void CodeGenerator::indent() {
    for (int i = 0; i < indent_level_; ++i) {
        out_ << "    ";
    }
}

std::string CodeGenerator::generate(ProgramNode& program) {
    out_.str("");
    out_.clear();
    indent_level_ = 0;

    if (target_ == Target::HtmlWebview) {
        out_ << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
        out_ << "  <meta charset=\"UTF-8\">\n";
        out_ << "  <title>Quick Kinetic App</title>\n";
        out_ << "  <style>\n";
        out_ << "    * { box-sizing: border-box; margin: 0; padding: 0; }\n";
        out_ << "    body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: #0f172a; color: #f8fafc; }\n";
        out_ << "    .qkf-window { border: 1px solid #334155; border-radius: 8px; overflow: hidden; background: #1e293b; margin: 20px; }\n";
        out_ << "    .qkf-titlebar { display: flex; align-items: center; justify-content: space-between; padding: 8px 14px; background: #0f172a; }\n";
        out_ << "    .qkf-window-controls { display: flex; gap: 8px; }\n";
        out_ << "    .qkf-ctrl-btn { width: 12px; height: 12px; border-radius: 50%; border: none; cursor: pointer; }\n";
        out_ << "    .qkf-btn-close { background: #ef4444; }\n";
        out_ << "    .qkf-btn-min { background: #eab308; }\n";
        out_ << "    .qkf-btn-max { background: #22c55e; }\n";
        out_ << "    .qkf-toolbar { display: flex; background: #1e293b; border-bottom: 1px solid #334155; position: relative; }\n";
        out_ << "    .qkf-menu-item { position: relative; padding: 8px 14px; cursor: pointer; user-select: none; font-size: 13px; }\n";
        out_ << "    .qkf-menu-item:hover { background: #334155; }\n";
        out_ << "    .qkf-dropdown { display: none; position: absolute; top: 100%; left: 0; background: #1e293b; border: 1px solid #334155; min-width: 160px; z-index: 100; box-shadow: 0 4px 6px rgba(0,0,0,0.3); }\n";
        out_ << "    .qkf-menu-item:hover > .qkf-dropdown { display: block; }\n";
        out_ << "    .qkf-dropdown .qkf-menu-item { padding: 8px 12px; }\n";
        out_ << "    .qkf-dropdown .qkf-dropdown { left: 100%; top: 0; }\n";
        out_ << "    .qkf-content { padding: 20px; }\n";
        out_ << "  </style>\n</head>\n<body>\n";
    }

    program.accept(*this);

    if (target_ == Target::HtmlWebview) {
        out_ << "</body>\n</html>\n";
    }

    return out_.str();
}

void CodeGenerator::visit(LiteralExpr& node) {
    if (node.token.type == TokenType::StringLiteral) {
        out_ << "\"" << node.token.text << "\"";
    } else {
        out_ << node.token.text;
    }
}

void CodeGenerator::visit(IdentifierExpr& node) {
    out_ << node.name;
}

void CodeGenerator::visit(BinaryExpr& node) {
    out_ << "(";
    if (node.left) node.left->accept(*this);
    out_ << " " << node.op.text << " ";
    if (node.right) node.right->accept(*this);
    out_ << ")";
}

void CodeGenerator::visit(ScopeResolutionExpr& node) {
    out_ << node.scope << "::" << node.member;
}

void CodeGenerator::visit(FunctionCallExpr& node) {
    if (node.callee) node.callee->accept(*this);
    out_ << "(";
    for (size_t i = 0; i < node.args.size(); ++i) {
        if (node.args[i]) node.args[i]->accept(*this);
        if (i + 1 < node.args.size()) out_ << ", ";
    }
    out_ << ")";
}

void CodeGenerator::visit(ArrayLiteralExpr& node) {
    out_ << "[";
    for (size_t i = 0; i < node.elements.size(); ++i) {
        if (node.elements[i]) node.elements[i]->accept(*this);
        if (i + 1 < node.elements.size()) out_ << ", ";
    }
    out_ << "]";
}

void CodeGenerator::visit(IndexExpr& node) {
    if (node.target) node.target->accept(*this);
    out_ << "[";
    if (node.index) node.index->accept(*this);
    out_ << "]";
}

void CodeGenerator::visit(MemberAccessExpr& node) {
    if (node.target) node.target->accept(*this);
    out_ << "." << node.member;
}

void CodeGenerator::visit(PropertyNode& node) {
    indent();
    out_ << node.name << ": ";
    if (node.value) {
        node.value->accept(*this);
    } else if (node.block_value) {
        out_ << "{\n";
        indent_level_++;
        node.block_value->accept(*this);
        indent_level_--;
        indent();
        out_ << "}";
    }
    out_ << ";\n";
}

void CodeGenerator::visit(UIElementNode& node) {
    indent();
    std::string tag = node.component_name;
    std::string id_attr = "";
    std::string name_attr = "";
    std::string label_attr = "";

    for (const auto& p : node.properties) {
        if (p->name == "id" && p->value) {
            auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
            if (lit) id_attr = " id=\"" + lit->token.text + "\"";
        } else if (p->name == "name" && p->value) {
            auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
            if (lit) name_attr = " data-name=\"" + lit->token.text + "\"";
        } else if (p->name == "label" && p->value) {
            auto lit = dynamic_cast<LiteralExpr*>(p->value.get());
            if (lit) label_attr = lit->token.text;
        }
    }

    if (tag == "Toolbar") {
        out_ << "<nav class=\"qkf-toolbar\"" << id_attr << name_attr << ">\n";
    } else if (tag == "TitleBar") {
        out_ << "<div class=\"qkf-titlebar\"" << id_attr << name_attr << ">\n";
    } else if (tag == "WindowControls") {
        out_ << "<div class=\"qkf-window-controls\"" << id_attr << name_attr << ">\n";
        out_ << "  <button class=\"qkf-ctrl-btn qkf-btn-close\"></button>\n";
        out_ << "  <button class=\"qkf-ctrl-btn qkf-btn-min\"></button>\n";
        out_ << "  <button class=\"qkf-ctrl-btn qkf-btn-max\"></button>\n";
        out_ << "</div>\n";
        return;
    } else {
        bool has_sub = !node.children.empty();
        out_ << "<div class=\"qkf-menu-item" << (has_sub ? " qkf-has-children" : "") << "\"" << id_attr << name_attr << ">";
        out_ << (label_attr.empty() ? tag : label_attr);
        if (has_sub) {
            out_ << "\n<div class=\"qkf-dropdown\">\n";
        }
    }

    indent_level_++;
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    indent_level_--;

    if (tag == "Toolbar") {
        indent();
        out_ << "</nav>\n";
    } else if (tag == "TitleBar") {
        indent();
        out_ << "</div>\n";
    } else {
        if (!node.children.empty()) {
            out_ << "</div>\n";
        }
        out_ << "</div>\n";
    }
}

void CodeGenerator::visit(ExprStmt& node) {
    indent();
    if (node.expr) node.expr->accept(*this);
    out_ << ";\n";
}

void CodeGenerator::visit(VarDeclStmt& node) {
    indent();
    out_ << "let " << node.name << " = ";
    if (node.init) node.init->accept(*this);
    out_ << ";\n";
}

void CodeGenerator::visit(BlockStmt& node) {
    for (const auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void CodeGenerator::visit(IfStmt& node) {
    indent();
    out_ << "if (";
    if (node.condition) node.condition->accept(*this);
    out_ << ") {\n";
    indent_level_++;
    if (node.then_branch) node.then_branch->accept(*this);
    indent_level_--;
    indent();
    out_ << "}";
    if (node.else_branch) {
        out_ << " else {\n";
        indent_level_++;
        node.else_branch->accept(*this);
        indent_level_--;
        indent();
        out_ << "}";
    }
    out_ << "\n";
}

void CodeGenerator::visit(WhileStmt& node) {
    indent();
    out_ << "while (";
    if (node.condition) node.condition->accept(*this);
    out_ << ") {\n";
    indent_level_++;
    if (node.body) node.body->accept(*this);
    indent_level_--;
    indent();
    out_ << "}\n";
}

void CodeGenerator::visit(DoWhileStmt& node) {
    indent();
    out_ << "do {\n";
    indent_level_++;
    if (node.body) node.body->accept(*this);
    indent_level_--;
    indent();
    out_ << "} while (";
    if (node.condition) node.condition->accept(*this);
    out_ << ");\n";
}

void CodeGenerator::visit(ForStmt& node) {
    indent();
    out_ << "for (";
    if (node.is_for_in) {
        out_ << node.iterator_name << " in ";
        if (node.collection) node.collection->accept(*this);
    } else {
        if (node.init) node.init->accept(*this);
        out_ << "; ";
        if (node.condition) node.condition->accept(*this);
        out_ << "; ";
        if (node.increment) node.increment->accept(*this);
    }
    out_ << ") {\n";
    indent_level_++;
    if (node.body) node.body->accept(*this);
    indent_level_--;
    indent();
    out_ << "}\n";
}

void CodeGenerator::visit(BreakStmt&) {
    indent();
    out_ << "break;\n";
}

void CodeGenerator::visit(ContinueStmt&) {
    indent();
    out_ << "continue;\n";
}

void CodeGenerator::visit(TryCatchStmt& node) {
    indent();
    out_ << "try {\n";
    indent_level_++;
    if (node.try_block) node.try_block->accept(*this);
    indent_level_--;
    indent();
    out_ << "}";
    for (const auto& c : node.catch_clauses) {
        out_ << " catch (" << c.error_var << ") {\n";
        indent_level_++;
        if (c.body) c.body->accept(*this);
        indent_level_--;
        indent();
        out_ << "}";
    }
    if (node.finally_block) {
        out_ << " finally {\n";
        indent_level_++;
        node.finally_block->accept(*this);
        indent_level_--;
        indent();
        out_ << "}";
    }
    out_ << "\n";
}

void CodeGenerator::visit(ThrowStmt& node) {
    indent();
    out_ << "throw ";
    if (node.expr) node.expr->accept(*this);
    out_ << ";\n";
}

void CodeGenerator::visit(FunctionDecl& node) {
    indent();
    out_ << "f " << node.name << "(";
    for (size_t i = 0; i < node.params.size(); ++i) {
        out_ << node.params[i].name << ": " << node.params[i].type_name;
        if (i + 1 < node.params.size()) out_ << ", ";
    }
    out_ << ") ";
    if (node.is_void) {
        out_ << "void ";
    } else {
        out_ << "return = " << node.return_type << " ";
    }
    out_ << "{\n";
    indent_level_++;
    for (const auto& s : node.statements) {
        s->accept(*this);
    }
    if (node.return_expr) {
        indent();
        out_ << "return ";
        node.return_expr->accept(*this);
        out_ << ";\n";
    }
    indent_level_--;
    indent();
    out_ << "}\n\n";
}

void CodeGenerator::visit(ImportDecl& node) {
    indent();
    out_ << "import \"" << node.module_path << "\";\n";
}

void CodeGenerator::visit(TakeDecl& node) {
    indent();
    out_ << "take from \"" << node.source_file << "\";\n";
}

void CodeGenerator::visit(NamespaceDecl& node) {
    indent();
    out_ << "namespace " << node.name << " {\n";
    indent_level_++;
    for (const auto& d : node.declarations) if (d) d->accept(*this);
    indent_level_--;
    indent();
    out_ << "}\n\n";
}

void CodeGenerator::visit(EnumDecl& node) {
    indent();
    out_ << "enum " << node.name << " {\n";
    indent_level_++;
    for (size_t i = 0; i < node.items.size(); ++i) {
        indent();
        out_ << node.items[i].name;
        if (node.items[i].value) {
            out_ << " = ";
            node.items[i].value->accept(*this);
        }
        if (i + 1 < node.items.size()) out_ << ",";
        out_ << "\n";
    }
    indent_level_--;
    indent();
    out_ << "};\n\n";
}

void CodeGenerator::visit(ClassMemberDecl& node) {
    indent();
    if (node.method) node.method->accept(*this);
    else {
        out_ << node.type_name << " " << node.member_name << ";\n";
    }
}

void CodeGenerator::visit(ClassDecl& node) {
    indent();
    out_ << "class " << node.name;
    if (!node.base_class.empty()) {
        out_ << " : public " << node.base_class;
    }
    out_ << " {\n";
    indent_level_++;
    for (const auto& m : node.members) if (m) m->accept(*this);
    indent_level_--;
    indent();
    out_ << "};\n\n";
}

void CodeGenerator::visit(StateDecl& node) {
    indent();
    out_ << "state " << node.name << " = ";
    if (node.initial_value) node.initial_value->accept(*this);
    out_ << ";\n";
}

void CodeGenerator::visit(WindowDecl& node) {
    if (target_ == Target::HtmlWebview) {
        out_ << "<div class=\"qkf-window\">\n";
        indent_level_++;
        for (const auto& el : node.root_elements) {
            el->accept(*this);
        }
        indent_level_--;
        out_ << "</div>\n";
    }
}

void CodeGenerator::visit(PageDecl& node) {
    for (const auto& el : node.root_elements) {
        el->accept(*this);
    }
}

void CodeGenerator::visit(ProgramNode& node) {
    for (const auto& d : node.declarations) {
        if (d) d->accept(*this);
    }
}

} // namespace qkf
