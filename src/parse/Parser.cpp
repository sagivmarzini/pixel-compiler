#include "Parser.h"
#include "AST/Statement.h"

Parser::Parser(std::vector<Token> tokens)
    : _tokens(std::move(tokens)), _position(0) {
}

Program Parser::parseProgram() {
    std::vector<std::unique_ptr<AstNode> > declarations;

    while (!isAtEnd()) {
        declarations.push_back(parseStatement());
    }

    return {std::move(declarations)};
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (check<LBrace>()) {
        return parseBlock();
    }
    if (checkValue(Keyword::Var) || checkValue(Keyword::Const)) {
        return parseVariableDeclaration();
    }
    if (checkValue(Keyword::Func)) {
        return parseFunctionDeclaration();
    }
    if (checkValue(Keyword::Return)) {
        return parseReturnStatement();
    }
    if (checkValue(Keyword::If)) {
        return parseIfStatement();
    }
    if (checkValue(Keyword::While)) {
        return parseWhileLoop();
    }
    if (checkValue(Keyword::For)) {
        return parseForLoop();
    }
    if (check<Identifier>()) {
        if (checkNextValue(Operator::Assignment)) {
            return parseVariableAssignment();
        }
    }

    // Otherwise parse expression statement
    auto expression = parseExpression();
    expect<Semicolon>();
    return std::make_unique<ExpressionStatement>(std::move(expression));
}

std::unique_ptr<Statement> Parser::parseBlock() {
    expect<LBrace>();

    Block block;
    while (!check<RBrace>()) {
        if (isAtEnd()) {
            throw std::runtime_error("Missing closing bracket '}'!");
        }
        block.statements.push_back(parseStatement());
    }
    expect<RBrace>();

    return std::make_unique<Block>(std::move(block));
}

std::vector<FunctionCall::FunctionArgument> Parser::parseFunctionArguments() {
    std::vector<FunctionCall::FunctionArgument> args;
    while (!check<RParen>()) {
        if (isAtEnd()) throw std::runtime_error("Missing closing paren ')'!");

        auto name = expect<Identifier>();
        expect<Colon>();
        auto value = parseExpression();

        args.emplace_back(name.name, std::move(value));

        if (!check<RParen>())
            expect<Comma>();
    }

    return args;
}

std::unique_ptr<Statement> Parser::parseFunctionDeclaration() {
    expect<Keyword>();
    auto name = expect<Identifier>();
    expect<LParen>();

    std::vector<FunctionDeclaration::FunctionParameter> parameters;
    //parse parameters
    while (!check<RParen>()) {
        if (isAtEnd()) {
            throw std::runtime_error("Missing closing brace ')'!");
        }

        auto paramName = expect<Identifier>();
        expect<Colon>();
        auto paramType = expect<Type>();

        parameters.emplace_back(paramName.name, paramType);
        if (!check<RParen>())
            expect<Comma>(); // if didn't read the end, get a comma seperator [func foo(a:int, b:int)]
    }

    expect<RParen>();
    expect<Arrow>();

    auto returnType = expect<Type>();
    auto block      = parseBlock();

    return std::make_unique<FunctionDeclaration>(returnType, name.name, parameters, std::move(block));
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    const bool isConst = checkValue(Keyword::Const) ? true : false;
    expect<Keyword>();

    auto [name] = expect<Identifier>();

    // if initializing with inferred type set it to undefined
    Type                        type  = Type::Unspecified;
    std::unique_ptr<Expression> value = nullptr;
    // if initializing with type get it
    if (match<Colon>()) {
        type = expect<Type>();
    }

    if (checkValue(Operator::Assignment)) {
        expect<Operator>();
        value = parseExpression();
    }

    if (!value && type == Type::Unspecified) {
        throw std::runtime_error("Trying to initialize a variable without a type!");
    }
    expect<Semicolon>();

    return std::make_unique<VariableDeclaration>(isConst, type, name, std::move(value));
}

std::unique_ptr<Statement> Parser::parseVariableAssignment() {
    auto name = expect<Identifier>();

    expectValue(Operator::Assignment);

    auto value = parseExpression();
    expect<Semicolon>();

    return std::make_unique<VariableAssignment>(name.name, std::move(value));
}

std::unique_ptr<Expression> Parser::parseFunctionCall() {
    auto [name] = expect<Identifier>();

    expect<LParen>();
    auto arguments = parseFunctionArguments();
    expect<RParen>();

    return std::make_unique<FunctionCall>(name, std::move(arguments));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    expect<Keyword>();
    auto condition = parseExpression();

    auto thenBranch = parseStatement();

    std::unique_ptr<Statement> elseBranch = nullptr;

    if (matchValue(Keyword::Else)) {
        if (checkValue(Keyword::If)) {
            elseBranch = parseIfStatement(); // nested if
        } else {
            elseBranch = parseStatement(); // normal else
        }
    }
    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::parseWhileLoop() {
    expect<Keyword>();
    auto condition = parseExpression();

    auto block = parseStatement();

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

    std::unique_ptr<Expression> step = std::make_unique<IntegerLiteralNode>(1);
    if (matchValue(Keyword::Step)) {
        step = parseExpression();
    }

    auto body = parseStatement();

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

    while (checkValue(Operator::Or)) {
        auto op    = expect<Operator>();
        auto right = parseBooleanAndExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseBooleanAndExpression() {
    auto left = parseBooleanEqualityExpression();

    while (checkValue(Operator::And)) {
        auto op    = expect<Operator>();
        auto right = parseBooleanEqualityExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}


std::unique_ptr<Expression> Parser::parseBooleanEqualityExpression() {
    auto left = parseComparisonExpression();

    while (checkValue(Operator::Equal) || checkValue(Operator::NotEqual)) {
        auto op    = expect<Operator>();
        auto right = parseComparisonExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseComparisonExpression() {
    auto left = parseAdditiveExpression();

    while (checkValue(Operator::LessThan) || checkValue(Operator::LessEqual)
           || checkValue(Operator::GreaterThan) || checkValue(Operator::GreaterEqual)) {
        auto op    = expect<Operator>();
        auto right = parseAdditiveExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();

    while (checkValue(Operator::Plus) || checkValue(Operator::Minus)) {
        auto op    = expect<Operator>();
        auto right = parseMultiplicativeExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();

    while (checkValue(Operator::Star) || checkValue(Operator::Slash)) {
        auto op    = expect<Operator>();
        auto right = parseUnaryExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseUnaryExpression() {
    if (checkValue(Operator::Plus) || checkValue(Operator::Minus)
        || checkValue(Operator::Exclamation)) {
        auto op      = expect<Operator>();
        auto operand = parsePrimary();

        return std::make_unique<UnaryExpression>(std::move(operand), op);
    }
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (check<IntegerLiteral>()) {
        return std::make_unique<IntegerLiteralNode>(expect<IntegerLiteral>().value);
    }
    if (check<FloatLiteral>()) {
        return std::make_unique<FloatLiteralNode>(expect<FloatLiteral>().value);
    }
    if (check<BooleanLiteral>()) {
        return std::make_unique<BooleanLiteralNode>(expect<BooleanLiteral>().value);
    }
    if (check<StringLiteral>()) {
        return std::make_unique<StringLiteralNode>(expect<StringLiteral>().value);
    }
    if (check<Identifier>()) {
        if (checkNext<LParen>()) return parseFunctionCall();

        return std::make_unique<IdentifierNode>(expect<Identifier>().name);
    }
    if (check<LParen>()) {
        expect<LParen>();
        auto expression = parseExpression();
        expect<RParen>();
        return expression;
    }
    throw std::runtime_error("Unexpected token '" + peek().metadata.lexeme + "'");
}


Token& Parser::peek() {
    if (_position >= _tokens.size()) {
        return _endOfFileToken;
    }
    return _tokens[_position];
}

Token& Parser::peekNext() {
    auto position = _position + 1;
    if (position >= _tokens.size()) {
        return _endOfFileToken;
    }
    return _tokens[position];
}

template<typename T>
bool Parser::check() {
    return std::holds_alternative<T>(peek().type);
}

template<typename T>
bool Parser::match() {
    const bool match = check<T>();
    if (match) eat();
    return match;
}

template<typename T>
bool Parser::matchValue(T value) {
    const bool match = checkValue(value);
    if (match) eat();
    return match;
}

template<typename T>
bool Parser::checkNext() {
    return std::holds_alternative<T>(peekNext().type);
}

template<typename T>
bool Parser::checkValue(T value) {
    return check<T>() && std::get<T>(peek().type) == value;
}

template<typename T>
bool Parser::checkNextValue(T value) {
    return checkNext<T>() && std::get<T>(peekNext().type) == value;
}

template<typename T>
T Parser::expect() {
    if (check<T>()) {
        auto tokenType = _tokens[_position].type;
        _position++;
        return std::get<T>(tokenType);
    }
    throw std::runtime_error(
        "Unexpected token! expected " + std::string{typeid(T).name()} + ", got '" + tokenToString(peek()) + "'");
}

template<typename T>
T Parser::expectValue(T value) {
    if (!checkValue(value)) {
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
    return check<EndOfFile>();
}
