cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

# Sources for the unit test binary (no Compiler.cpp / Semantic / IRGenerator)
add_executable(pxl_tests
    unit/test_lexer.cpp
    unit/test_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/lex/Lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/lex/Token.cpp
    ${CMAKE_SOURCE_DIR}/src/lex/LexerError.cpp
    ${CMAKE_SOURCE_DIR}/src/CompilerError.cpp
    ${CMAKE_SOURCE_DIR}/src/CompilerException.cpp
    ${CMAKE_SOURCE_DIR}/src/parse/Parser.cpp
    ${CMAKE_SOURCE_DIR}/src/parse/ParserError.cpp
    ${CMAKE_SOURCE_DIR}/src/parse/AST/Expression.cpp
    ${CMAKE_SOURCE_DIR}/src/parse/AST/Statement.cpp
    ${CMAKE_SOURCE_DIR}/src/parse/AST/AstPrinter.cpp
    ${CMAKE_SOURCE_DIR}/src/types/TypeNode.cpp
    ${CMAKE_SOURCE_DIR}/src/types/TypeContext.cpp
)

target_include_directories(pxl_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${LLVM_INCLUDE_DIRS}
)
target_compile_definitions(pxl_tests PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(pxl_tests PRIVATE Catch2::Catch2WithMain ${llvm_libs})
catch_discover_tests(pxl_tests)

# ─── Compilation integration test helper ─────────────────────────────────────
function(pxl_add_compile_test name source_file expect_success)
    add_test(
        NAME compile/${name}
        COMMAND $<TARGET_FILE:pxl> ${source_file}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    if(NOT expect_success)
        set_tests_properties(compile/${name} PROPERTIES WILL_FAIL TRUE)
    endif()
endfunction()

# Valid programs (should compile cleanly)
pxl_add_compile_test(valid/minimal
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/valid/minimal.pxl TRUE)
pxl_add_compile_test(valid/float_incdec
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/valid/float_incdec.pxl TRUE)
pxl_add_compile_test(valid/for_loop_step
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/valid/for_loop_step.pxl TRUE)
pxl_add_compile_test(valid/hello_world
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/valid/hello_world.pxl TRUE)

# Invalid programs (should fail)
pxl_add_compile_test(invalid/missing_semicolon
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/missing_semicolon.pxl FALSE)
pxl_add_compile_test(invalid/undefined_var
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/undefined_var.pxl FALSE)
pxl_add_compile_test(invalid/unexpected_char
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/unexpected_char.pxl FALSE)

# 80/20 tests: High-impact type checking and semantic errors
pxl_add_compile_test(invalid/type_mismatch
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/type_mismatch.pxl FALSE)
pxl_add_compile_test(invalid/const_reassignment
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/const_reassignment.pxl FALSE)
pxl_add_compile_test(invalid/missing_function_args
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/missing_function_args.pxl FALSE)
pxl_add_compile_test(invalid/negative_array_size
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/negative_array_size.pxl FALSE)
