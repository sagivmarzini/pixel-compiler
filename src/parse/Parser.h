#ifndef COMPILER_PROJECT_PARSER_H
#define COMPILER_PROJECT_PARSER_H

#include "AST/Statement.h"
#include "lex/Lexer.h"

class Parser {
public:
    Parser(std::vector<Token> tokens);

    Program parseProgram();

private:
    std::vector<Token> _tokens;
    size_t _position;
    Token _endOfFileToken = Token(EndOfFile(), -1, -1, "EOF");

    std::unique_ptr<Statement> parseStatement();

    std::unique_ptr<Statement> parseBlock();

    std::vector<FunctionCall::FunctionArgument> parseFunctionArguments();

    std::unique_ptr<ASTNode> parseFunctionDeclaration();

    std::unique_ptr<Statement> parseVariableDeclaration();

    std::unique_ptr<Statement> parseVariableAssignment();

    std::unique_ptr<Expression> parseFunctionCall();

    std::unique_ptr<Statement> parseIfStatement();

    std::unique_ptr<Statement> parseWhileLoop();

    std::unique_ptr<RangeExpression> parseRangeExpression();

    std::unique_ptr<Statement> parseForLoop();

    std::unique_ptr<Statement> parseReturnStatement();

    std::unique_ptr<Expression> parseExpression();

    std::unique_ptr<Expression> parseBooleanOrExpression();

    std::unique_ptr<Expression> parseBooleanAndExpression();

    std::unique_ptr<Expression> parseBooleanEqualityExpression();

    std::unique_ptr<Expression> parseComparisonExpression();

    std::unique_ptr<Expression> parseAdditiveExpression();

    std::unique_ptr<Expression> parseMultiplicativeExpression();

    std::unique_ptr<Expression> parseUnaryExpression();

    std::unique_ptr<Expression> parsePrimary();

    Token &peek();

    Token &peekNext();

    template<typename T>
    bool match();

    template<typename T>
    bool matchNext();

    template<typename T>
    bool matchValue(T value);

    template<typename T>
    bool matchNextValue(T value);

    void eat();

    template<typename T>
    T expect(); // expects and eats the token if it matches
    template<typename T>
    T expectValue(T value);

    bool isAtEnd();
};


#endif //COMPILER_PROJECT_PARSER_H
