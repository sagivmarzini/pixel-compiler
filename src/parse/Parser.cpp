#include "Parser.h"

#include "AST/Statement.h"

Parser::Parser(std::vector<Token> tokens)
    : _tokens(std::move(tokens)), _position(0) {
}

Program Parser::parseProgram() {
    std::vector<std::unique_ptr<ASTNode> > declarations;

    while (!isAtEnd()) {
        declarations.push_back(parseStatement());
    }

    return {std::move(declarations)};
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (matchEnum(Keyword::Func)) {
        return parseFunctionDeclaration();
    }
    if (matchEnum(Keyword::Var)) {
        return parseVariableDeclaration();
    }
    if (matchEnum(Keyword::Return)) {
        return parseReturnStatement();
    }
    if (matchEnum(Keyword::If)) {
        return parseIfStatement();
    }
    if (matchEnum(Keyword::While)) {
        return parseWhileStatement();
    }
    if (match<Identifier>()) {
        if (match<LParen>(1)) {
            //if the token after an Identifier is a ( then it is a function call
            return parseFunctionCall();
        }
        if (match<Operator>(1)) {
            return parseVariableAssignment();
        }
    }

    throw std::runtime_error("Unexpected token '" + peek().lexeme + "'!");
}

std::unique_ptr<Statement> Parser::parseBlock() {
    expect<LBrace>();

    Block block;
    while (!match<RBrace>()) {
        block.statements.push_back(parseStatement());
    }
    expect<RBrace>();

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

        parameters.emplace_back(paramName.name, paramType);
        if (!match<RParen>())
            expect<Comma>(); //if didn't read the end, get a comma seperator [func foo(a:int, b:int)]
    }
    expect<RParen>();
    expect<Arrow>();

    auto returnType = expect<Type>();
    auto block = parseBlock();

    return std::make_unique<FunctionDeclaration>(returnType, name.name, parameters, std::move(block));
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    expect<Keyword>();
    auto name = expect<Identifier>();

    // if initializing with inferred type set it to undefined
    Type type = Type::Unspecified;
    std::unique_ptr<Expression> value = nullptr;
    //if initializing with type get it
    if (match<Colon>()) {
        expect<Colon>();
        type = expect<Type>();
    }

    if (matchEnum(Operator::Assignment)) {
        expect<Operator>();
        value = parseExpression();
    }

    if (!value && type == Type::Unspecified) {
        throw std::runtime_error("Trying to initialize a variable without a type!");
    }
    expect<Semicolon>();

    return std::make_unique<VariableDeclaration>(type, name.name, std::move(value));
}

std::unique_ptr<Statement> Parser::parseVariableAssignment() {
    auto name = expect<Identifier>();

    if (!matchEnum(Operator::Assignment)) {
        throw std::runtime_error("Cannot use a variable without assignment!");
    }
    expect<Operator>();
    auto value = parseExpression();
    expect<Semicolon>();

    return std::make_unique<VariableAssignment>(name.name, std::move(value));
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

    return std::make_unique<FunctionCall>(name.name, std::move(arguments));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    expect<Keyword>();
    expect<LParen>();
    auto condition = parseExpression();
    expect<RParen>();

    auto block = parseBlock();

    std::unique_ptr<Statement> elseBlock = nullptr;

    if (matchEnum(Keyword::Else)) {
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
    expect<Semicolon>();

    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseBooleanAndExpression();
}

std::unique_ptr<Expression> Parser::parseBooleanAndExpression() {
    auto left = parseBooleanOrExpression();

    while (matchEnum(Operator::And)) {
        auto op = expect<Operator>();
        auto right = parseBooleanOrExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseBooleanOrExpression() {
    auto left = parseBooleanEqualityExpression();

    while (matchEnum(Operator::Or)) {
        auto op = expect<Operator>();
        auto right = parseBooleanEqualityExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseBooleanEqualityExpression() {
    auto left = parseComparisonExpression();

    while (matchEnum(Operator::Equal) || matchEnum(Operator::NotEqual)) {
        auto op = expect<Operator>();
        auto right = parseComparisonExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseComparisonExpression() {
    auto left = parseAdditiveExpression();

    while (matchEnum(Operator::Less) || matchEnum(Operator::LessEqual)
           || matchEnum(Operator::Greater) || matchEnum(Operator::GreaterEqual)) {
        auto op = expect<Operator>();
        auto right = parseAdditiveExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();

    while (matchEnum(Operator::Plus) || matchEnum(Operator::Minus)) {
        auto op = expect<Operator>();
        auto right = parseMultiplicativeExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();

    while (matchEnum(Operator::Star) || matchEnum(Operator::Slash)) {
        auto op = expect<Operator>();
        auto right = parseUnaryExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseUnaryExpression() {
    if (matchEnum(Operator::Plus) || matchEnum(Operator::Minus)
        || matchEnum(Operator::Exclamation)) {
        auto op = expect<Operator>();
        auto operand = parsePrimary();

        return std::make_unique<UnaryExpression>(std::move(operand), op);
    }
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match<IntegerLiteral>()) {
        auto value = expect<IntegerLiteral>().value;
        return std::make_unique<IntegerLiteralNode>(value);
    }
    if (match<BooleanLiteral>()) {
        auto value = expect<BooleanLiteral>().value;
        return std::make_unique<BooleanLiteralNode>(value);
    }
    if (match<Identifier>()) {
        auto name = expect<Identifier>();

        if (match<LParen>()) {
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
            return std::make_unique<CallExpression>(name.name, std::move(arguments));
        }
        return std::make_unique<IdentifierNode>(name.name);
    }
    if (match<LParen>()) {
        expect<LParen>();
        auto expression = parseExpression();
        expect<RParen>();
        return expression;
    }
    throw std::runtime_error("Unexpected token '" + peek().lexeme + "'");
}


Token &Parser::peek(int offset) {
    if (_tokens.size() <= _position + offset) {
        return _tokens.back(); //return last token if passed the end
    }
    return _tokens[_position + offset];
}

template<typename T>
bool Parser::match(int offset) {
    return std::holds_alternative<T>(peek(offset).type);
}

template<typename T>
bool Parser::matchEnum(T type) {
    if (match<T>()) {
        return std::get<T>(peek().type) == type;
    }
    return false;
}

template<typename T>
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
