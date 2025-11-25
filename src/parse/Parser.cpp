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
    if (matchValue(Keyword::Var)) {
        return parseVariableDeclaration();
    }
    if (matchValue(Keyword::Return)) {
        return parseReturnStatement();
    }
    if (matchValue(Keyword::If)) {
        return parseIfStatement();
    }
    if (matchValue(Keyword::While)) {
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
        if (isAtEnd()) {
            throw std::runtime_error("Missing closing bracket '}'!");
        }
    }
    expect<RBrace>();

    return std::make_unique<Block>(std::move(block));
}

std::vector<FunctionArgument> Parser::parseFunctionArguments() {
    std::vector<FunctionArgument> args;
    while (!match<LParen>()) {
        auto name = expect<Identifier>();
        expect<Colon>();
        auto value = parseExpression();

        args.emplace_back(name.name, std::move(value));

        if (match<RParen>()) {
            expect<Comma>();
        }

        if (isAtEnd()) {
            throw std::runtime_error("Missing closing paren ')'!");
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
        auto paramName = expect<Identifier>();
        expect<Colon>();
        auto paramType = expect<Type>();

        parameters.emplace_back(paramName.name, paramType);
        if (!match<RParen>())
            expect<Comma>(); //if didn't read the end, get a comma seperator [func foo(a:int, b:int)]

        if (isAtEnd()) {
            throw std::runtime_error("Missing closing brace ')'!");
        }
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

    return std::make_unique<VariableDeclaration>(type, name.name, std::move(value));
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

std::unique_ptr<Statement> Parser::parseFunctionCall() {
    auto name = expect<Identifier>();

    expect<LParen>();

    //parse arguments
    auto arguments = parseFunctionArguments();

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

    if (matchValue(Keyword::Else)) {
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

    while (matchValue(Operator::And)) {
        auto op = expect<Operator>();
        auto right = parseBooleanOrExpression();

        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parseBooleanOrExpression() {
    auto left = parseBooleanEqualityExpression();

    while (matchValue(Operator::Or)) {
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
            auto arguments = parseFunctionArguments();
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
    auto position = _position + offset;
    if (position >= _tokens.size()) {
        return _tokens.back(); //return last token if passed the end
    }
    return _tokens[position];
}

template<typename T>
bool Parser::match(int offset) {
    return std::holds_alternative<T>(peek(offset).type);
}

template<typename T>
bool Parser::matchValue(T type) {
    return match<T>() && std::get<T>(peek().type) == type;
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

void Parser::eat() {
    _position++;
}

bool Parser::isAtEnd() {
    return match<EndOfFile>();
}
