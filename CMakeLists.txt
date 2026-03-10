cmake_minimum_required(VERSION 3.16)
project(pxl-lang VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Dependencies ---
#find_package(SDL3 REQUIRED)
find_package(LLVM REQUIRED CONFIG)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)
# --- Runtime Library (The "Standard Library" for your language) ---
# We build this, but we DON'T link it to the compiler executable.
add_library(pxl_runtime STATIC
        runtime/pxl_string.c
        runtime/pxl_utilities.c
        runtime/pxl_string.h
        runtime/pxl_utilities.h
        runtime/pxl_graphics.h
        runtime/pxl_graphics.c
        runtime/pxl_draw.h
        runtime/pxl_draw.c
)
target_include_directories(pxl_runtime PUBLIC runtime)
#target_link_libraries(pxl_runtime PRIVATE SDL3::SDL3)

# --- The Compiler ---
llvm_map_components_to_libnames(llvm_libs core support native target)

# Using a variable for sources keeps add_executable clean
set(COMPILER_SOURCES
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
        src/CompilerError.cpp
        src/CompilerError.h
        src/parse/ParserError.cpp
        src/parse/ParserError.h
        src/semantic/SemanticError.cpp
        src/semantic/SemanticError.h
        src/IR/IRGeneratorLLVM.cpp
        src/IR/IRGeneratorLLVM.h
        src/semantic/FunctionSignature.h
)
add_executable(pxl-compiler ${COMPILER_SOURCES})
add_dependencies(pxl-compiler pxl_runtime)

# Modern scoped configuration
target_include_directories(pxl-compiler PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(pxl-compiler PRIVATE ${LLVM_DEFINITIONS})

# Link LLVM libs
target_link_libraries(pxl-compiler PRIVATE ${llvm_libs})
# Tell the C++ code where the runtime library is located
target_compile_definitions(pxl-compiler PRIVATE
        PXL_RUNTIME_LIB_PATH="${CMAKE_BINARY_DIR}/libs/libpxl_runtime.a"
)
