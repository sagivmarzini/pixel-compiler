#ifndef COMPILER_PROJECT_PARSER_H
#define COMPILER_PROJECT_PARSER_H


#include "ast.h"
#include "lex/Lexer.h"

class Parser {
public:
    Parser(Lexer &lex);

    std::unique_ptr<ASTNode> parse();

private:
    std::vector<Token> tokens;
    size_t position;

    Token &current();

    Token &peek(int offset = 1);

    void advance();

    bool match(TokenType type);

    Token expect(TokenType type);

    bool isAtEnd();

    // Recursive descent parsing methods
    std::unique_ptr<ASTNode> parseProgram();

    std::unique_ptr<ASTNode> parseFunctionDeclaration();

    std::unique_ptr<ASTNode> parseStatement();

    std::unique_ptr<ASTNode> parseBlock();

    std::unique_ptr<ASTNode> parseIfStatement();

    std::unique_ptr<ASTNode> parseWhileStatement();

    std::unique_ptr<ASTNode> parseReturnStatement();

    std::unique_ptr<ASTNode> parseExpression();

    std::unique_ptr<ASTNode> parseComparison();

    std::unique_ptr<ASTNode> parseTerm();

    std::unique_ptr<ASTNode> parseFactor();

    std::unique_ptr<ASTNode> parsePrimary();
};


#endif //COMPILER_PROJECT_PARSER_H
