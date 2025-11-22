#ifndef COMPILER_PROJECT_PARSER_H
#define COMPILER_PROJECT_PARSER_H

#include "AST/Statement.h"
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
    std::unique_ptr<Statement> parseStatement();

    std::unique_ptr<Statement> parseBlock();

    std::unique_ptr<Statement> parseFunctionDeclaration();

    std::unique_ptr<Statement> parseVariableDeclaration();

    std::unique_ptr<Statement> parseVariableAssignment();

    std::unique_ptr<Statement> parseFunctionCall();

    std::unique_ptr<Statement> parseIfStatement();

    std::unique_ptr<Statement> parseWhileStatement();

    std::unique_ptr<Statement> parseReturnStatement();

    std::unique_ptr<Expression> parseExpression();

    std::unique_ptr<Expression> parseComparison();

    std::unique_ptr<Expression> parseTerm();

    std::unique_ptr<Expression> parseFactor();

    std::unique_ptr<Expression> parsePrimary();

    Token &peek(int offset = 1);

    template<typename T>
    bool match();

    template<typename T>
    T expect(); //expects and eats the token if it matches

    bool isAtEnd();
};


#endif //COMPILER_PROJECT_PARSER_H
