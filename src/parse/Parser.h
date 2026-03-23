#ifndef COMPILER_PROJECT_PARSER_H
#define COMPILER_PROJECT_PARSER_H

#include "AST/Statement.h"
#include "lex/Lexer.h"

class ParserError;
enum class ParserErrorType;

class Parser {
public:
    Parser(std::vector<Token> tokens);

    AST::Program parseProgram();

    struct ParseUnwindException : std::runtime_error {
        ParseUnwindException() : std::runtime_error("") {
        }
    };

private:
    std::vector<Token> _tokens;
    size_t _position;
    Token _endOfFileToken = Token(EndOfFile(), -1, -1, "EOF");
    std::vector<CompilerError> _errors;
    // A flag to prevent multiple errors from one single mistake
    bool _isPanicMode = false;

    std::unique_ptr<AST::Statement> parseDeclaration();


    std::unique_ptr<AST::Statement> parseStatement();

    std::unique_ptr<AST::Block> parseBlock();

    std::vector<AST::FunctionCall::FunctionArgument> parseFunctionArguments();

    std::unique_ptr<AST::Statement> parseFunctionDeclaration();

    std::unique_ptr<AST::Statement> parseVariableDeclaration();

    std::unique_ptr<AST::Statement> parseVariableAssignment();

    std::unique_ptr<AST::Statement> parseArrayAssignment();

    std::unique_ptr<AST::Expression> parseFunctionCall();

    std::unique_ptr<AST::Statement> parseIfStatement();

    std::unique_ptr<AST::Statement> parseWhileLoop();

    std::unique_ptr<AST::RangeExpression> parseRangeExpression();

    std::unique_ptr<AST::Statement> parseForLoop();

    std::unique_ptr<AST::Statement> parseReturnStatement();

    std::unique_ptr<AST::Expression> parseExpression();

    std::unique_ptr<AST::Expression> parseBooleanOrExpression();

    std::unique_ptr<AST::Expression> parseBooleanAndExpression();

    std::unique_ptr<AST::Expression> parseBooleanEqualityExpression();

    std::unique_ptr<AST::Expression> parseComparisonExpression();

    std::unique_ptr<AST::Expression> parseAdditiveExpression();

    std::unique_ptr<AST::Expression> parseMultiplicativeExpression();

    std::unique_ptr<AST::Expression> parseUnaryExpression();

    std::unique_ptr<AST::Expression> parseIncDecExpression();


    std::unique_ptr<AST::Expression> parsePrimary();

    std::unique_ptr<AST::Expression> parseArrayLiteral();

    std::unique_ptr<AST::Expression> parseArrayIndex();

    Token& peek();

    Token& peekPrevious();

    Token& peekNext();

    template<typename T>
    bool check();

    template<typename T>
    bool checkNext();

    template<typename T>
    bool match(); // Check token type and advance if matches

    template<typename T>
    bool matchValue(T value); // Check and advance if matches

    template<typename T>
    bool checkValue(T value);

    template<typename T>
    bool checkNextValue(T value);

    void advance();

    template<typename T>
    T expect(); // expects and eats the token if it matches
    template<typename T>
    T expectValue(T value);

    bool isAtEnd();

    void synchronize();

    bool isAtStartOfStatement();

    [[noreturn]] void logError(const ParserErrorType& type, const Token& errorToken,
                               const TokenType& expectedTokenType = ScalarKind::Unspecified);
};


#endif //COMPILER_PROJECT_PARSER_H
