#include "ASTPrinter.h"

#include <iostream>

#include "Statement.h"

void ASTPrinter::visit(IntegerLiteralNode &node) {
    std::cout << node.value << ' ';
}

void ASTPrinter::visit(StringLiteralNode &node) {
    std::cout << '\"' + node.value + "\" ";
}

void ASTPrinter::visit(BooleanLiteralNode &node) {
    std::cout << (node.value ? "true" : "false") << ' ';
}

void ASTPrinter::visit(BinaryExpression &node) {
    node.left->accept(*this);
    std::cout << ' ' << Token{node.op, 0, 0, ""} << ' ';
    node.right->accept(*this);
}

void ASTPrinter::visit(UnaryExpression &node) {
    std::cout << Token{node.op, 0, 0, ""} << ' ';
    node.operand->accept(*this);
}

void ASTPrinter::visit(IdentifierNode &node) {
    std::cout << node.name.name << ' ';
}

void ASTPrinter::visit(CallExpression &node) {
    std::cout << node.functionName.name << '(';
    for (auto &arg: node.arguments) {
        arg->accept(*this);
        if (arg != *node.arguments.end()) {
            std::cout << ", ";
        }
    }
    std::cout << ')';
}

void ASTPrinter::visit(VariableDeclaration &node) {
    std::cout << node.name.name << " : " << Token{node.type, 0, 0, ""}
            << " = ";
    if (node.initializer) {
        node.initializer->accept(*this);
    }
    std::cout << std::endl;
}

void ASTPrinter::visit(VariableAssignment &node) {
    std::cout << node.name.name << " = ";
    node.newValue->accept(*this);
    std::cout << std::endl;
}

void ASTPrinter::visit(ReturnStatement &node) {
    std::cout << "return ";
    node.value->accept(*this);
    std::cout << std::endl;
}

void ASTPrinter::visit(Block &node) {
    std::cout << "{\n";
    for (auto &statement: node.statements) {
        statement->accept(*this);
    }
    std::cout << "}\n";
}

void ASTPrinter::visit(WhileStatement &node) {
    std::cout << "while (";
    node.condition->accept(*this);
    std::cout << ")\n";
    node.body->accept(*this);
}

void ASTPrinter::visit(IfStatement &node) {
    std::cout << "if (";
    node.condition->accept(*this);
    std::cout << ")\n";
    node.thenBranch->accept(*this);
    if (node.elseBranch) {
        std::cout << "else\n";
        node.elseBranch->accept(*this);
    }
}

void ASTPrinter::visit(FunctionDeclaration &node) {
    std::cout << node.name.name << '(';
    for (auto &param: node.parameters) {
        std::cout << param.name.name << ": " << Token{param.type, 0, 0, ""} << ", ";
    }
    std::cout << ") -> " << Token{node.returnType, 0, 0, ""} << std::endl;
    node.body->accept(*this);
}

void ASTPrinter::visit(FunctionCall &node) {
    std::cout << node.functionName.name << '(';
    for (auto &arg: node.arguments) {
        arg->accept(*this);
        if (arg != *node.arguments.end()) {
            std::cout << ", ";
        }
    }
    std::cout << ")\n";
}

void ASTPrinter::visit(Program &program) {
    std::cout << "program:\n";
    for (auto &node: program.declarations) {
        node->accept(*this);
    }
}
