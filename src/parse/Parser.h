#ifndef COMPILER_PROJECT_PARSER_H
#define COMPILER_PROJECT_PARSER_H

#include "AST/AstNode.h"
#include "lex/Lexer.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);

    Program parse();

private:
    std::vector<Token> _tokens;
    size_t _position;

    Program parseProgram();

    // Recursive descent parsing methods
    std::unique_ptr<ASTNode> parseStatement();

    std::unique_ptr<ASTNode> parseBlock();

    std::unique_ptr<ASTNode> parseFunctionDeclaration();

    std::unique_ptr<ASTNode> parseVariableDeclaration();

    //parses statements that begin with an identifier like function calls or variable assignments
    std::unique_ptr<ASTNode> parseStatementIdentifier();

    std::unique_ptr<ASTNode> parseIfStatement();

    std::unique_ptr<ASTNode> parseWhileStatement();

    std::unique_ptr<ASTNode> parseReturnStatement();

    std::unique_ptr<ASTNode> parseExpression();

    std::unique_ptr<ASTNode> parseComparison();

    std::unique_ptr<ASTNode> parseTerm();

    std::unique_ptr<ASTNode> parseFactor();

    std::unique_ptr<ASTNode> parsePrimary();

    Token &peek(int offset = 1);

    template<typename T>
    bool match();

    template<typename T>
    Token expect(); //expects and eats the token if it matches

    bool isAtEnd();
};


#endif //COMPILER_PROJECT_PARSER_H
