cmake_minimum_required(VERSION 3.16)
project(compiler-project)

set(CMAKE_CXX_STANDARD 23)

# Add your source files here
add_executable(compiler-project
        src/main.cpp
        src/compiler.cpp
        src/lex/lexer.cpp
        src/parse/AST.h
        src/parse/parser.cpp
        src/parse/parser.h
)

target_include_directories(compiler-project
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
