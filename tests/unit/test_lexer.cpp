#include <catch2/catch_test_macros.hpp>
#include "lex/Lexer.h"
#include "CompilerException.h"

static std::vector<Token> lex(std::string_view src) {
    return Lexer(std::string(src)).lex();
}

TEST_CASE("Lexer — integer literals", "[lexer]") {
    SECTION("single digit") {
        auto tokens = lex("5");
        REQUIRE(tokens.size() == 2);
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[0].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[0].type).value == 5);
    }
    SECTION("multi digit") {
        auto tokens = lex("42");
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[0].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[0].type).value == 42);
    }
    SECTION("zero") {
        auto tokens = lex("0");
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[0].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[0].type).value == 0);
    }
    SECTION("negative not a token") {
        auto tokens = lex("-3");
        REQUIRE(tokens.size() == 3);
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::Minus);
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[1].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[1].type).value == 3);
    }
}

TEST_CASE("Lexer — float literals", "[lexer]") {
    SECTION("basic float") {
        auto tokens = lex("3.14");
        REQUIRE(std::holds_alternative<FloatLiteral>(tokens[0].type));
        float val = std::get<FloatLiteral>(tokens[0].type).value;
        REQUIRE((val > 3.13f && val < 3.15f));
    }
    SECTION("leading zero") {
        auto tokens = lex("0.5");
        REQUIRE(std::holds_alternative<FloatLiteral>(tokens[0].type));
        float val = std::get<FloatLiteral>(tokens[0].type).value;
        REQUIRE((val > 0.49f && val < 0.51f));
    }
    SECTION("range boundary: 1..5 lexes as IntegerLiteral(1), DoubleDot, IntegerLiteral(5)") {
        auto tokens = lex("1..5");
        REQUIRE(tokens.size() == 4);
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[0].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[0].type).value == 1);
        REQUIRE(std::holds_alternative<DoubleDot>(tokens[1].type));
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[2].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[2].type).value == 5);
    }
}

TEST_CASE("Lexer — keywords", "[lexer]") {
    SECTION("func") {
        auto tokens = lex("func");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::Func);
    }
    SECTION("var") {
        auto tokens = lex("var");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::Var);
    }
    SECTION("const") {
        auto tokens = lex("const");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::Const);
    }
    SECTION("return") {
        auto tokens = lex("return");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::Return);
    }
    SECTION("if") {
        auto tokens = lex("if");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::If);
    }
    SECTION("else") {
        auto tokens = lex("else");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::Else);
    }
    SECTION("while") {
        auto tokens = lex("while");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::While);
    }
    SECTION("for") {
        auto tokens = lex("for");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::For);
    }
    SECTION("in (regression: fa30213)") {
        auto tokens = lex("in");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::In);
    }
    SECTION("step (regression: fa30213)") {
        auto tokens = lex("step");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::Step);
    }
}

TEST_CASE("Lexer — type keywords", "[lexer]") {
    SECTION("Int") {
        auto tokens = lex("Int");
        REQUIRE(std::holds_alternative<PrimitiveKind>(tokens[0].type));
        REQUIRE(std::get<PrimitiveKind>(tokens[0].type) == PrimitiveKind::Int);
    }
    SECTION("Float") {
        auto tokens = lex("Float");
        REQUIRE(std::holds_alternative<PrimitiveKind>(tokens[0].type));
        REQUIRE(std::get<PrimitiveKind>(tokens[0].type) == PrimitiveKind::Float);
    }
    SECTION("Bool") {
        auto tokens = lex("Bool");
        REQUIRE(std::holds_alternative<PrimitiveKind>(tokens[0].type));
        REQUIRE(std::get<PrimitiveKind>(tokens[0].type) == PrimitiveKind::Bool);
    }
    SECTION("String") {
        auto tokens = lex("String");
        REQUIRE(std::holds_alternative<PrimitiveKind>(tokens[0].type));
        REQUIRE(std::get<PrimitiveKind>(tokens[0].type) == PrimitiveKind::String);
    }
    SECTION("Void") {
        auto tokens = lex("Void");
        REQUIRE(std::holds_alternative<PrimitiveKind>(tokens[0].type));
        REQUIRE(std::get<PrimitiveKind>(tokens[0].type) == PrimitiveKind::Void);
    }
}

TEST_CASE("Lexer — boolean literals", "[lexer]") {
    SECTION("true") {
        auto tokens = lex("true");
        REQUIRE(std::holds_alternative<BooleanLiteral>(tokens[0].type));
        REQUIRE(std::get<BooleanLiteral>(tokens[0].type).value == true);
    }
    SECTION("false") {
        auto tokens = lex("false");
        REQUIRE(std::holds_alternative<BooleanLiteral>(tokens[0].type));
        REQUIRE(std::get<BooleanLiteral>(tokens[0].type).value == false);
    }
}

TEST_CASE("Lexer — operators", "[lexer]") {
    SECTION("++") {
        auto tokens = lex("++");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::PlusPlus);
    }
    SECTION("--") {
        auto tokens = lex("--");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::MinusMinus);
    }
    SECTION("->") {
        auto tokens = lex("->");
        REQUIRE(std::holds_alternative<Arrow>(tokens[0].type));
    }
    SECTION("..") {
        auto tokens = lex("..");
        REQUIRE(std::holds_alternative<DoubleDot>(tokens[0].type));
    }
    SECTION("==") {
        auto tokens = lex("==");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::Equal);
    }
    SECTION("!=") {
        auto tokens = lex("!=");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::NotEqual);
    }
    SECTION("&&") {
        auto tokens = lex("&&");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::LogicalAnd);
    }
    SECTION("||") {
        auto tokens = lex("||");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::LogicalOr);
    }
    SECTION("+") {
        auto tokens = lex("+");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::Plus);
    }
    SECTION("-") {
        auto tokens = lex("-");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::Minus);
    }
    SECTION("*") {
        auto tokens = lex("*");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::Star);
    }
    SECTION("/") {
        auto tokens = lex("/");
        REQUIRE(std::holds_alternative<Operator>(tokens[0].type));
        REQUIRE(std::get<Operator>(tokens[0].type) == Operator::Slash);
    }
}

TEST_CASE("Lexer — string literals", "[lexer]") {
    SECTION("simple string") {
        auto tokens = lex("\"hello\"");
        REQUIRE(std::holds_alternative<StringLiteral>(tokens[0].type));
        REQUIRE(std::get<StringLiteral>(tokens[0].type).value == "hello");
    }
    SECTION("empty string") {
        auto tokens = lex("\"\"");
        REQUIRE(std::holds_alternative<StringLiteral>(tokens[0].type));
        REQUIRE(std::get<StringLiteral>(tokens[0].type).value == "");
    }
    SECTION("with spaces") {
        auto tokens = lex("\"hello world\"");
        REQUIRE(std::holds_alternative<StringLiteral>(tokens[0].type));
        REQUIRE(std::get<StringLiteral>(tokens[0].type).value == "hello world");
    }
}

TEST_CASE("Lexer — comments are skipped", "[lexer]") {
    SECTION("single line comment") {
        auto tokens = lex("// comment\n42");
        REQUIRE(tokens.size() == 2);
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[0].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[0].type).value == 42);
    }
    SECTION("multi line comment") {
        auto tokens = lex("/* comment */42");
        REQUIRE(tokens.size() == 2);
        REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[0].type));
        REQUIRE(std::get<IntegerLiteral>(tokens[0].type).value == 42);
    }
}

TEST_CASE("Lexer — identifiers", "[lexer]") {
    SECTION("simple name") {
        auto tokens = lex("myVar");
        REQUIRE(std::holds_alternative<Identifier>(tokens[0].type));
        REQUIRE(std::get<Identifier>(tokens[0].type).name == "myVar");
    }
    SECTION("underscore prefix") {
        auto tokens = lex("_x");
        REQUIRE(std::holds_alternative<Identifier>(tokens[0].type));
        REQUIRE(std::get<Identifier>(tokens[0].type).name == "_x");
    }
    SECTION("single underscore is Underscore token, not identifier") {
        auto tokens = lex("_");
        REQUIRE(std::holds_alternative<Underscore>(tokens[0].type));
    }
}

TEST_CASE("Lexer — punctuation", "[lexer]") {
    SECTION("semicolon") {
        auto tokens = lex(";");
        REQUIRE(std::holds_alternative<Semicolon>(tokens[0].type));
    }
    SECTION("colon") {
        auto tokens = lex(":");
        REQUIRE(std::holds_alternative<Colon>(tokens[0].type));
    }
    SECTION("comma") {
        auto tokens = lex(",");
        REQUIRE(std::holds_alternative<Comma>(tokens[0].type));
    }
    SECTION("left brace") {
        auto tokens = lex("{");
        REQUIRE(std::holds_alternative<LeftBrace>(tokens[0].type));
    }
    SECTION("right brace") {
        auto tokens = lex("}");
        REQUIRE(std::holds_alternative<RightBrace>(tokens[0].type));
    }
    SECTION("left paren") {
        auto tokens = lex("(");
        REQUIRE(std::holds_alternative<LeftParen>(tokens[0].type));
    }
    SECTION("right paren") {
        auto tokens = lex(")");
        REQUIRE(std::holds_alternative<RightParen>(tokens[0].type));
    }
    SECTION("left bracket") {
        auto tokens = lex("[");
        REQUIRE(std::holds_alternative<LeftBracket>(tokens[0].type));
    }
    SECTION("right bracket") {
        auto tokens = lex("]");
        REQUIRE(std::holds_alternative<RightBracket>(tokens[0].type));
    }
}

TEST_CASE("Lexer — error: unexpected character", "[lexer]") {
    REQUIRE_THROWS_AS(lex("@"), CompilerException);
    try {
        lex("@");
        FAIL("should have thrown CompilerException");
    } catch (const CompilerException& e) {
        auto msgs = e.errors();
        REQUIRE(!msgs.empty());
        auto msg = msgs[0];
        CHECK(msg.find("error:") != std::string::npos);
        CHECK(msg.find("@") != std::string::npos);
        size_t error_pos = msg.find("error:");
        if (error_pos != std::string::npos) {
            size_t msg_start = error_pos + 6;
            while (msg_start < msg.size() && msg[msg_start] == ' ') {
                msg_start++;
            }
            if (msg_start < msg.size()) {
                CHECK(!isupper(msg[msg_start]));
            }
        }
    }
}

TEST_CASE("Lexer — error: unterminated string", "[lexer]") {
    REQUIRE_THROWS_AS(lex("\"hello"), CompilerException);
}

TEST_CASE("Lexer — error: unterminated block comment", "[lexer]") {
    REQUIRE_THROWS_AS(lex("/* no end"), CompilerException);
}

TEST_CASE("Lexer — token count: full expression", "[lexer]") {
    auto tokens = lex("var x: Int = 5;");
    REQUIRE(tokens.size() == 8);
    REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
    REQUIRE(std::holds_alternative<Identifier>(tokens[1].type));
    REQUIRE(std::holds_alternative<Colon>(tokens[2].type));
    REQUIRE(std::holds_alternative<PrimitiveKind>(tokens[3].type));
    REQUIRE(std::holds_alternative<Operator>(tokens[4].type));
    REQUIRE(std::holds_alternative<IntegerLiteral>(tokens[5].type));
    REQUIRE(std::holds_alternative<Semicolon>(tokens[6].type));
    REQUIRE(std::holds_alternative<EndOfFile>(tokens[7].type));
}
