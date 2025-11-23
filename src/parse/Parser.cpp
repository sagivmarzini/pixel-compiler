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

std::unique_ptr<Statement> Parser::parseStatement() {
    return std::visit(
        [this]<typename U>(U &&arg) -> std::unique_ptr<Statement> {
            using T = std::decay_t<U>();

            if constexpr (std::is_same<T, Keyword>()) {
                switch (std::decay_t<Keyword>()) {
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
                if (std::holds_alternative<LParen>(peek(1).type)) {
                    //if the token after an Identifier is a ( then it is a function call
                    return parseFunctionCall();
                } else if (std::holds_alternative<Operator>(peek(1).type)) {
                    return parseVariableAssignment();
                }
            } else {
                throw std::runtime_error("Unexpected token '" + peek().lexeme + "'");
            }
        }, peek().type);
}

std::unique_ptr<Statement> Parser::parseBlock() {
    expect<RBrace>();

    Block block;
    while (!match<LBrace>()) {
        block.statements.push_back(parseStatement());
    }
    expect<LBrace>();

    return std::make_unique<Block>(std::move(block));
}

std::unique_ptr<Statement> Parser::parseFunctionDeclaration() {
    expect<Keyword>();
    auto name = expect<Identifier>();
    expect<LParen>();

    //parse parameters
    auto parameters = std::vector<FunctionDeclaration::Parameter>();
    while (!match<RParen>()) {
        auto paramName = expect<Identifier>();
        expect<Colon>();
        auto paramType = expect<Type>();

        parameters.emplace_back(paramName, paramType);
        if (!match<RParen>())
            expect<Comma>(); //if didn't read the end, get a comma seperator [func foo(a:int, b:int)]
    }
    expect<RParen>();
    expect<Arrow>();

    auto returnType = expect<Type>();
    auto block = parseBlock();

    return std::make_unique<FunctionDeclaration>(returnType, name, parameters, std::move(block));
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    expect<Keyword>();
    auto name = expect<Identifier>();

    // if initializing with inferred type set it to undefined
    Type type = Type::Undefined;
    std::unique_ptr<Expression> value = nullptr;
    //if initializing with type get it
    if (match<Colon>()) {
        expect<Colon>();
        type = expect<Type>();
    }

    if (std::get<Operator>(peek().type) == Operator::Assignment) {
        expect<Operator>();
        value = parseExpression();
    }

    if (!value && type == Type::Undefined) {
        throw std::runtime_error("Trying to initialize a variable without a type!");
    }
    expect<Semicolon>();

    return std::make_unique<VariableDeclaration>(type, name, std::move(value));
}

std::unique_ptr<Statement> Parser::parseVariableAssignment() {
    auto name = expect<Identifier>();

    if (std::get<Operator>(peek().type) != Operator::Assignment) {
        throw std::runtime_error("Cannot use a variable without assignment!");
    }

    auto value = parseExpression();
    return std::make_unique<VariableAssignment>(name, std::move(value));
}

std::unique_ptr<Statement> Parser::parseFunctionCall() {
    auto name = expect<Identifier>();

    expect<LParen>();

    //parse arguments
    auto arguments = std::vector<std::unique_ptr<Expression> >();
    while (!match<RParen>()) {
        auto arg = parseExpression();

        arguments.emplace_back(std::move(arg));
        if (!match<RParen>())
            expect<Comma>(); //if didn't read the end, get a comma seperator [foo(a, 5)]
    }
    expect<RParen>();
    expect<Semicolon>();

    return std::make_unique<FunctionCall>(name, std::move(arguments));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    expect<Keyword>();
    expect<LBrace>();
    auto condition = parseExpression();
    expect<RBrace>();

    auto block = parseBlock();

    std::unique_ptr<Statement> elseBlock = nullptr;

    auto keyword = std::get_if<Keyword>(&peek().type);
    if (keyword && *keyword == Keyword::Else) {
        expect<Keyword>();
        elseBlock = parseBlock();
    }
    return std::make_unique<IfStatement>(std::move(condition), std::move(block), std::move(elseBlock));
}

std::unique_ptr<Statement> Parser::parseWhileStatement() {
    expect<Keyword>();
    expect<LParen>();
    auto condition = parseExpression();
    expect<RParen>();

    auto block = parseBlock();

    return std::make_unique<WhileStatement>(std::move(condition), std::move(block));
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    expect<Keyword>();
    auto value = parseExpression();

    return std::make_unique<ReturnStatement>(std::move(value));
}


Token &Parser::peek(int offset) {
    if (_tokens.size() <= _position + offset) {
        return _tokens.back(); //return last token if passed the end
    }
    return _tokens[_position + offset];
}

template
<typename T>

bool Parser::match() {
    return std::holds_alternative<T>(peek().type);
}

template
<typename T>
T Parser::expect() {
    if (match<T>()) {
        Token token = _tokens[_position];
        _position += 1;
        return std::get<T>(token.type);
    }
    throw std::runtime_error("Unexpected Token! got '" + tokenToString(peek()) + "'");
}

bool Parser::isAtEnd() {
    return match<EndOfFile>();
}
