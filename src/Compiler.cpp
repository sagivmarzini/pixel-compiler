#include "Compiler.h"

#include <format>

#include "semantic/SymbolPool.h"
#include "semantic/SymbolTable.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "IR/IRGeneratorLLVM.h"
#include "parse/Parser.h"
#include "parse/AST/AstPrinter.h"
#include "semantic/DeclarationPassVisitor.h"
#include "semantic/TypeCheckerVisitor.h"

Compiler::Compiler(std::string sourceFile) : _sourceFileName(std::move(sourceFile)) {
    std::ifstream file(_sourceFileName);
    if (!file.is_open()) {
        throw std::invalid_argument(std::format("Failed to open file: '{}'", sourceFile));
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    _sourceCode = buffer.str();

    initFunctions();
}

void Compiler::compile() const {
    // Lexing
    Lexer lexer(_sourceCode);

    const auto tokens = lexer.lex();

    // Parsing
    Parser parser(tokens);
    auto ast = parser.parseProgram();

    AstPrinter printer;

    // Semantic analyzing
    SymbolPool symbols;
    SymbolTable symbolTable(symbols);
    symbolTable.declareBuiltinFunctions(_functionRegistry.getAllApiFunctions());

    DeclarationPassVisitor declPass(symbolTable);
    declPass.run(ast);

    TypeCheckerVisitor typeChecker(symbolTable);
    typeChecker.run(ast);

    IRGeneratorLLVM irGenerator(_functionRegistry);
    irGenerator.visit(ast);
    irGenerator.print();

    irGenerator.createExecutable("../out");
}

void Compiler::printTokens(const std::vector<Token>& tokens) {
    for (const auto& token: tokens) {
        std::cout << token << '\n';
    }
}

void Compiler::initFunctions() {
    // -----------------------------
    // Math & IO API — user-callable
    // -----------------------------
    _functionRegistry.registerApi("printf", // user types `print(...)`
                                  {{{"str", Type::String, true}}, Type::Int, "printf", true});

    _functionRegistry.registerApi("sin",
                                  {{{"angle", Type::Float, true}}, Type::Float, "sinf"});
    _functionRegistry.registerApi("cos",
                                  {{{"angle", Type::Float, true}}, Type::Float, "cosf"});
    _functionRegistry.registerApi("tan",
                                  {{{"angle", Type::Float, true}}, Type::Float, "tanf"});
    _functionRegistry.registerApi("sqrt",
                                  {{{"value", Type::Float, true}}, Type::Float, "sqrtf"});
    _functionRegistry.registerApi("pow",
                                  {{{"base", Type::Float}, {"exponent", Type::Float}}, Type::Float, "powf"});
    _functionRegistry.registerApi("floor",
                                  {{{"value", Type::Float, true}}, Type::Float, "floorf"});

    _functionRegistry.registerApi("abs",
                                  {{{"value", Type::Int, true}}, Type::Int});
    _functionRegistry.registerApi("rand",
                                  {{}, Type::Int});

    // -----------------------------
    // Graphics API — user-callable
    // -----------------------------
    _functionRegistry.registerApi("setCanvas",
                                  {{{"width", Type::Int}, {"height", Type::Int}}, Type::Void});
    _functionRegistry.registerApi("background",
                                  {{{"r", Type::Int}, {"g", Type::Int}, {"b", Type::Int}}, Type::Void});
    _functionRegistry.registerApi("setColor",
                                  {{{"r", Type::Int}, {"g", Type::Int}, {"b", Type::Int}}, Type::Void});

    _functionRegistry.registerApi("rect", // user types `rect(...)`
                                  {
                                      {{"x", Type::Int}, {"y", Type::Int}, {"w", Type::Int}, {"h", Type::Int}},
                                      Type::Void, "pxl_draw_rect"
                                  });
    _functionRegistry.registerApi("rectEmpty", // user types `rectEmpty(...)`
                                  {
                                      {{"x", Type::Int}, {"y", Type::Int}, {"w", Type::Int}, {"h", Type::Int}},
                                      Type::Void, "pxl_draw_empty_rect"
                                  });
    _functionRegistry.registerApi("circle",
                                  {
                                      {{"x", Type::Int}, {"y", Type::Int}, {"r", Type::Int}}, Type::Void,
                                      "pxl_draw_circle"
                                  });
    _functionRegistry.registerApi("circleEmpty",
                                  {
                                      {{"x", Type::Int}, {"y", Type::Int}, {"r", Type::Int}}, Type::Void,
                                      "pxl_draw_empty_circle"
                                  });
    _functionRegistry.registerApi("line",
                                  {
                                      {
                                          {"fromX", Type::Int}, {"fromY", Type::Int}, {"toX", Type::Int},
                                          {"toY", Type::Int}
                                      },
                                      Type::Void, "pxl_draw_line"
                                  });
    _functionRegistry.registerApi("point",
                                  {{{"x", Type::Int}, {"y", Type::Int}}, Type::Void, "pxl_draw_point"});
    _functionRegistry.registerApi("loadImage",
                                  {{{"path", Type::String}}, Type::Pointer, "pxl_load_image"});
    _functionRegistry.registerApi("drawImage",
                                  {
                                      {{"tex", Type::Pointer}, {"x", Type::Int}, {"y", Type::Int}}, Type::Void,
                                      "pxl_draw_image"
                                  });
    // -----------------------------
    // Graphics Lifecycle — internal
    // -----------------------------
    _functionRegistry.registerInternal("init",
                                       {{}, Type::Void, "pxl_init"});

    // Note: pxl_run takes two function pointers as arguments
    _functionRegistry.registerInternal("run",
                                       {
                                           {{"setup", Type::Pointer}, {"draw", Type::Pointer}},
                                           Type::Void, "pxl_run"
                                       });

    _functionRegistry.registerInternal("quit",
                                       {{}, Type::Void, "pxl_quit"});

    // -----------------------------
    // Runtime string helpers — internal
    // -----------------------------
    _functionRegistry.registerInternal("pxl_create_string",
                                       {{{"data", Type::String}, {"size", Type::Int}}, Type::String});
    _functionRegistry.registerInternal("pxl_destroy_string",
                                       {{{"str", Type::String}}, Type::Void});
    _functionRegistry.registerInternal("pxl_copy",
                                       {{{"dest", Type::String}, {"src", Type::String}}, Type::Void});
    _functionRegistry.registerInternal("pxl_get_string_data",
                                       {{{"str", Type::String}}, Type::String});
    _functionRegistry.registerInternal("pxl_concat_string",
                                       {{{"a", Type::String}, {"b", Type::String}}, Type::String});
    _functionRegistry.registerInternal("pxl_char_at",
                                       {{{"str", Type::String}, {"index", Type::Int}}, Type::Int});
    _functionRegistry.registerInternal("pxl_string_equals",
                                       {{{"a", Type::String}, {"b", Type::String}}, Type::Bool});
    _functionRegistry.registerInternal("pxl_string_not_equals",
                                       {{{"a", Type::String}, {"b", Type::String}}, Type::Bool});
    _functionRegistry.registerInternal("pxl_string_greater",
                                       {{{"a", Type::String}, {"b", Type::String}}, Type::Bool});
    _functionRegistry.registerInternal("pxl_string_smaller",
                                       {{{"a", Type::String}, {"b", Type::String}}, Type::Bool});
    _functionRegistry.registerInternal("pxl_string_greater_equals",
                                       {{{"a", Type::String}, {"b", Type::String}}, Type::Bool});
    _functionRegistry.registerInternal("pxl_string_smaller_equals",
                                       {{{"a", Type::String}, {"b", Type::String}}, Type::Bool});
}
