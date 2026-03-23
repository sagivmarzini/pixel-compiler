#include "DeclarationPassVisitor.h"

#include "CompilerException.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "parse/AST/AstNode.h"

void DeclarationPassVisitor::run(AST::AstNode& root) {
    enterScope(); // Push the global scope
    root.accept(*this);
    exitScope();

    if (!_errors.empty())
        throw CompilerException(_errors);
}

void DeclarationPassVisitor::visit(AST::Program& program) {
    program.scope = _symbolTable.getCurrentScope();
    for (const auto& stmt: program.statements) {
        stmt->accept(*this);
    }
}

void DeclarationPassVisitor::visit(AST::FunctionDeclaration& node) {
    if (node.name == "main") node.name = "__main";
    const auto symbol = _symbolTable.declare(node.name, Symbol::SymbolKind::Function, node.returnType);
    if (!symbol) {
        logError(SemanticErrorType::DuplicateDeclaration, node, node.name);
        return;
    }
    node.symbol = symbol;

    enterScope();
    for (auto& param: node.parameters) {
        symbol->params.push_back(param);

        Symbol* paramSymbol = _symbolTable.declare(param.name, Symbol::SymbolKind::Parameter, param.type);

        if (!paramSymbol) {
            logError(SemanticErrorType::ParameterRedeclaration, node, param.name);
        } else {
            param.symbol = paramSymbol;
        }
    }

    node.body->accept(*this);
    exitScope();
}

void DeclarationPassVisitor::visit(AST::IfStatement& node) {
    enterScope();
    node.thenBranch->accept(*this);
    exitScope();

    if (node.elseBranch) {
        enterScope();
        node.elseBranch->accept(*this);
        exitScope();
    }
}

void DeclarationPassVisitor::visit(AST::WhileLoop& node) {
    enterScope();
    node.body->accept(*this);
    exitScope();
}

void DeclarationPassVisitor::visit(AST::ForLoop& node) {
    enterScope();

    const auto symbol = _symbolTable.declare(node.identifier, Symbol::SymbolKind::Variable, ScalarKind::Int);
    if (!symbol) {
        logError(SemanticErrorType::DuplicateDeclaration, node, node.identifier);
    }
    node.symbol = symbol;
    node.body->accept(*this);
    exitScope();
}

void DeclarationPassVisitor::visit(AST::Block& node) {
    enterScope();
    node.scope = _symbolTable.getCurrentScope();
    for (const auto& stmt: node.statements) {
        stmt->accept(*this);
    }
    exitScope();
}

void DeclarationPassVisitor::visit(AST::VariableDeclaration& node) {
    const auto symbolPtr = _symbolTable.declare(node.name, Symbol::SymbolKind::Variable, node.specifiedType,
                                                node.isConst);
    if (!symbolPtr) {
        logError(SemanticErrorType::DuplicateDeclaration, node, node.name);
    }

    node.symbol = symbolPtr;
}

void DeclarationPassVisitor::visit(AST::FunctionCall& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::BinaryExpression& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::UnaryExpression& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::IncDecExpression& node) {
    // nothing to do in decleration pass
}

void DeclarationPassVisitor::visit(AST::VariableAssignment& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::ReturnStatement& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::IntegerLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::FloatLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::StringLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::BooleanLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::VariableExpression& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::RangeExpression& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::ExpressionStatement& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::ArrayAssignment& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::ArrayLiteral& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(AST::ArrayIndex& node) {
    // nothing to do in declaration pass
}
