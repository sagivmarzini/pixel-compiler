cmake_minimum_required(VERSION 4.0)
project(compiler_project)

set(CMAKE_CXX_STANDARD 23)

add_executable(compiler_project
        src/main.cpp
        src/Compiler.cpp
        src/Compiler.cpp
        src/Compiler.h

        src/lex/Token.h
        src/lex/Lexer.cpp
        src/lex/Lexer.h

        src/parse/AstNode.h
        src/parse/Visitor.h
        src/parse/ExpressionNodes.h
        src/parse/StatementNodes.h
        src/parse/ExpressionNodes.cpp
        src/parse/StatementNodes.cpp)
