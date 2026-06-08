# Testing Guide for pixel-compiler

## Overview

This project uses **Catch2 v3** as its testing framework and **CTest** for test discovery and execution. The testing strategy balances comprehensive regression coverage with minimal maintenance burden, appropriate for a small open-source compiler maintained by 1–2 developers.

---

## Architecture

### Test Types

The project includes three complementary test types:

1. **Lexer Unit Tests** (`tests/unit/test_lexer.cpp`)
   - Test tokenization of source code
   - Cover all token types, keywords, literals, and operators
   - Include error cases (unterminated strings, unexpected characters)
   - Verify lowercase error messages (regression for issue bc84dc0)
   - Verify `in` and `step` keywords are recognized (regression for fa30213)

2. **Parser Unit Tests** (`tests/unit/test_parser.cpp`)
   - Test AST construction from token streams
   - Cover variable declarations, function declarations, control flow, expressions
   - Verify operator precedence and associativity
   - Test function declarations inherit from `Statement` (regression for 05f768e)
   - Test `for..in` loop syntax with optional `step` (regression for fa30213)

3. **Compilation Integration Tests** (`tests/programs/valid/` and `tests/programs/invalid/`)
   - Run `pxl <file>` and check exit code (0 for valid, non-zero for invalid)
   - Exercise the full compiler pipeline: Lexer → Parser → Semantic Analysis → IR Generation → Linking
   - Catch regressions in later pipeline stages not covered by unit tests
   - Serve as regression tests for reported bugs (e.g., float `++`/`--` using integer IR in issue #37)

### Test Binary

A single `pxl_tests` executable contains both unit tests. It is built with:
- All lexer/parser source files (Lexer.cpp, Token.cpp, Parser.cpp, etc.)
- Catch2 with `Catch2::Catch2WithMain` target (provides `main()`)
- LLVM libraries (because `TypeNode.h` includes `<llvm/IR/Type.h>`)

The compilation integration tests invoke the `pxl` compiler directly via CTest, so they run the complete pipeline including semantic analysis and IR generation.

---

## Running Tests Locally

### Prerequisites

- **CMake** 3.20+
- **LLVM 18** (or compatible) — install with:
  - macOS: `brew install llvm@18`
  - Ubuntu: follow [apt.llvm.org](https://apt.llvm.org)
- **C++23 compiler** (typically bundled with system compiler)
- **Ninja** or **Make** (recommended: Ninja for faster builds)

### Quick Start

```bash
cd pixel-compiler
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### Available CTest Options

```bash
# Run only unit tests
ctest --test-dir build -R "^[^/]+$"   # Tests without "/" in name (pxl_tests/**)

# Run only compilation tests
ctest --test-dir build -R "^compile/"

# Run a specific test
ctest --test-dir build -R "Lexer"

# Show verbose output
ctest --test-dir build --output-on-failure -V

# Run tests in parallel (faster)
ctest --test-dir build -j $(nproc)
```

---

## Test Organization

```
tests/
├── CMakeLists.txt                         # Test build rules + compilation test registration
├── unit/
│   ├── test_lexer.cpp                     # ~50 Lexer TEST_CASEs
│   └── test_parser.cpp                    # ~40 Parser TEST_CASEs
└── programs/
    ├── valid/                             # Programs that must compile
    │   ├── minimal.pxl                    # Smoke test: bare setup() + draw()
    │   ├── float_incdec.pxl               # Regression: issue #37 (FAdd/FSub)
    │   ├── for_loop_step.pxl              # Regression: fa30213 (in/step keywords)
    │   └── hello_world.pxl                # Variables, types, operators
    └── invalid/                           # Programs that must fail to compile
        ├── missing_semicolon.pxl
        ├── undefined_var.pxl
        └── unexpected_char.pxl
```

---

## Adding a New Test

### Adding a Unit Test (Lexer or Parser)

1. Open `tests/unit/test_lexer.cpp` or `tests/unit/test_parser.cpp`

2. Add a new `TEST_CASE`:
   ```cpp
   TEST_CASE("Parser — for loop edge case", "[parser]") {
       SECTION("step must be positive") {
           // Your test code here
       }
   }
   ```

3. Use `REQUIRE`, `CHECK`, and `REQUIRE_THROWS_AS` assertions:
   ```cpp
   auto tokens = lex("hello");
   REQUIRE(tokens.size() == 2);
   CHECK(std::holds_alternative<Identifier>(tokens[0].type));
   REQUIRE_THROWS_AS(lex("@"), CompilerException);
   ```

4. Rebuild and run:
   ```bash
   cmake --build build
   ctest --test-dir build -R "test name"
   ```

### Adding a Regression Test for a Reported Bug

When a bug is reported and fixed:

1. **Create a `.pxl` test program** that exercises the bug:
   ```bash
   # For a bug that causes compilation to fail:
   touch tests/programs/invalid/my_bug.pxl
   echo "func setup() -> Void { } func draw() -> Void { /* code that triggered bug */ }" > tests/programs/invalid/my_bug.pxl
   
   # For a bug that causes incorrect behavior:
   touch tests/programs/valid/my_bug_fixed.pxl
   echo "func setup() -> Void { } func draw() -> Void { /* code that demonstrated bug */ }" > tests/programs/valid/my_bug_fixed.pxl
   ```

2. **Register it in** `tests/CMakeLists.txt`:
   ```cmake
   pxl_add_compile_test(invalid/my_bug
       ${CMAKE_CURRENT_SOURCE_DIR}/programs/invalid/my_bug.pxl FALSE)
   
   pxl_add_compile_test(valid/my_bug_fixed
       ${CMAKE_CURRENT_SOURCE_DIR}/programs/valid/my_bug_fixed.pxl TRUE)
   ```

3. **Add a unit test** if the bug is in lexer or parser:
   ```cpp
   TEST_CASE("Parser — my bug regression", "[parser]") {
       // Test that verifies the fix
   }
   ```

4. **Document the issue** in a comment:
   ```cpp
   // Regression test for issue #42: parser incorrectly rejected valid syntax
   ```

### Example: Regression Test for Issue #37 (Float ++ used integer IR)

**Problem:** `x++` on a `Float` variable generated integer `CreateAdd` instead of `CreateFAdd`, causing LLVM type errors.

**Test file** (`tests/programs/valid/float_incdec.pxl`):
```pixel
func setup() -> Void {
}

func draw() -> Void {
    var x: Float = 5.0;
    x++;
    x--;
}
```

**CMake registration** (already in `tests/CMakeLists.txt`):
```cmake
pxl_add_compile_test(valid/float_incdec
    ${CMAKE_CURRENT_SOURCE_DIR}/programs/valid/float_incdec.pxl TRUE)
```

**Test execution:** When you run `ctest`, if `pxl float_incdec.pxl` exits non-zero, the test fails and the bug is caught.

---

## GitHub Actions CI

The project runs automated tests on every push to `main` and every pull request.

### Workflow File
- Location: `.github/workflows/ci.yml`
- Runs on: **Ubuntu 22.04**
- Steps:
  1. Install LLVM 18 (from apt.llvm.org, as cmake doesn't find the system LLVM by default)
  2. Install ninja-build
  3. Configure CMake (Release build, Ninja generator)
  4. Build the compiler and tests
  5. Run all tests with `ctest`

### What Happens on Failure

If any test fails (unit test assertion fails, or a valid program doesn't compile), the CI job fails and:
- GitHub marks the PR with a red ✗
- The commit shows test failure in the interface
- New contributors immediately see if their change broke something

### Troubleshooting CI Failures

**If CI fails but tests pass locally:**
- The issue is likely environment-specific (LLVM path, build flags, etc.)
- Check the CI job output in GitHub Actions
- Common causes: different LLVM version, missing dependency

**If a test occasionally fails:**
- This indicates a flaky test (timing-dependent, random seed, etc.)
- Investigate and add explicit assertions to eliminate non-determinism

---

## Limitations and Future Improvements

### Phase 1 Limitations

1. **No binary execution testing** — Every compiled Pixel program opens an SDL3 window via the runtime. We only test that compilation succeeds/fails, not that the binary works correctly. To test binary output, the CI would need Xvfb or similar headless graphics support.

2. **Compile-fail tests are exit-code only** — Invalid programs are checked only for non-zero exit, not for specific error messages. Future improvement: use `PASS_REGULAR_EXPRESSION` on stderr to assert the expected error is emitted.

3. **Compile path is hardcoded** — `IRGeneratorLLVM::createExecutable("../out")` writes to a fixed location. This is fine for Phase 1 but could be refactored to accept a path argument.

### Suggested Phase 2 Improvements

1. **Semantic analysis tests** — Direct testing of `DeclarationPassVisitor`, `TypeCheckerVisitor`, etc., with mock ASTs.

2. **Type checking error message tests** — Assert that specific type errors produce the expected message.

3. **IR generation tests** — Verify that the LLVM IR generated for a given expression is correct (e.g., `x++` on Float generates `CreateFAdd` with ConstantFP(1.0)).

4. **Binary execution tests in CI** — Use Xvfb + frame-counting to verify that compiled programs behave correctly (e.g., the ball in "bouncing ball" demo actually bounces).

5. **Parameterized tests** — Use Catch2 `GENERATE` to test the same logic with multiple inputs, reducing code duplication.

---

## Common Test Patterns

### Testing Lexer Output

```cpp
TEST_CASE("Lexer — tokens", "[lexer]") {
    auto tokens = lex("var x: Int = 5;");
    REQUIRE(tokens.size() == 8);
    REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
    REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::Var);
}
```

### Testing Parser AST Structure

```cpp
TEST_CASE("Parser — function declaration", "[parser]") {
    auto prog = parse("func foo() -> Void {}");
    auto* func = dynamic_cast<AST::FunctionDeclaration*>(prog.statements[0].get());
    REQUIRE(func != nullptr);
    CHECK(func->name == "foo");
}
```

### Testing Error Cases

```cpp
TEST_CASE("Lexer — error: unterminated string", "[lexer]") {
    REQUIRE_THROWS_AS(lex("\"hello"), CompilerException);
}
```

### Testing Compiler Behavior (Integration)

```bash
# Create a .pxl file that should compile
echo "func setup() -> Void {} func draw() -> Void {}" > test.pxl

# Run the compiler
pxl test.pxl

# If exit code is 0, the test passes
```

---

## References

- **Catch2 Documentation:** https://github.com/catchorg/Catch2/tree/devel/docs
- **CTest Documentation:** https://cmake.org/cmake/help/latest/manual/ctest.1.html
- **CMake FetchContent:** https://cmake.org/cmake/help/latest/module/FetchContent.html

---

## Contributing Tests

When you fix a bug or add a feature:

1. **Add a unit test** if the change is in the lexer, parser, or AST.
2. **Add a compilation test** if the change affects the full compiler pipeline.
3. **Document** what the test covers and why (especially for regressions).
4. **Run locally** to ensure the test passes: `ctest --test-dir build --output-on-failure`
5. **Push** — GitHub Actions will verify your tests pass on Ubuntu 22.04.

This ensures the project's correctness improves over time without introducing fragile or duplicated tests.
