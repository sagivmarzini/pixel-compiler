cmake_minimum_required(VERSION 3.20)  # 3.20+ needed for FetchContent to be rock solid
project(pxl-lang VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Fetch SDL3 (only affects pxl_runtime, invisible to the compiler) ──────────
include(FetchContent)
FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.10    # change deliberately on upgrades
        GIT_SHALLOW TRUE
)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

# ─── Runtime Library ────────────────────────────────────────────────────────────
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)

add_library(pxl_runtime STATIC
        runtime/pxl_string.c
        runtime/pxl_string.h
        runtime/pxl_error.c
        runtime/pxl_error.h
        runtime/pxl_graphics.c
        runtime/pxl_graphics.h
)

target_include_directories(pxl_runtime PUBLIC runtime)

# SDL3 headers for compiling the runtime .c files
target_include_directories(pxl_runtime PRIVATE ${sdl3_SOURCE_DIR}/include)

# Bake SDL3 into pxl_runtime.a — users link one archive, no SDL3 needed
target_link_libraries(pxl_runtime PRIVATE SDL3::SDL3-static)

# Physically merge SDL3's .a into libpxl_runtime.a so it's self-contained
add_custom_command(TARGET pxl_runtime POST_BUILD
        COMMAND ${CMAKE_AR} -x $<TARGET_FILE:SDL3-static>
        COMMAND ${CMAKE_AR} -rcs $<TARGET_FILE:pxl_runtime> *.o
        COMMAND ${CMAKE_COMMAND} -E rm -f *.o
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/libs
        COMMENT "Merging SDL3 into libpxl_runtime.a"
)

# System libs SDL3 needs on Linux (always present, no user action required)
if (UNIX AND NOT APPLE)
    target_link_libraries(pxl_runtime PRIVATE dl pthread)
endif ()

# ─── Compiler Executable ────────────────────────────────────────────────────────
find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(llvm_libs core support native target)

set(COMPILER_SOURCES
        src/main.cpp
        src/Compiler.cpp
        src/Compiler.h
        src/CompilerException.cpp
        src/CompilerException.h
        src/CompilerError.cpp
        src/CompilerError.h
        src/lex/Lexer.cpp
        src/lex/Lexer.h
        src/lex/LexerError.cpp
        src/lex/LexerError.h
        src/lex/Token.cpp
        src/lex/Token.h
        src/parse/Parser.cpp
        src/parse/Parser.h
        src/parse/ParserError.cpp
        src/parse/ParserError.h
        src/parse/AST/AstNode.h
        src/parse/AST/AstVisitor.h
        src/parse/AST/Expression.cpp
        src/parse/AST/Expression.h
        src/parse/AST/Statement.cpp
        src/parse/AST/Statement.h
        src/parse/AST/AstPrinter.cpp
        src/parse/AST/AstPrinter.h
        src/semantic/SemanticVisitor.cpp
        src/semantic/SemanticVisitor.h
        src/semantic/DeclarationPassVisitor.cpp
        src/semantic/DeclarationPassVisitor.h
        src/semantic/TypeCheckerVisitor.cpp
        src/semantic/TypeCheckerVisitor.h
        src/semantic/Symbol.h
        src/semantic/Scope.cpp
        src/semantic/Scope.h
        src/semantic/SymbolPool.cpp
        src/semantic/SymbolPool.h
        src/semantic/SymbolTable.cpp
        src/semantic/SymbolTable.h
        src/semantic/SemanticError.cpp
        src/semantic/SemanticError.h
        src/semantic/functions/FunctionRegistry.cpp
        src/semantic/functions/FunctionRegistry.h
        src/semantic/functions/FunctionInfo.h
        src/IR/IRGeneratorLLVM.cpp
        src/IR/IRGeneratorLLVM.h
        src/semantic/globals/GlobalRegistry.cpp
        src/semantic/globals/GlobalRegistry.h
        src/semantic/globals/GlobalEntry.h
        src/types/TypeContext.h
        src/types/TypeContext.cpp
        src/types/TypeNode.h
        src/types/TypeNode.cpp
)

add_executable(pxl ${COMPILER_SOURCES})
add_dependencies(pxl pxl_runtime)

target_include_directories(pxl PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(pxl PRIVATE
        ${LLVM_DEFINITIONS}
        PXL_RUNTIME_LIB_PATH="${CMAKE_BINARY_DIR}/libs/libpxl_runtime.a"
)

target_link_libraries(pxl PRIVATE ${llvm_libs})