//
// Created by Sagiv Marzini on 05/12/2025.
//

#include "DeclarationPassVisitor.h"

#include "SymbolPool.h"
#include "parse/AST/AstNode.h"
#include "parse/AST/Statement.h"

void DeclarationPassVisitor::run(AstNode &root) {
    pushScope(); // Push the global scope
    root.accept(*this);
    popScope();
}

void DeclarationPassVisitor::visit(Program &program) {
    for (auto &stmt: program.statements) {
        stmt->accept(*this);
    }
}

void DeclarationPassVisitor::visit(FunctionDeclaration &node) {
    auto &symbol = _pool.createSymbol(node.name, Symbol::SymbolKind::Function, node.returnType, _currentScope);
    _currentScope->declare(symbol);
    node.symbol = &symbol;

    pushScope();
    for (auto &param: node.parameters) {
        auto &symbol = _pool.createSymbol(param.name, Symbol::SymbolKind::Parameter, param.type,
                                          _currentScope);
        _currentScope->declare(symbol);
    }
    node.body->accept(*this);

    popScope();
}

void DeclarationPassVisitor::visit(IfStatement &node) {
    pushScope();
    node.thenBranch->accept(*this);
    popScope();

    if (node.elseBranch) {
        pushScope();
        node.elseBranch->accept(*this);
        popScope();
    }
}

void DeclarationPassVisitor::visit(WhileLoop &node) {
    pushScope();
    node.body->accept(*this);
    popScope();
}

void DeclarationPassVisitor::visit(ForLoop &node) {
    pushScope();
    node.body->accept(*this);
    popScope();
}

void DeclarationPassVisitor::visit(Block &node) {
    pushScope();
    for (auto &stmt: node.statements) {
        stmt->accept(*this);
    }
    popScope();
}

void DeclarationPassVisitor::visit(VariableDeclaration &node) {
    auto &symbol = _pool.createSymbol(node.name, Symbol::SymbolKind::Variable, node.type, _currentScope);
    _currentScope->declare(symbol);
    node.symbol = &symbol;
}

void DeclarationPassVisitor::visit(FunctionCall &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(BinaryExpression &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(UnaryExpression &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(VariableAssignment &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(ReturnStatement &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(IntegerLiteralNode &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(FloatLiteralNode &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(StringLiteralNode &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(BooleanLiteralNode &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(IdentifierNode &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(RangeExpression &node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(ExpressionStatement &node) {
    // nothing to do in declaration pass
}
