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
    _functionRegistry.registerApi("print",
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

    // -----------------------------
    // Graphics — Structure
    // -----------------------------
    _functionRegistry.registerApi("loop",
                                  {{}, Type::Void, "pxl_loop"});
    _functionRegistry.registerApi("noLoop",
                                  {{}, Type::Void, "pxl_no_loop"});

    // -----------------------------
    // Graphics — Canvas setup
    // -----------------------------
    _functionRegistry.registerApi("canvas",
                                  {{{"width", Type::Int}, {"height", Type::Int}}, Type::Void, "pxl_set_canvas_size"});
    _functionRegistry.registerApi("frameRate",
                                  {{{"fps", Type::Int}}, Type::Void, "pxl_set_frames_per_second"});
    _functionRegistry.registerApi("title",
                                  {{{"t", Type::String}}, Type::Void, "pxl_set_window_title"});

    // -----------------------------
    // Graphics — Color / settings
    // -----------------------------
    _functionRegistry.registerApi("background",
                                  {
                                      {{"r", Type::Float}, {"g", Type::Float}, {"b", Type::Float}}, Type::Void,
                                      "pxl_background"
                                  });
    _functionRegistry.registerApi("fill",
                                  {
                                      {{"r", Type::Float}, {"g", Type::Float}, {"b", Type::Float}}, Type::Void,
                                      "pxl_fill"
                                  });
    _functionRegistry.registerApi("fillAlpha",
                                  {
                                      {{"r", Type::Float}, {"g", Type::Float}, {"b", Type::Float}, {"a", Type::Float}},
                                      Type::Void, "pxl_fill_a"
                                  });
    _functionRegistry.registerApi("noFill",
                                  {{}, Type::Void, "pxl_no_fill"});
    _functionRegistry.registerApi("stroke",
                                  {
                                      {{"r", Type::Float}, {"g", Type::Float}, {"b", Type::Float}}, Type::Void,
                                      "pxl_stroke"
                                  });
    _functionRegistry.registerApi("strokeAlpha",
                                  {
                                      {{"r", Type::Float}, {"g", Type::Float}, {"b", Type::Float}, {"a", Type::Float}},
                                      Type::Void, "pxl_stroke_a"
                                  });
    _functionRegistry.registerApi("noStroke",
                                  {{}, Type::Void, "pxl_no_stroke"});
    _functionRegistry.registerApi("strokeWeight",
                                  {{{"weight", Type::Float}}, Type::Void, "pxl_stroke_weight"});
    _functionRegistry.registerApi("colorMode",
                                  {{{"mode", Type::Int}}, Type::Void, "pxl_color_mode"});

    // -----------------------------
    // Graphics — 2D Primitives
    // -----------------------------
    _functionRegistry.registerApi("rect",
                                  {
                                      {{"x", Type::Float}, {"y", Type::Float}, {"w", Type::Float}, {"h", Type::Float}},
                                      Type::Void, "pxl_rect"
                                  });
    _functionRegistry.registerApi("circle",
                                  {
                                      {{"x", Type::Float}, {"y", Type::Float}, {"d", Type::Float}}, Type::Void,
                                      "pxl_circle"
                                  });
    _functionRegistry.registerApi("ellipse",
                                  {
                                      {{"x", Type::Float}, {"y", Type::Float}, {"w", Type::Float}, {"h", Type::Float}},
                                      Type::Void, "pxl_ellipse"
                                  });
    _functionRegistry.registerApi("line",
                                  {
                                      {
                                          {"x1", Type::Float}, {"y1", Type::Float}, {"x2", Type::Float},
                                          {"y2", Type::Float}
                                      },
                                      Type::Void, "pxl_line"
                                  });
    _functionRegistry.registerApi("triangle",
                                  {
                                      {
                                          {"x1", Type::Float}, {"y1", Type::Float}, {"x2", Type::Float},
                                          {"y2", Type::Float}, {"x3", Type::Float}, {"y3", Type::Float}
                                      },
                                      Type::Void, "pxl_triangle"
                                  });
    _functionRegistry.registerApi("point",
                                  {{{"x", Type::Float}, {"y", Type::Float}}, Type::Void, "pxl_point"});

    // -----------------------------
    // Graphics — Transforms
    // -----------------------------
    _functionRegistry.registerApi("translate",
                                  {{{"x", Type::Float}, {"y", Type::Float}}, Type::Void, "pxl_translate"});
    _functionRegistry.registerApi("rotate",
                                  {{{"angle", Type::Float}}, Type::Void, "pxl_rotate"});
    _functionRegistry.registerApi("scale",
                                  {{{"sx", Type::Float}, {"sy", Type::Float}}, Type::Void, "pxl_scale"});
    _functionRegistry.registerApi("push",
                                  {{}, Type::Void, "pxl_push"});
    _functionRegistry.registerApi("pop",
                                  {{}, Type::Void, "pxl_pop"});

    // -----------------------------
    // Graphics — Math utilities
    // -----------------------------
    _functionRegistry.registerApi("map",
                                  {
                                      {
                                          {"value", Type::Float}, {"start1", Type::Float}, {"stop1", Type::Float},
                                          {"start2", Type::Float}, {"stop2", Type::Float}
                                      },
                                      Type::Float, "pxl_map"
                                  });
    _functionRegistry.registerApi("lerp",
                                  {
                                      {{"start", Type::Float}, {"stop", Type::Float}, {"amt", Type::Float}},
                                      Type::Float, "pxl_lerp"
                                  });
    _functionRegistry.registerApi("constrain",
                                  {
                                      {{"n", Type::Float}, {"low", Type::Float}, {"high", Type::Float}}, Type::Float,
                                      "pxl_constrain"
                                  });
    _functionRegistry.registerApi("dist",
                                  {
                                      {
                                          {"x1", Type::Float}, {"y1", Type::Float}, {"x2", Type::Float},
                                          {"y2", Type::Float}
                                      },
                                      Type::Float, "pxl_dist"
                                  });
    _functionRegistry.registerApi("random",
                                  {{{"low", Type::Float}, {"high", Type::Float}}, Type::Float, "pxl_random"});
    _functionRegistry.registerApi("noise",
                                  {{{"x", Type::Float}}, Type::Float, "pxl_noise"});
    _functionRegistry.registerApi("noise2",
                                  {{{"x", Type::Float}, {"y", Type::Float}}, Type::Float, "pxl_noise2"});

    // -----------------------------
    // Graphics — Typography
    // -----------------------------
    _functionRegistry.registerApi("text",
                                  {
                                      {{"str", Type::String}, {"x", Type::Float}, {"y", Type::Float}}, Type::Void,
                                      "pxl_text"
                                  });
    _functionRegistry.registerApi("textSize",
                                  {{{"size", Type::Float}}, Type::Void, "pxl_text_size"});
    _functionRegistry.registerApi("textAlign",
                                  {{{"align", Type::Int}}, Type::Void, "pxl_text_align"});

    // -----------------------------
    // Graphics — Lifecycle (internal)
    // -----------------------------
    _functionRegistry.registerInternal("run",
                                       {{{"setup", Type::Pointer}, {"draw", Type::Pointer}}, Type::Void, "pxl_run"});
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
