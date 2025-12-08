#include "Parser.h"
#include "AST/Statement.h"

Parser::Parser(std::vector<Token> tokens)
    : _tokens(std::move(tokens)), _position(0) {
}

Program Parser::parseProgram() {
    std::vector<std::unique_ptr<ASTNode> > declarations;

    while (!isAtEnd()) {
        if (matchValue(Keyword::Func)) {
            declarations.push_back(parseFunctionDeclaration());
        } else {
            declarations.push_back(parseStatement());
        }
    }

    return {std::move(declarations)};
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (matchValue(Keyword::Var) || matchValue(Keyword::Const)) {
        return parseVariableDeclaration();
    }
    if (matchValue(Keyword::Return)) {
        return parseReturnStatement();
    }
    if (matchValue(Keyword::If)) {
        return parseIfStatement();
    }
    if (matchValue(Keyword::While)) {
        return parseWhileLoop();
    }
    if (match<Identifier>()) {
        if (matchNext<Operator>()) {
            return parseVariableAssignment();
        }
    }

    throw std::runtime_error("Unexpected token '" + peek().metadata.lexeme + "'!");
}

std::unique_ptr<Statement> Parser::parseBlock() {
    expect<LBrace>();

    Block block;
    while (!match<RBrace>()) {
        if (isAtEnd()) {
            throw std::runtime_error("Missing closing bracket '}'!");
        }
        block.statements.push_back(parseStatement());
    }
    expect<RBrace>();

    return std::make_unique<Block>(std::move(block));
}

std::vector<FunctionArgument> Parser::parseFunctionArguments() {
    std::vector<FunctionArgument> args;
    while (!match<LParen>()) {
        if (isAtEnd()) {
            throw std::runtime_error("Missing closing paren ')'!");
        }
        auto name = expect<Identifier>();
        expect<Colon>();
        auto value = parseExpression();

        args.emplace_back(name.name, std::move(value));

        if (match<RParen>()) {
            expect<Comma>();
        }
    }
    return args;
}

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration() {
    expect<Keyword>();
    auto name = expect<Identifier>();
    expect<LParen>();

    std::vector<FunctionDeclaration::FunctionParameter> parameters;
    //parse parameters
    while (!match<RParen>()) {
        if (isAtEnd()) {
            throw std::runtime_error("Missing closing brace ')'!");
        }

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
    const bool isConst = matchValue(Keyword::Const) ? true : false;
    expect<Keyword>();

    auto name = expect<Identifier>();

    // if initializing with inferred type set it to undefined
    Type type = Type::Unspecified;
    std::unique_ptr<Expression> value = nullptr;
    // if initializing with type get it
    if (match<Colon>()) {
        eat();
        type = expect<Type>();
    }

    if (matchValue(Operator::Assignment)) {
        expect<Operator>();
        value = parseExpression();
    }

    if (!value && type == Type::Unspecified) {
        throw std::runtime_error("Trying to initialize a variable without a type!");
    }
    expect<Semicolon>();

    return std::make_unique<VariableDeclaration>(isConst, type, name.name, std::move(value));
}

std::unique_ptr<Statement> Parser::parseVariableAssignment() {
    auto name = expect<Identifier>();

    if (!matchValue(Operator::Assignment)) {
        throw std::runtime_error("Cannot use a variable without assignment!");
    }
    eat();
    auto value = parseExpression();
    expect<Semicolon>();

    return std::make_unique<VariableAssignment>(name.name, std::move(value));
}

std::unique_ptr<Expression> Parser::parseFunctionCall() {
    auto [name] = expect<Identifier>();

    expect<LParen>();

    //parse arguments
    auto arguments = parseFunctionArguments();

    expect<RParen>();
    expect<Semicolon>();

    return std::make_unique<FunctionCall>(name, std::move(arguments));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    expect<Keyword>();
    expect<LParen>();
    auto condition = parseExpression();
    expect<RParen>();

    auto block = parseBlock();

    std::unique_ptr<Statement> elseBlock = nullptr;

    if (matchValue(Keyword::Else)) {
        expect<Keyword>();
        elseBlock = parseBlock();
    }
    return std::make_unique<IfStatement>(std::move(condition), std::move(block), std::move(elseBlock));
}

std::unique_ptr<Statement> Parser::parseWhileLoop() {
    expect<Keyword>();
    expect<LParen>();
    auto condition = parseExpression();
    expect<RParen>();

    auto block = parseBlock();

    return std::make_unique<WhileLoop>(std::move(condition), std::move(block));
}

std::unique_ptr<RangeExpression> Parser::parseRangeExpression() {
    auto start = parseExpression();
    expect<DoubleDot>();
    auto end = parseExpression();

    return std::make_unique<RangeExpression>(std::move(start), std::move(end));
}

std::unique_ptr<Statement> Parser::parseForLoop() {
    expect<Keyword>();
    auto identifier = expect<Identifier>();
    expectValue(Keyword::In);
    auto range = parseRangeExpression();

    std::unique_ptr<Expression> step;
    if (matchValue(Keyword::Step)) {
        eat();
        step = parseExpression();
    }

    auto body = parseBlock();

    return std::make_unique<ForLoop>(identifier.name, std::move(range), std::move(step), std::move(body));
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    expect<Keyword>();
    auto value = parseExpression();
    expect<Semicolon>();

    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseBooleanOrExpression();
}


std::unique_ptr<Expression> Parser::parseBooleanOrExpression() {
    auto left = parseBooleanAndExpression();

    while (matchValue(Operator::Or)) {
        auto op = expect<Operator>();
        auto right = parseBooleanAndExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseBooleanAndExpression() {
    auto left = parseBooleanEqualityExpression();

    while (matchValue(Operator::And)) {
        auto op = expect<Operator>();
        auto right = parseBooleanEqualityExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}


std::unique_ptr<Expression> Parser::parseBooleanEqualityExpression() {
    auto left = parseComparisonExpression();

    while (matchValue(Operator::Equal) || matchValue(Operator::NotEqual)) {
        auto op = expect<Operator>();
        auto right = parseComparisonExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseComparisonExpression() {
    auto left = parseAdditiveExpression();

    while (matchValue(Operator::Less) || matchValue(Operator::LessEqual)
           || matchValue(Operator::Greater) || matchValue(Operator::GreaterEqual)) {
        auto op = expect<Operator>();
        auto right = parseAdditiveExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();

    while (matchValue(Operator::Plus) || matchValue(Operator::Minus)) {
        auto op = expect<Operator>();
        auto right = parseMultiplicativeExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();

    while (matchValue(Operator::Star) || matchValue(Operator::Slash)) {
        auto op = expect<Operator>();
        auto right = parseUnaryExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseUnaryExpression() {
    if (matchValue(Operator::Plus) || matchValue(Operator::Minus)
        || matchValue(Operator::Exclamation)) {
        auto op = expect<Operator>();
        auto operand = parsePrimary();

        return std::make_unique<UnaryExpression>(std::move(operand), op);
    }
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match<IntegerLiteral>()) {
        return std::make_unique<IntegerLiteralNode>(expect<IntegerLiteral>().value);
    }
    if (match<FloatLiteral>()) {
        return std::make_unique<FloatLiteralNode>(expect<FloatLiteral>().value);
    }
    if (match<BooleanLiteral>()) {
        return std::make_unique<BooleanLiteralNode>(expect<BooleanLiteral>().value);
    }
    if (match<StringLiteral>()) {
        return std::make_unique<StringLiteralNode>(expect<StringLiteral>().value);
    }
    if (match<Identifier>()) {
        auto identifier = expect<Identifier>();

        if (match<LParen>()) {
            expect<LParen>();
            auto arguments = parseFunctionArguments();
            return std::make_unique<FunctionCall>(identifier.name, std::move(arguments));
        }
        return std::make_unique<IdentifierNode>(identifier.name);
    }
    if (match<LParen>()) {
        expect<LParen>();
        auto expression = parseExpression();
        expect<RParen>();
        return expression;
    }
    throw std::runtime_error("Unexpected token '" + peek().metadata.lexeme + "'");
}


Token &Parser::peek() {
    if (_position >= _tokens.size()) {
        return _endOfFileToken;
    }
    return _tokens[_position];
}

Token &Parser::peekNext() {
    auto position = _position + 1;
    if (position >= _tokens.size()) {
        return _endOfFileToken;
    }
    return _tokens[position];
}

template<typename T>
bool Parser::match() {
    return std::holds_alternative<T>(peek().type);
}

template<typename T>
bool Parser::matchNext() {
    return std::holds_alternative<T>(peekNext().type);
}

template<typename T>
bool Parser::matchValue(T value) {
    return match<T>() && std::get<T>(peek().type) == value;
}

template<typename T>
bool Parser::matchNextValue(T value) {
    return matchNext<T>() && std::get<T>(peekNext().type) == value;
}

template<typename T>
T Parser::expect() {
    if (match<T>()) {
        auto tokenType = _tokens[_position].type;
        _position++;
        return std::get<T>(tokenType);
    }
    throw std::runtime_error("Unexpected Token! got '" + tokenToString(peek()) + "'");
}

template<typename T>
T Parser::expectValue(T value) {
    if (!matchValue(value)) {
        throw std::runtime_error("Unexpected Token! got '" + tokenToString(peek()) + "'");
    }
    T v = std::get<T>(peek().type);
    eat();
    return v;
}

void Parser::eat() {
    _position++;
}

bool Parser::isAtEnd() {
    return match<EndOfFile>();
}
