#include "Parser.h"

#include "CompilerException.h"
#include "AST/Statement.h"
#include "ParserError.h"

Parser::Parser(std::vector<Token> tokens)
    : _tokens(std::move(tokens)), _position(0) {
}

Program Parser::parseProgram() {
    Program program({0, 0, ""});

    while (!isAtEnd()) {
        try {
            program.addStatement(parseStatement());
        } catch (const ParseUnwindException& e) {
            synchronize();
        }
    }

    if (!_errors.empty()) {
        throw CompilerException(_errors);
    }

    return program;
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
    if (matchValue(Keyword::Else)) {
        error(ParserErrorType::ElseWithoutIf, peekPrevious());
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
    const auto startToken = peek();
    auto       expression = parseExpression();
    expect<Semicolon>();
    return std::make_unique<ExpressionStatement>(startToken.metadata, std::move(expression));
}

std::unique_ptr<Block> Parser::parseBlock() {
    const auto startToken = peek();
    expect<LBrace>();

    Block block(peekPrevious().metadata);
    while (!check<RBrace>()) {
        if (isAtEnd()) {
            error(ParserErrorType::MissingClosingBrace, startToken);
        }
        block.statements.push_back(parseStatement());
    }
    expect<RBrace>();

    return std::make_unique<Block>(std::move(block));
}

std::vector<FunctionCall::FunctionArgument> Parser::parseFunctionArguments() {
    std::vector<FunctionCall::FunctionArgument> args;
    while (!check<RParen>()) {
        if (isAtEnd())
            error(ParserErrorType::MissingClosingParen, peek());

        auto name = expect<Identifier>();
        expect<Colon>();
        auto value = parseExpression();

        args.emplace_back(name.name, std::move(value));

        if (!check<RParen>()) {
            expect<Comma>();
            if (check<RParen>())
                error(ParserErrorType::TrailingComma, peek());
        }
    }

    return args;
}

std::unique_ptr<Statement> Parser::parseFunctionDeclaration() {
    expect<Keyword>();
    auto [name]       = expect<Identifier>();
    auto namePosition = peekPrevious().metadata;
    expect<LParen>();

    std::vector<FunctionDeclaration::FunctionParameter> parameters;
    //parse parameters
    while (!check<RParen>()) {
        if (isAtEnd()) {
            error(ParserErrorType::MissingClosingParen, peek());
        }

        auto paramName = expect<Identifier>();
        expect<Colon>();
        auto paramType = expect<Type>();

        parameters.emplace_back(paramName.name, paramType);
        if (!check<RParen>()) {
            expect<Comma>(); // if didn't read the end, get a comma seperator
            // Check for trailing comma
            if (check<RParen>())
                error(ParserErrorType::TrailingComma, peek());
        }
    }

    expect<RParen>();
    expect<Arrow>();

    auto returnType = expect<Type>();
    auto block      = parseBlock();

    return std::make_unique<FunctionDeclaration>(namePosition, returnType, name, parameters, std::move(block));
}

std::unique_ptr<Statement> Parser::parseVariableDeclaration() {
    const bool isConst = checkValue(Keyword::Const) ? true : false;
    expect<Keyword>();

    const auto varNameToken = peek(); // for logging error on the variable
    auto       [name]       = expect<Identifier>();

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
        error(ParserErrorType::TypelessVarDeclaration, varNameToken);
    }
    expect<Semicolon>();

    return std::make_unique<VariableDeclaration>(varNameToken.metadata, isConst, type, name, std::move(value));
}

std::unique_ptr<Statement> Parser::parseVariableAssignment() {
    auto       name    = expect<Identifier>();
    const auto namePos = peekPrevious().metadata;

    expectValue(Operator::Assignment);

    auto value = parseExpression();
    expect<Semicolon>();

    return std::make_unique<VariableAssignment>(namePos, name.name, std::move(value));
}

std::unique_ptr<Expression> Parser::parseFunctionCall() {
    auto       [name]  = expect<Identifier>();
    const auto namePos = peekPrevious().metadata;

    expect<LParen>();
    auto arguments = parseFunctionArguments();
    expect<RParen>();

    return std::make_unique<FunctionCall>(namePos, name, std::move(arguments));
}

std::unique_ptr<Statement> Parser::parseIfStatement() {
    expect<Keyword>();
    const auto ifPos     = peekPrevious().metadata;
    auto       condition = parseExpression();

    auto thenBranch = parseStatement();

    std::unique_ptr<Statement> elseBranch = nullptr;

    if (matchValue(Keyword::Else)) {
        if (checkValue(Keyword::If)) {
            elseBranch = parseIfStatement(); // nested if
        } else {
            elseBranch = parseStatement(); // normal else
        }
    }
    return std::make_unique<IfStatement>(ifPos, std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::parseWhileLoop() {
    expect<Keyword>();
    const auto whilePos  = peekPrevious().metadata;
    auto       condition = parseExpression();

    auto block = parseStatement();

    return std::make_unique<WhileLoop>(whilePos, std::move(condition), std::move(block));
}

std::unique_ptr<RangeExpression> Parser::parseRangeExpression() {
    const auto startPosition = peek().metadata;
    auto       start         = parseExpression();
    expect<DoubleDot>();
    auto end = parseExpression();

    return std::make_unique<RangeExpression>(startPosition, std::move(start), std::move(end));
}

std::unique_ptr<Statement> Parser::parseForLoop() {
    expect<Keyword>();
    const auto forPos     = peekPrevious().metadata;
    auto       identifier = expect<Identifier>();
    expectValue(Keyword::In);
    auto range = parseRangeExpression();

    std::unique_ptr<Expression> step = std::make_unique<IntegerLiteralNode>(peek().metadata, 1);
    if (matchValue(Keyword::Step)) {
        step = parseExpression();
    }

    auto body = parseStatement();

    return std::make_unique<ForLoop>(forPos, identifier.name, std::move(range), std::move(step), std::move(body));
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    expect<Keyword>();
    const auto returnPos = peekPrevious().metadata;

    std::unique_ptr<Expression> value;

    if (!match<Semicolon>())
        value = parseExpression();
    expect<Semicolon>();

    return std::make_unique<ReturnStatement>(returnPos, std::move(value));
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseBooleanOrExpression();
}


std::unique_ptr<Expression> Parser::parseBooleanOrExpression() {
    const auto startPosition = peek().metadata;
    auto       left          = parseBooleanAndExpression();

    while (checkValue(Operator::Or)) {
        auto op    = expect<Operator>();
        auto right = parseBooleanAndExpression();

        left = std::make_unique<BinaryExpression>(startPosition, std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseBooleanAndExpression() {
    const auto startPosition = peek().metadata;
    auto       left          = parseBooleanEqualityExpression();

    while (checkValue(Operator::And)) {
        auto op    = expect<Operator>();
        auto right = parseBooleanEqualityExpression();

        left = std::make_unique<BinaryExpression>(startPosition, std::move(left), op, std::move(right));
    }
    return left;
}


std::unique_ptr<Expression> Parser::parseBooleanEqualityExpression() {
    const auto startPosition = peek().metadata;
    auto       left          = parseComparisonExpression();

    while (checkValue(Operator::Equal) || checkValue(Operator::NotEqual)) {
        auto op    = expect<Operator>();
        auto right = parseComparisonExpression();

        left = std::make_unique<BinaryExpression>(startPosition, std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseComparisonExpression() {
    const auto startPosition = peek().metadata;
    auto       left          = parseAdditiveExpression();

    while (checkValue(Operator::LessThan) || checkValue(Operator::LessEqual)
           || checkValue(Operator::GreaterThan) || checkValue(Operator::GreaterEqual)) {
        auto op    = expect<Operator>();
        auto right = parseAdditiveExpression();

        left = std::make_unique<BinaryExpression>(startPosition, std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseAdditiveExpression() {
    const auto startPosition = peek().metadata;
    auto       left          = parseMultiplicativeExpression();

    while (checkValue(Operator::Plus) || checkValue(Operator::Minus)) {
        auto op    = expect<Operator>();
        auto right = parseMultiplicativeExpression();

        left = std::make_unique<BinaryExpression>(startPosition, std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseMultiplicativeExpression() {
    const auto startPosition = peek().metadata;
    auto       left          = parseUnaryExpression();

    while (checkValue(Operator::Star) || checkValue(Operator::Slash)) {
        auto op    = expect<Operator>();
        auto right = parseUnaryExpression();

        left = std::make_unique<BinaryExpression>(startPosition, std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseUnaryExpression() {
    if (checkValue(Operator::Plus) || checkValue(Operator::Minus)
        || checkValue(Operator::Exclamation)) {
        auto       op      = expect<Operator>();
        const auto opPos   = peekPrevious().metadata;
        auto       operand = parsePrimary();

        return std::make_unique<UnaryExpression>(opPos, std::move(operand), op);
    }
    return parsePrimary();
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (check<IntegerLiteral>()) {
        return std::make_unique<IntegerLiteralNode>(peek().metadata, expect<IntegerLiteral>().value);
    }
    if (check<FloatLiteral>()) {
        return std::make_unique<FloatLiteralNode>(peek().metadata, expect<FloatLiteral>().value);
    }
    if (check<BooleanLiteral>()) {
        return std::make_unique<BooleanLiteralNode>(peek().metadata, expect<BooleanLiteral>().value);
    }
    if (check<StringLiteral>()) {
        return std::make_unique<StringLiteralNode>(peek().metadata, expect<StringLiteral>().value);
    }
    if (check<Identifier>()) {
        if (checkNext<LParen>()) return parseFunctionCall();

        return std::make_unique<IdentifierNode>(peek().metadata, expect<Identifier>().name);
    }
    if (check<LParen>()) {
        expect<LParen>();
        auto expression = parseExpression();
        expect<RParen>();
        return expression;
    }
    error(ParserErrorType::ExpectedExpression, peek());

    throw std::runtime_error("Internal error: unreachable");
}


Token& Parser::peek() {
    if (_position >= _tokens.size()) {
        return _endOfFileToken;
    }
    return _tokens[_position];
}

Token& Parser::peekPrevious() {
    if (_position == 0)
        return _endOfFileToken;
    return _tokens[_position - 1];
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
bool Parser::checkNext() {
    return std::holds_alternative<T>(peekNext().type);
}

template<typename T>
bool Parser::match() {
    const bool match = check<T>();
    if (match) advance();
    return match;
}

template<typename T>
bool Parser::matchValue(T value) {
    const bool match = checkValue(value);
    if (match) advance();
    return match;
}

template<typename T>
bool Parser::checkValue(T value) {
    return check<T>() && std::get<T>(peek().type) == value;
}

template<typename T>
bool Parser::checkNextValue(T value) {
    return checkNext<T>() && std::get<T>(peekNext().type) == value;
}

void Parser::advance() {
    if (!isAtEnd()) _position++;
}

template<typename T>
T Parser::expect() {
    if (check<T>()) {
        T value = std::get<T>(peek().type);
        advance();
        return value;
    }

    // error() throws ParseUnwindException, so this line
    // technically never returns, but we call it to log and throw.
    error(ParserErrorType::UnexpectedToken, peek(), T());

    // Unreachable, but keeps the compiler happy
    throw std::runtime_error("Internal error: unreachable");
}

template<typename T>
T Parser::expectValue(T value) {
    if (!checkValue(value)) {
        error(ParserErrorType::UnexpectedToken, peek(), value);
    }
    T returnValue = std::get<T>(peek().type);
    advance();
    return returnValue;
}

bool Parser::isAtEnd() {
    return check<EndOfFile>();
}

void Parser::synchronize() {
    _isPanicMode = false;

    // If the token that caused the error is already a safe 'anchor',
    // stay there! parseProgram will pick it up on the next loop.
    if (isAtStartOfStatement()) return;

    // Otherwise, we must skip the "bad" token that caused the error
    advance();

    while (!isAtEnd()) {
        // Stop if the PREVIOUS token was a semicolon (end of a statement)
        if (std::holds_alternative<Semicolon>(peekPrevious().type)) return;

        // Stop if the CURRENT token starts a new block/statement
        if (isAtStartOfStatement()) return;

        advance();
    }
}

bool Parser::isAtStartOfStatement() {
    if (check<Keyword>()) {
        switch (std::get<Keyword>(peek().type)) {
            case Keyword::Func:
            case Keyword::Var:
            case Keyword::Const:
            case Keyword::If:
            case Keyword::While:
            case Keyword::Return:
            case Keyword::For:
                return true;
            default:
                return false;
        }
    }
    if (check<LBrace>()) return true;
    return false;
}

void Parser::error(const ParserErrorType& type, const Token& errorToken, const TokenType& expectedTokenType) {
    if (_isPanicMode) {
        std::cout << "There was actually a panic mode so I skipped (testing if this flag is necessary)\n";
        return;
    } // Don't report secondary errors
    _isPanicMode = true;

    _errors.push_back(ParserError(type, errorToken, expectedTokenType));

    // Unwind the stack to the nearest synchronization point
    throw ParseUnwindException();
}
