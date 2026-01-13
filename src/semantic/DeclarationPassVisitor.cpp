//
// Created by Sagiv Marzini on 05/12/2025.
//

#include "DeclarationPassVisitor.h"

#include "CompilerException.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "parse/AST/AstNode.h"
#include "parse/AST/Statement.h"

void DeclarationPassVisitor::run(AstNode& root) {
    enterScope(); // Push the global scope
    root.accept(*this);
    exitScope();

    if (!_errors.empty())
        throw CompilerException(_errors);
}

void DeclarationPassVisitor::visit(Program& program) {
    program.scope = _symbolTable.getCurrentScope();
    for (const auto& stmt: program.statements) {
        stmt->accept(*this);
    }
}

void DeclarationPassVisitor::visit(FunctionDeclaration& node) {
    auto symbol = _symbolTable.declare(node.name, Symbol::SymbolKind::Function, node.returnType);
    if (!symbol) {
        logError(SemanticErrorType::DuplicateDeclaration, node, node.name);
    }
    node.symbol = symbol;

    enterScope();
    for (const auto& param: node.parameters) {
        symbol->params.push_back(param);
        if (!_symbolTable.declare(param.name, Symbol::SymbolKind::Parameter, param.type)) {
            logError(SemanticErrorType::ParameterRedeclaration, node, param.name);
        }
    }
    node.body->accept(*this);

    exitScope();
}

void DeclarationPassVisitor::visit(IfStatement& node) {
    enterScope();
    node.thenBranch->accept(*this);
    exitScope();

    if (node.elseBranch) {
        enterScope();
        node.elseBranch->accept(*this);
        exitScope();
    }
}

void DeclarationPassVisitor::visit(WhileLoop& node) {
    enterScope();
    node.body->accept(*this);
    exitScope();
}

void DeclarationPassVisitor::visit(ForLoop& node) {
    enterScope();
    if (!_symbolTable.declare(node.identifier, Symbol::SymbolKind::Variable, Type::Int)) {
        logError(SemanticErrorType::DuplicateDeclaration, node, node.identifier);
    }
    node.body->accept(*this);
    exitScope();
}

void DeclarationPassVisitor::visit(Block& node) {
    enterScope();
    node.scope = _symbolTable.getCurrentScope();
    for (const auto& stmt: node.statements) {
        stmt->accept(*this);
    }
    exitScope();
}

void DeclarationPassVisitor::visit(VariableDeclaration& node) {
    const auto symbolPtr = _symbolTable.declare(node.name, Symbol::SymbolKind::Variable, node.specifiedType,
                                                node.isConst);
    if (!symbolPtr) {
        logError(SemanticErrorType::DuplicateDeclaration, node, node.name);
    }

    node.symbol = symbolPtr;
}

void DeclarationPassVisitor::visit(FunctionCall& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(BinaryExpression& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(UnaryExpression& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(VariableAssignment& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(ReturnStatement& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(IntegerLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(FloatLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(StringLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(BooleanLiteralNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(IdentifierNode& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(RangeExpression& node) {
    // nothing to do in declaration pass
}

void DeclarationPassVisitor::visit(ExpressionStatement& node) {
    // nothing to do in declaration pass
}
