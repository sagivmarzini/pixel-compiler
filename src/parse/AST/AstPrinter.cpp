#include "AstPrinter.h"

#include <iostream>

#include "Statement.h"
#include "semantic/Symbol.h"

void AstPrinter::print(AstNode& root) {
    _indent = 0;
    root.accept(*this);
}

void AstPrinter::printIndent() const {
    for (int i = 0; i < _indent; i++) {
        std::cout << "  ";
    }
}

void AstPrinter::printSymbol(const Symbol& symbol) {
    printIndent();
    std::cout << "Symbol:\n";
    _indent++;

    printIndent();
    std::cout << "Name: " << symbol.name << "\n";

    printIndent();
    std::cout << "Kind: " << symbolKindToString(symbol.kind) << "\n";

    printIndent();
    std::cout << "Type: " << symbol.type << "\n";

    printIndent();
    std::cout << "Scope: " << symbol.scope << "\n";

    if (!symbol.params.empty()) {
        printIndent();
        std::cout << "Params: [";
        for (size_t i = 0; i < symbol.params.size(); i++) {
            std::cout << symbol.params[i].name << ": " << symbol.params[i].type;
            if (i < symbol.params.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    _indent--;
}

std::string AstPrinter::symbolKindToString(Symbol::SymbolKind kind) {
    switch (kind) {
        case Symbol::SymbolKind::Variable: return "Variable";
        case Symbol::SymbolKind::Function: return "Function";
        case Symbol::SymbolKind::Parameter: return "Parameter";
        case Symbol::SymbolKind::TypeDefinition: return "TypeDefinition";
        case Symbol::SymbolKind::Constant: return "Constant";
        default: return "Unknown";
    }
}

void AstPrinter::visit(Program& program) {
    printIndent();
    std::cout << "Program\n";
    _indent++;
    for (auto& node: program.statements) {
        node->accept(*this);
    }
    _indent--;
}

void AstPrinter::visit(IntegerLiteralNode& node) {
    printIndent();
    std::cout << "IntegerLiteral: " << node.value << "\n";
}

void AstPrinter::visit(FloatLiteralNode& node) {
    printIndent();
    std::cout << "FloatLiteral: " << node.value << "\n";
}

void AstPrinter::visit(StringLiteralNode& node) {
    printIndent();
    std::cout << "StringLiteral: \"" << node.value << "\"\n";
}

void AstPrinter::visit(BooleanLiteralNode& node) {
    printIndent();
    std::cout << "BooleanLiteral: " << (node.value ? "true" : "false") << "\n";
}

void AstPrinter::visit(BinaryExpression& node) {
    printIndent();
    std::cout << "BinaryExpression: " << node.op << "\n";
    _indent++;
    node.left->accept(*this);
    node.right->accept(*this);
    _indent--;
}

void AstPrinter::visit(UnaryExpression& node) {
    printIndent();
    std::cout << "UnaryExpression: " << node.op << "\n";
    _indent++;
    node.operand->accept(*this);
    _indent--;
}

void AstPrinter::visit(IdentifierNode& node) {
    printIndent();
    std::cout << "Identifier: " << node.name << "\n";
}

void AstPrinter::visit(FunctionCall& node) {
    printIndent();
    std::cout << "FunctionCall: " << node.functionName << "\n";
    _indent++;
    for (auto& arg: node.arguments) {
        printIndent();
        std::cout << "Argument: " << arg.name << "\n";
        _indent++;
        arg.value->accept(*this);
        _indent--;
    }
    _indent--;
}

void AstPrinter::visit(VariableDeclaration& node) {
    printIndent();
    std::cout << "VariableDeclaration: " << node.name << " : " << node.specifiedType << "\n";

    if (node.symbol) {
        _indent++;
        printSymbol(*node.symbol);
        _indent--;
    }

    if (node.value) {
        _indent++;
        printIndent();
        std::cout << "InitialValue:\n";
        _indent++;
        node.value->accept(*this);
        _indent--;
        _indent--;
    }
}

void AstPrinter::visit(VariableAssignment& node) {
    printIndent();
    std::cout << "VariableAssignment: " << node.varName << "\n";
    _indent++;
    node.assignedValue->accept(*this);
    _indent--;
}

void AstPrinter::visit(ReturnStatement& node) {
    printIndent();
    std::cout << "ReturnStatement\n";
    _indent++;
    node.value->accept(*this);
    _indent--;
}

void AstPrinter::visit(ExpressionStatement& node) {
    printIndent();
    std::cout << "ExpressionStatement\n";
    _indent++;
    node.expression->accept(*this);
    _indent--;
}

void AstPrinter::visit(Block& node) {
    printIndent();
    std::cout << "Block\n";
    _indent++;
    for (auto& statement: node.statements) {
        statement->accept(*this);
    }
    _indent--;
}

void AstPrinter::visit(WhileLoop& node) {
    printIndent();
    std::cout << "WhileLoop\n";
    _indent++;
    printIndent();
    std::cout << "Condition:\n";
    _indent++;
    node.condition->accept(*this);
    _indent--;
    printIndent();
    std::cout << "Body:\n";
    _indent++;
    node.body->accept(*this);
    _indent--;
    _indent--;
}

void AstPrinter::visit(ForLoop& node) {
    printIndent();
    std::cout << "ForLoop\n";
    _indent++;

    printIndent();
    std::cout << "Identifier: " << node.identifier << "\n";

    printIndent();
    std::cout << "Range:\n";
    _indent++;
    node.range->accept(*this);
    _indent--;

    printIndent();
    std::cout << "Step:\n";
    _indent++;
    node.step->accept(*this);
    _indent--;

    printIndent();
    std::cout << "Body:\n";
    _indent++;
    node.body->accept(*this);
    _indent--;

    _indent--;
}

void AstPrinter::visit(IfStatement& node) {
    printIndent();
    std::cout << "IfStatement\n";
    _indent++;
    printIndent();
    std::cout << "Condition:\n";
    _indent++;
    node.condition->accept(*this);
    _indent--;
    printIndent();
    std::cout << "Then:\n";
    _indent++;
    node.thenBranch->accept(*this);
    _indent--;
    if (node.elseBranch) {
        printIndent();
        std::cout << "Else:\n";
        _indent++;
        node.elseBranch->accept(*this);
        _indent--;
    }
    _indent--;
}

void AstPrinter::visit(FunctionDeclaration& node) {
    printIndent();
    std::cout << "FunctionDeclaration: " << node.name << " -> " << node.returnType << "\n";

    _indent++;
    if (node.symbol) {
        printSymbol(*node.symbol);
    }

    if (!node.parameters.empty()) {
        printIndent();
        std::cout << "Parameters:\n";
        _indent++;
        for (auto& param: node.parameters) {
            printIndent();
            std::cout << param.name << ": " << param.type << "\n";
        }
        _indent--;
    }
    printIndent();
    std::cout << "Body:\n";
    _indent++;
    node.body->accept(*this);
    _indent--;
    _indent--;
}

void AstPrinter::visit(RangeExpression& node) {
    printIndent();
    std::cout << "RangeExpression\n";
    _indent++;

    printIndent();
    std::cout << "Start:\n";
    _indent++;
    node.start->accept(*this);
    _indent--;

    printIndent();
    std::cout << "End:\n";
    _indent++;
    node.end->accept(*this);
    _indent--;

    _indent--;
}
