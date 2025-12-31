cmake_minimum_required(VERSION 3.16)
project(compiler-project)

set(CMAKE_CXX_STANDARD 23)

add_executable(compiler-project
        src/main.cpp
        src/Compiler.cpp
        src/lex/Lexer.cpp
        src/CompilerException.cpp
        src/CompilerException.h
        src/lex/LexerError.cpp
        src/lex/LexerError.h
        src/parse/Parser.cpp
        src/parse/Parser.h
        src/parse/AST/Expression.h
        src/parse/AST/AstNode.h
        src/parse/AST/Statement.h
        src/parse/AST/Expression.cpp
        src/parse/AST/Statement.cpp
        src/parse/AST/AstVisitor.h
        src/semantic/SemanticVisitor.cpp
        src/semantic/SemanticVisitor.h
        src/semantic/Symbol.h
        src/semantic/Scope.cpp
        src/semantic/Scope.h
        src/semantic/SymbolPool.cpp
        src/semantic/SymbolPool.h
        src/semantic/DeclarationPassVisitor.cpp
        src/semantic/DeclarationPassVisitor.h
        src/parse/AST/AstPrinter.cpp
        src/parse/AST/AstPrinter.h
        src/lex/Token.cpp
        src/semantic/TypeCheckerVisitor.cpp
        src/semantic/TypeCheckerVisitor.h
        src/semantic/SymbolTable.cpp
        src/semantic/SymbolTable.h
)

target_include_directories(compiler-project
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
