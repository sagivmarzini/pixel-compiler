cmake_minimum_required(VERSION 3.16)
project(compiler-project)

set(CMAKE_CXX_STANDARD 23)

# Add your source files here
add_executable(compiler-project
        src/main.cpp
        src/Compiler.cpp
        src/lex/Lexer.cpp
        src/parse/Parser.cpp
        src/parse/Parser.h
        src/parse/AST/Expression.h
        src/parse/AST/AstNode.h
        src/parse/AST/Statement.h
        src/parse/AST/Expression.cpp
        src/parse/AST/Statement.cpp
        src/parse/AST/Visitor.cpp
        src/parse/AST/Visitor.h
)

target_include_directories(compiler-project
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
