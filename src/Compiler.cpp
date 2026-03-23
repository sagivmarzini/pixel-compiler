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

Compiler::Compiler(std::string sourceFile)
    : _sourceFileName(std::move(sourceFile)) {
    std::ifstream file(_sourceFileName);
    if (!file.is_open()) {
        throw std::invalid_argument(std::format("Failed to open file: '{}'", sourceFile));
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    _sourceCode = buffer.str();

    initFunctions();
    initGlobals();
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
    symbolTable.declareBuiltinGlobals(_globalRegistry.getAllGlobals());

    DeclarationPassVisitor declPass(symbolTable);
    declPass.run(ast);

    TypeCheckerVisitor typeChecker(symbolTable);
    typeChecker.run(ast);
    printer.print(ast);

    // IRGeneratorLLVM irGenerator(_functionRegistry, _globalRegistry);
    // irGenerator.visit(ast);
    // irGenerator.print();
    //
    // irGenerator.createExecutable("../out");
}

void Compiler::printTokens(const std::vector<Token>& tokens) {
    for (const auto& token: tokens) {
        std::cout << token << '\n';
    }
}

void Compiler::initFunctions() {
    const auto intType = _typeContext.getInt();
    const auto floatType = _typeContext.getFloat();
    const auto stringType = _typeContext.getString();
    const auto voidType = _typeContext.getVoid();
    const auto boolType = _typeContext.getBool();
    const auto pointerType = _typeContext.getPointer();

    // -----------------------------
    // Math & IO API — user-callable
    // -----------------------------
    _functionRegistry.registerApi("print",
                                  {{{"str", stringType, true}}, ScalarKind::Int, "printf", true});

    // Math — LLVM intrinsics (float in, float out)
    _functionRegistry.registerIntrinsic("abs", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::fabs);
    _functionRegistry.registerIntrinsic("sqrt", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::sqrt);
    _functionRegistry.registerIntrinsic(
        "pow", {{{"base", floatType}, {"exponent", floatType}}, ScalarKind::Float},
        llvm::Intrinsic::pow);
    _functionRegistry.registerIntrinsic("sin", {{{"angle", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::sin);
    _functionRegistry.registerIntrinsic("cos", {{{"angle", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::cos);
    _functionRegistry.registerIntrinsic("exp", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::exp);
    _functionRegistry.registerIntrinsic("exp2", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::exp2);
    _functionRegistry.registerIntrinsic("log", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::log);
    _functionRegistry.registerIntrinsic("log2", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::log2);
    _functionRegistry.registerIntrinsic("log10", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::log10);
    _functionRegistry.registerIntrinsic("floor", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::floor);
    _functionRegistry.registerIntrinsic("ceil", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::ceil);
    _functionRegistry.registerIntrinsic("round", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::round);
    _functionRegistry.registerIntrinsic("trunc", {{{"value", floatType, true}}, ScalarKind::Float},
                                        llvm::Intrinsic::trunc);
    _functionRegistry.registerIntrinsic(
        "min", {{{"a", floatType}, {"b", floatType}}, ScalarKind::Float},
        llvm::Intrinsic::minnum);
    _functionRegistry.registerIntrinsic(
        "max", {{{"a", floatType}, {"b", floatType}}, ScalarKind::Float},
        llvm::Intrinsic::maxnum);
    _functionRegistry.registerIntrinsic(
        "copysign", {{{"mag", floatType}, {"sgn", floatType}}, ScalarKind::Float},
        llvm::Intrinsic::copysign);
    _functionRegistry.registerApi("tan",
                                  {
                                      {{"angle", floatType, true}},
                                      ScalarKind::Float, "tanf"
                                  });
    _functionRegistry.registerApi("asin",
                                  {
                                      {{"value", floatType, true}},
                                      ScalarKind::Float, "asinf"
                                  });
    _functionRegistry.registerApi("acos",
                                  {
                                      {{"value", floatType, true}},
                                      ScalarKind::Float, "acosf"
                                  });
    _functionRegistry.registerApi("atan",
                                  {
                                      {{"value", floatType, true}},
                                      ScalarKind::Float, "atanf"
                                  });
    _functionRegistry.registerApi("atan2",
                                  {
                                      {{"y", floatType}, {"x", floatType}},
                                      ScalarKind::Float, "atan2f"
                                  });

    // -----------------------------
    // Graphics — Structure
    // -----------------------------
    _functionRegistry.registerApi("loop",
                                  {{}, ScalarKind::Void, "pxl_loop"});
    _functionRegistry.registerApi("noLoop",
                                  {{}, ScalarKind::Void, "pxl_no_loop"});

    // -----------------------------
    // Graphics — Canvas setup
    // -----------------------------
    _functionRegistry.registerApi("canvas",
                                  {
                                      {
                                          {"width", intType},
                                          {"height", intType}
                                      },
                                      ScalarKind::Void, "pxl_set_canvas_size"
                                  });
    _functionRegistry.registerApi("frameRate",
                                  {
                                      {{"fps", intType}}, ScalarKind::Void,
                                      "pxl_set_frames_per_second"
                                  });
    _functionRegistry.registerApi("title",
                                  {
                                      {{"t", stringType}}, ScalarKind::Void,
                                      "pxl_set_window_title"
                                  });

    // -----------------------------
    // Graphics — Color / settings
    // -----------------------------
    _functionRegistry.registerApi("background",
                                  {
                                      {
                                          {"r", floatType},
                                          {"g", floatType}, {"b", floatType}
                                      },
                                      ScalarKind::Void,
                                      "pxl_background"
                                  });
    _functionRegistry.registerApi("fill",
                                  {
                                      {
                                          {"r", floatType},
                                          {"g", floatType}, {"b", floatType}
                                      },
                                      ScalarKind::Void,
                                      "pxl_fill"
                                  });
    _functionRegistry.registerApi("fillAlpha",
                                  {
                                      {
                                          {"r", floatType},
                                          {"g", floatType},
                                          {"b", floatType}, {"a", floatType}
                                      },
                                      ScalarKind::Void, "pxl_fill_a"
                                  });
    _functionRegistry.registerApi("noFill",
                                  {{}, ScalarKind::Void, "pxl_no_fill"});
    _functionRegistry.registerApi("stroke",
                                  {
                                      {
                                          {"r", floatType},
                                          {"g", floatType}, {"b", floatType}
                                      },
                                      ScalarKind::Void,
                                      "pxl_stroke"
                                  });
    _functionRegistry.registerApi("strokeAlpha",
                                  {
                                      {
                                          {"r", floatType},
                                          {"g", floatType},
                                          {"b", floatType}, {"a", floatType}
                                      },
                                      ScalarKind::Void, "pxl_stroke_a"
                                  });
    _functionRegistry.registerApi("noStroke",
                                  {{}, ScalarKind::Void, "pxl_no_stroke"});
    _functionRegistry.registerApi("strokeWeight",
                                  {
                                      {{"weight", floatType}}, ScalarKind::Void,
                                      "pxl_stroke_weight"
                                  });
    _functionRegistry.registerApi("colorMode",
                                  {
                                      {{"mode", intType}}, ScalarKind::Void,
                                      "pxl_color_mode"
                                  });

    // -----------------------------
    // Graphics — 2D Primitives
    // -----------------------------
    _functionRegistry.registerApi("rect",
                                  {
                                      {
                                          {"x", floatType},
                                          {"y", floatType},
                                          {"w", floatType}, {"h", floatType}
                                      },
                                      ScalarKind::Void, "pxl_rect"
                                  });
    _functionRegistry.registerApi("circle",
                                  {
                                      {
                                          {"x", floatType},
                                          {"y", floatType}, {"d", floatType}
                                      },
                                      ScalarKind::Void,
                                      "pxl_circle"
                                  });
    _functionRegistry.registerApi("ellipse",
                                  {
                                      {
                                          {"x", floatType},
                                          {"y", floatType},
                                          {"w", floatType}, {"h", floatType}
                                      },
                                      ScalarKind::Void, "pxl_ellipse"
                                  });
    _functionRegistry.registerApi("line",
                                  {
                                      {
                                          {"x1", floatType},
                                          {"y1", floatType},
                                          {"x2", floatType},
                                          {"y2", floatType}
                                      },
                                      ScalarKind::Void, "pxl_line"
                                  });
    _functionRegistry.registerApi("triangle",
                                  {
                                      {
                                          {"x1", floatType},
                                          {"y1", floatType},
                                          {"x2", floatType},
                                          {"y2", floatType},
                                          {"x3", floatType},
                                          {"y3", floatType}
                                      },
                                      ScalarKind::Void, "pxl_triangle"
                                  });
    _functionRegistry.registerApi("point",
                                  {
                                      {{"x", floatType}, {"y", floatType}},
                                      ScalarKind::Void, "pxl_point"
                                  });

    // -----------------------------
    // Graphics — Transforms
    // -----------------------------
    _functionRegistry.registerApi("translate",
                                  {
                                      {{"x", floatType}, {"y", floatType}},
                                      ScalarKind::Void, "pxl_translate"
                                  });
    _functionRegistry.registerApi("rotate",
                                  {
                                      {{"angle", floatType}}, ScalarKind::Void,
                                      "pxl_rotate"
                                  });
    _functionRegistry.registerApi("scale",
                                  {
                                      {
                                          {"sx", floatType},
                                          {"sy", floatType}
                                      },
                                      ScalarKind::Void, "pxl_scale"
                                  });
    _functionRegistry.registerApi("push",
                                  {{}, ScalarKind::Void, "pxl_push"});
    _functionRegistry.registerApi("pop",
                                  {{}, ScalarKind::Void, "pxl_pop"});

    // -----------------------------
    // Graphics — Math utilities
    // -----------------------------
    _functionRegistry.registerApi("map",
                                  {
                                      {
                                          {"value", floatType},
                                          {"start1", floatType},
                                          {"stop1", floatType},
                                          {"start2", floatType},
                                          {"stop2", floatType}
                                      },
                                      ScalarKind::Float, "pxl_map"
                                  });
    _functionRegistry.registerApi("lerp",
                                  {
                                      {
                                          {"start", floatType},
                                          {"stop", floatType},
                                          {"amt", floatType}
                                      },
                                      ScalarKind::Float, "pxl_lerp"
                                  });
    _functionRegistry.registerApi("constrain",
                                  {
                                      {
                                          {"n", floatType},
                                          {"low", floatType},
                                          {"high", floatType}
                                      },
                                      ScalarKind::Float,
                                      "pxl_constrain"
                                  });
    _functionRegistry.registerApi("dist",
                                  {
                                      {
                                          {"x1", floatType},
                                          {"y1", floatType},
                                          {"x2", floatType},
                                          {"y2", floatType}
                                      },
                                      ScalarKind::Float, "pxl_dist"
                                  });
    _functionRegistry.registerApi("random",
                                  {
                                      {
                                          {"low", floatType},
                                          {"high", floatType}
                                      },
                                      ScalarKind::Float, "pxl_random"
                                  });
    _functionRegistry.registerApi("noise",
                                  {
                                      {{"x", floatType}}, ScalarKind::Float,
                                      "pxl_noise"
                                  });
    _functionRegistry.registerApi("noise2",
                                  {
                                      {{"x", floatType}, {"y", floatType}},
                                      ScalarKind::Float, "pxl_noise2"
                                  });

    // -----------------------------
    // Graphics — Typography
    // -----------------------------
    _functionRegistry.registerApi("text",
                                  {
                                      {
                                          {"str", stringType},
                                          {"x", floatType}, {"y", floatType}
                                      },
                                      ScalarKind::Void,
                                      "pxl_text"
                                  });
    _functionRegistry.registerApi("textSize",
                                  {
                                      {{"size", floatType}}, ScalarKind::Void,
                                      "pxl_text_size"
                                  });
    _functionRegistry.registerApi("textAlign",
                                  {
                                      {{"align", intType}}, ScalarKind::Void,
                                      "pxl_text_align"
                                  });

    // -----------------------------
    // Graphics — Lifecycle (internal)
    // -----------------------------
    _functionRegistry.registerInternal("run",
                                       {
                                           {
                                               {"setup", pointerType},
                                               {"draw", pointerType}
                                           },
                                           ScalarKind::Void, "pxl_run"
                                       });
    _functionRegistry.registerInternal("quit",
                                       {{}, ScalarKind::Void, "pxl_quit"});

    // -----------------------------
    // Runtime string helpers — internal
    // -----------------------------
    _functionRegistry.registerInternal("pxl_create_string",
                                       {
                                           {
                                               {"data", stringType},
                                               {"size", intType}
                                           },
                                           ScalarKind::String
                                       });
    _functionRegistry.registerInternal("pxl_destroy_string",
                                       {{{"str", stringType}}, ScalarKind::Void});
    _functionRegistry.registerInternal("pxl_copy",
                                       {
                                           {
                                               {"dest", stringType},
                                               {"src", stringType}
                                           },
                                           ScalarKind::Void
                                       });
    _functionRegistry.registerInternal("pxl_get_string_data",
                                       {{{"str", stringType}}, ScalarKind::String});
    _functionRegistry.registerInternal("pxl_concat_string",
                                       {
                                           {
                                               {"a", stringType},
                                               {"b", stringType}
                                           },
                                           ScalarKind::String
                                       });
    _functionRegistry.registerInternal("pxl_char_at",
                                       {
                                           {
                                               {"str", stringType},
                                               {"index", intType}
                                           },
                                           ScalarKind::Int
                                       });
    _functionRegistry.registerInternal("pxl_string_equals",
                                       {
                                           {
                                               {"a", stringType},
                                               {"b", stringType}
                                           },
                                           ScalarKind::Bool
                                       });
    _functionRegistry.registerInternal("pxl_string_not_equals",
                                       {
                                           {
                                               {"a", stringType},
                                               {"b", stringType}
                                           },
                                           ScalarKind::Bool
                                       });
    _functionRegistry.registerInternal("pxl_string_greater",
                                       {
                                           {
                                               {"a", stringType},
                                               {"b", stringType}
                                           },
                                           ScalarKind::Bool
                                       });
    _functionRegistry.registerInternal("pxl_string_smaller",
                                       {
                                           {
                                               {"a", stringType},
                                               {"b", stringType}
                                           },
                                           ScalarKind::Bool
                                       });
    _functionRegistry.registerInternal("pxl_string_greater_equals",
                                       {
                                           {
                                               {"a", stringType},
                                               {"b", stringType}
                                           },
                                           ScalarKind::Bool
                                       });
    _functionRegistry.registerInternal("pxl_string_smaller_equals",
                                       {
                                           {
                                               {"a", stringType},
                                               {"b", stringType}
                                           },
                                           ScalarKind::Bool
                                       });
}


void Compiler::initGlobals() {
    _globalRegistry.registerGlobal("mouseX", ScalarKind::Float, "pxl_mouse_x");
    _globalRegistry.registerGlobal("mouseY", ScalarKind::Float, "pxl_mouse_y");
    _globalRegistry.registerGlobal("mousePressed", ScalarKind::Bool, "pxl_mouse_pressed");
    _globalRegistry.registerGlobal("keyPressed", ScalarKind::Bool, "pxl_key_pressed");
    _globalRegistry.registerGlobal("keyCode", ScalarKind::Int, "pxl_key_code");
}
