cmake_minimum_required(VERSION 3.16)
project(compiler-project)

set(CMAKE_CXX_STANDARD 23)

# Add your source files here
add_executable(compiler-project
        src/main.cpp
        src/Compiler.cpp
        src/lex/Lexer.cpp
)