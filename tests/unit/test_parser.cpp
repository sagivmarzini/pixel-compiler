#include <catch2/catch_test_macros.hpp>
#include "lex/Lexer.h"
#include "parse/Parser.h"
#include "parse/AST/Statement.h"
#include "parse/AST/Expression.h"
#include "types/TypeContext.h"
#include "CompilerException.h"

static AST::Program parse(std::string_view src) {
    auto tokens = Lexer(std::string(src)).lex();
    TypeContext ctx;
    return Parser(std::move(tokens), ctx).parseProgram();
}

TEST_CASE("Parser — variable declaration", "[parser]") {
    SECTION("var with explicit type") {
        auto prog = parse("var x: Int;");
        REQUIRE(prog.statements.size() == 1);
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(prog.statements[0].get());
        REQUIRE(var_decl != nullptr);
        CHECK(var_decl->name == "x");
        CHECK(var_decl->isConst == false);
        CHECK(var_decl->initializer == nullptr);
    }
    SECTION("const with initializer") {
        auto prog = parse("const pi = 3.14;");
        REQUIRE(prog.statements.size() == 1);
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(prog.statements[0].get());
        REQUIRE(var_decl != nullptr);
        CHECK(var_decl->isConst == true);
        REQUIRE(var_decl->initializer != nullptr);
    }
    SECTION("var with inferred type") {
        auto prog = parse("var n = 42;");
        REQUIRE(prog.statements.size() == 1);
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(prog.statements[0].get());
        REQUIRE(var_decl != nullptr);
        CHECK(var_decl->name == "n");
    }
    SECTION("typeless var throws") {
        REQUIRE_THROWS_AS(parse("var x;"), CompilerException);
    }
    SECTION("array declaration") {
        auto prog = parse("var arr[10]: Int;");
        REQUIRE(prog.statements.size() == 1);
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(prog.statements[0].get());
        REQUIRE(var_decl != nullptr);
        CHECK(var_decl->name == "arr");
    }
}

TEST_CASE("Parser — function declaration", "[parser]") {
    SECTION("basic void function") {
        auto prog = parse("func foo() -> Void {}");
        REQUIRE(prog.statements.size() == 1);
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        REQUIRE(func_decl != nullptr);
        CHECK(func_decl->name == "foo");
    }
    SECTION("function with parameters") {
        auto prog = parse("func add(a: Int, b: Int) -> Int { return a; }");
        REQUIRE(prog.statements.size() == 1);
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        REQUIRE(func_decl != nullptr);
        CHECK(func_decl->parameters.size() == 2);
        CHECK(func_decl->parameters[0].name == "a");
        CHECK(func_decl->parameters[1].name == "b");
    }
    SECTION("FunctionDeclaration is a Statement (regression: 05f768e)") {
        auto prog = parse("func foo() -> Void {}");
        auto* stmt = dynamic_cast<AST::Statement*>(prog.statements[0].get());
        REQUIRE(stmt != nullptr);
    }
    SECTION("multiple functions") {
        auto prog = parse("func f1() -> Void {} func f2() -> Void {}");
        REQUIRE(prog.statements.size() == 2);
        auto* f1 = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* f2 = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[1].get());
        REQUIRE(f1 != nullptr);
        REQUIRE(f2 != nullptr);
    }
}

TEST_CASE("Parser — for loop", "[parser]") {
    SECTION("basic range (regression: fa30213)") {
        REQUIRE_NOTHROW(parse("func f() -> Void { for i in 0..10 {} }"));
    }
    SECTION("with step keyword (regression: fa30213)") {
        REQUIRE_NOTHROW(parse("func f() -> Void { for i in 0..100 step 5 {} }"));
    }
    SECTION("for loop is parsed correctly") {
        auto prog = parse("func f() -> Void { for i in 0..10 {} }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        REQUIRE(func_decl != nullptr);
        auto* block = func_decl->body.get();
        REQUIRE(block->statements.size() == 1);
        auto* for_loop = dynamic_cast<AST::ForLoop*>(block->statements[0].get());
        REQUIRE(for_loop != nullptr);
    }
}

TEST_CASE("Parser — if / else", "[parser]") {
    SECTION("simple if") {
        auto prog = parse("func f() -> Void { if true {} }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        REQUIRE(func_decl != nullptr);
        REQUIRE(func_decl->body->statements.size() == 1);
        auto* if_stmt = dynamic_cast<AST::IfStatement*>(func_decl->body->statements[0].get());
        REQUIRE(if_stmt != nullptr);
        CHECK(if_stmt->elseBranch == nullptr);
    }
    SECTION("if-else") {
        auto prog = parse("func f() -> Void { if true {} else {} }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        REQUIRE(func_decl != nullptr);
        auto* if_stmt = dynamic_cast<AST::IfStatement*>(func_decl->body->statements[0].get());
        REQUIRE(if_stmt != nullptr);
        REQUIRE(if_stmt->elseBranch != nullptr);
    }
    SECTION("else-if chain") {
        REQUIRE_NOTHROW(parse("func f() -> Void { if true {} else if false {} else {} }"));
    }
}

TEST_CASE("Parser — while loop", "[parser]") {
    SECTION("basic while") {
        auto prog = parse("func f() -> Void { while true {} }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        REQUIRE(func_decl != nullptr);
        auto* while_loop = dynamic_cast<AST::WhileLoop*>(func_decl->body->statements[0].get());
        REQUIRE(while_loop != nullptr);
    }
}

TEST_CASE("Parser — binary expressions and precedence", "[parser]") {
    SECTION("addition") {
        auto prog = parse("func f() -> Void { var x = 1 + 2; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(func_decl->body->statements[0].get());
        REQUIRE(var_decl != nullptr);
        auto* bin_expr = dynamic_cast<AST::BinaryExpression*>(var_decl->initializer.get());
        REQUIRE(bin_expr != nullptr);
        CHECK(bin_expr->op == Operator::Plus);
    }
    SECTION("multiplication binds tighter than addition") {
        auto prog = parse("func f() -> Void { var x = 1 + 2 * 3; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(func_decl->body->statements[0].get());
        auto* plus_expr = dynamic_cast<AST::BinaryExpression*>(var_decl->initializer.get());
        REQUIRE(plus_expr != nullptr);
        CHECK(plus_expr->op == Operator::Plus);
        auto* mult_expr = dynamic_cast<AST::BinaryExpression*>(plus_expr->right.get());
        REQUIRE(mult_expr != nullptr);
        CHECK(mult_expr->op == Operator::Star);
    }
    SECTION("comparison") {
        auto prog = parse("func f() -> Void { var b = 1 < 2; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(func_decl->body->statements[0].get());
        auto* cmp_expr = dynamic_cast<AST::BinaryExpression*>(var_decl->initializer.get());
        REQUIRE(cmp_expr != nullptr);
        CHECK(cmp_expr->op == Operator::LessThan);
    }
}

TEST_CASE("Parser — increment/decrement expressions", "[parser]") {
    SECTION("postfix ++") {
        auto prog = parse("func f() -> Void { x++; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* expr_stmt = dynamic_cast<AST::ExpressionStatement*>(func_decl->body->statements[0].get());
        REQUIRE(expr_stmt != nullptr);
        auto* inc_dec = dynamic_cast<AST::IncDecExpression*>(expr_stmt->expression.get());
        REQUIRE(inc_dec != nullptr);
        CHECK(inc_dec->fix == AST::IncDecExpression::Fix::Postfix);
        CHECK(inc_dec->op == Operator::PlusPlus);
    }
    SECTION("prefix --") {
        auto prog = parse("func f() -> Void { --x; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* expr_stmt = dynamic_cast<AST::ExpressionStatement*>(func_decl->body->statements[0].get());
        REQUIRE(expr_stmt != nullptr);
        auto* inc_dec = dynamic_cast<AST::IncDecExpression*>(expr_stmt->expression.get());
        REQUIRE(inc_dec != nullptr);
        CHECK(inc_dec->fix == AST::IncDecExpression::Fix::Prefix);
    }
}

TEST_CASE("Parser — return statements", "[parser]") {
    SECTION("return with value") {
        auto prog = parse("func f() -> Int { return 42; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* ret_stmt = dynamic_cast<AST::ReturnStatement*>(func_decl->body->statements[0].get());
        REQUIRE(ret_stmt != nullptr);
        REQUIRE(ret_stmt->value != nullptr);
    }
    SECTION("return without value") {
        auto prog = parse("func f() -> Void { return; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* ret_stmt = dynamic_cast<AST::ReturnStatement*>(func_decl->body->statements[0].get());
        REQUIRE(ret_stmt != nullptr);
        CHECK(ret_stmt->value == nullptr);
    }
}

TEST_CASE("Parser — array operations", "[parser]") {
    SECTION("array index") {
        auto prog = parse("func f() -> Void { var x = arr[0]; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(func_decl->body->statements[0].get());
        auto* arr_index = dynamic_cast<AST::ArrayIndex*>(var_decl->initializer.get());
        REQUIRE(arr_index != nullptr);
    }
    SECTION("array literal") {
        auto prog = parse("func f() -> Void { var xs = [1, 2, 3]; }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(func_decl->body->statements[0].get());
        auto* arr_lit = dynamic_cast<AST::ArrayLiteral*>(var_decl->initializer.get());
        REQUIRE(arr_lit != nullptr);
        CHECK(arr_lit->elements.size() == 3);
    }
}

TEST_CASE("Parser — function calls", "[parser]") {
    SECTION("function call as expression") {
        auto prog = parse("func f() -> Void { var x = foo(a: 1); }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(func_decl->body->statements[0].get());
        auto* call = dynamic_cast<AST::FunctionCall*>(var_decl->initializer.get());
        REQUIRE(call != nullptr);
        CHECK(call->name == "foo");
    }
    SECTION("function call with named arguments") {
        auto prog = parse("func f() -> Void { g(x: 1, y: 2); }");
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
        auto* expr_stmt = dynamic_cast<AST::ExpressionStatement*>(func_decl->body->statements[0].get());
        REQUIRE(expr_stmt != nullptr);
        auto* call = dynamic_cast<AST::FunctionCall*>(expr_stmt->expression.get());
        REQUIRE(call != nullptr);
        CHECK(call->arguments.size() == 2);
    }
}

TEST_CASE("Parser — error recovery", "[parser]") {
    SECTION("missing semicolon") {
        REQUIRE_THROWS_AS(parse("func f() -> Void { var x: Int }"), CompilerException);
    }
    SECTION("unexpected closing brace") {
        REQUIRE_THROWS_AS(parse("func f() -> Void { } }"), CompilerException);
    }
    SECTION("typeless variable in function") {
        REQUIRE_THROWS_AS(parse("func f() -> Void { var x; }"), CompilerException);
    }
}

TEST_CASE("Parser — global scope", "[parser]") {
    SECTION("global variable declaration") {
        auto prog = parse("var x: Int = 0; func f() -> Void {}");
        REQUIRE(prog.statements.size() == 2);
        auto* var_decl = dynamic_cast<AST::VariableDeclaration*>(prog.statements[0].get());
        REQUIRE(var_decl != nullptr);
        auto* func_decl = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[1].get());
        REQUIRE(func_decl != nullptr);
    }
    SECTION("multiple global variables") {
        auto prog = parse("var x: Int = 0; var y: Float = 1.0; func f() -> Void {}");
        REQUIRE(prog.statements.size() == 3);
    }
}
