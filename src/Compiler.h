#ifndef COMPILER_PROJECT_COMPILER_H
#define COMPILER_PROJECT_COMPILER_H

#include "lex/Lexer.h"
#include "parse/Parser.h"
#include "semantic/FunctionSignature.h"


class Compiler {
public:
    explicit Compiler(std::string sourceFile);

    void compile() const;

    static void printTokens(const std::vector<Token>& tokens);

private:
    std::string                                        _sourceFile;
    std::string                                        _sourceCode;
    std::unordered_map<std::string, FunctionSignature> _builtinFunctions = {
        // IO
        {"printf", {{{"str", Type::String, true}}, Type::Int}},

        // Drawing functions (Return Void)
        {"setCanvas", {{{"width", Type::Int}, {"height", Type::Int}}, Type::Void}},
        {"background", {{{"r", Type::Int}, {"g", Type::Int}, {"b", Type::Int}}, Type::Void}},
        {"setColor", {{{"r", Type::Int}, {"g", Type::Int}, {"b", Type::Int}}, Type::Void}},
        {"rect", {{{"x", Type::Int}, {"y", Type::Int}, {"width", Type::Int}, {"height", Type::Int}}, Type::Void}},
        {"circle", {{{"x", Type::Int}, {"y", Type::Int}, {"radius", Type::Int}}, Type::Void}},

        // Math functions (Return Float/Double)
        {"sin", {{{"angle", Type::Float, true}}, Type::Float}},
        {"cos", {{{"angle", Type::Float, true}}, Type::Float}},
        {"tan", {{{"angle", Type::Float, true}}, Type::Float}},
        {"sqrt", {{{"value", Type::Float, true}}, Type::Float}},
        {"pow", {{{"base", Type::Float}, {"exponent", Type::Float}}, Type::Float}},
        {"floor", {{{"value", Type::Float, true}}, Type::Float}},

        // Int Math
        {"abs", {{{"value", Type::Int, true}}, Type::Int}},
        // TODO: If a call to `rand` was found, inject an srand(time(NULL)) at the start of the program
        {"rand", {{}, Type::Int}}
    };
};


#endif //COMPILER_PROJECT_COMPILER_H
