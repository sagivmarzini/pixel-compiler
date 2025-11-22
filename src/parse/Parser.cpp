#include "Parser.h"

#include "AST/Statement.h"

Parser::Parser(std::vector<Token> tokens)
    : _tokens(std::move(tokens)), _position(0) {
}

Program Parser::parse() {
    return parseProgram();
}

Program Parser::parseProgram() {
    std::vector<std::unique_ptr<ASTNode> > declarations = std::vector<std::unique_ptr<ASTNode> >();

    while (!isAtEnd()) {
        declarations.push_back(parseStatement());
    }

    return Program(declarations);
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    return std::visit(
        [this]<typename U>(U &&arg) -> std::unique_ptr<ASTNode> {
            using T = std::decay_t<U>();

            if constexpr (std::is_same<T, Keyword>()) {
                auto keyword = std::decay_t<Keyword>();
                switch (keyword) {
                    case Keyword::Func:
                        return parseFunctionDeclaration();

                    case Keyword::Var:
                        return parseVariableDeclaration();

                    case Keyword::Return:
                        return parseReturnStatement();

                    case Keyword::If:
                        return parseIfStatement();

                    case Keyword::While:
                        return parseWhileStatement();

                    default:
                        throw std::runtime_error("Unexpected token '" + peek().lexeme + "'!");
                }
            } else if constexpr (std::is_same<T, Identifier>()) {
                return parseStatementIdentifier();
            } else {
                throw std::runtime_error("Unexpected token '" + peek().lexeme + "'");
            }
        }, peek().type);
}

Token &Parser::peek(int offset) {
    if (_tokens.size() <= _position + offset) {
        return _tokens.back(); //return last token if passed the end
    }
    return _tokens[_position + offset];
}

template<typename T>
bool Parser::match() {
    return std::holds_alternative<T>(peek().type);
}

template<typename T>
Token Parser::expect() {
    if (match<T>()) {
        Token token = _tokens[_position];
        _position += 1;
        return token;
    }
    throw std::runtime_error("Unexpected Token! got '" + tokenToString(peek()) + "'");
}

bool Parser::isAtEnd() {
    return match<EndOfFile>();
}
