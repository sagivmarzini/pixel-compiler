# Test Architecture Overview

## Quick Summary

The pixel-compiler now has:
- **Lexer unit tests** — 50+ test cases covering tokenization, keywords, literals, and error messages
- **Parser unit tests** — 40+ test cases covering AST construction, control flow, and expressions
- **Integration tests** — 7 `.pxl` programs (4 valid, 3 invalid) that test the full compiler pipeline
- **GitHub Actions CI** — runs all tests on every push and PR
- **Catch2 v3 framework** — with CTest for discovery and execution

Build and test in 3 commands:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## How It Works

### 1. Lexer Unit Tests (`tests/unit/test_lexer.cpp`)

**Purpose:** Verify the lexer correctly tokenizes source code.

**Key test areas:**
- Integer, float, string, boolean literals
- Keywords: `func`, `var`, `const`, `return`, `if`, `else`, `while`, `for`, `in`, `step` (regression: fa30213)
- Operators: `++`, `--`, `->`, `..`, `==`, `!=`, `&&`, `||`, arithmetic operators
- Error cases: unexpected character (regression: bc84dc0 — must be lowercase), unterminated string, unterminated comment
- Line/column tracking
- Comment skipping (single-line and multi-line)

**Example test:**
```cpp
TEST_CASE("Lexer — keywords", "[lexer]") {
    SECTION("in (regression: fa30213)") {
        auto tokens = lex("in");
        REQUIRE(std::holds_alternative<Keyword>(tokens[0].type));
        REQUIRE(std::get<Keyword>(tokens[0].type) == Keyword::In);
    }
}
```

### 2. Parser Unit Tests (`tests/unit/test_parser.cpp`)

**Purpose:** Verify the parser constructs the correct AST from tokens.

**Key test areas:**
- Variable declarations: typed, inferred, const, arrays
- Function declarations (regression: 05f768e — must inherit Statement)
- For loops with `in` and optional `step` (regression: fa30213)
- If/else chains
- While loops
- Binary expressions with correct precedence (e.g., `*` binds tighter than `+`)
- Increment/decrement: prefix and postfix
- Array operations: indexing, literals
- Function calls: named arguments
- Return statements
- Error recovery: missing semicolons, unexpected tokens, typeless variables

**Example test:**
```cpp
TEST_CASE("Parser — for loop", "[parser]") {
    SECTION("with step keyword (regression: fa30213)") {
        REQUIRE_NOTHROW(parse("func f() -> Void { for i in 0..100 step 5 {} }"));
    }
}
```

### 3. Integration Tests (`.pxl` Programs)

**Purpose:** Test the full compiler pipeline (Lexer → Parser → Semantic Analysis → IR Gen → Link).

**Valid programs** (should exit 0):
- `minimal.pxl` — bare `setup()` and `draw()` (smoke test)
- `float_incdec.pxl` — regression for issue #37 (float `++`/`--` must use `FAdd`/`FSub`, not integer add)
- `for_loop_step.pxl` — regression for fa30213 (`for i in 0..10 step 2` must parse)
- `hello_world.pxl` — exercises variables, types, operators

**Invalid programs** (should exit non-zero):
- `missing_semicolon.pxl` — parser error
- `undefined_var.pxl` — semantic error (type checker error)
- `unexpected_char.pxl` — lexer error

**How they work:** CTest runs `pxl <file>` and checks the exit code. If a "valid" program exits non-zero or an "invalid" program exits 0, the test fails.

---

## Regression Test Examples

### Regression: Issue #37 — Float ++ Used Integer IR

**The bug:** `x++` on a `Float` variable generated LLVM `CreateAdd` (integer addition) instead of `CreateFAdd` (float addition), causing an LLVM type error.

**The fix:** `IRGeneratorLLVM::visit(IncDecExpression)` now checks `oldValue->getType()->isFloatingPointTy()` and uses `CreateFAdd`/`CreateFSub` for floats.

**How we test it:**
- **Unit test:** None explicitly (this is IR generation, tested end-to-end)
- **Integration test:** `tests/programs/valid/float_incdec.pxl` — if this compiles without an LLVM assertion, the fix is working

### Regression: fa30213 — `in` and `step` Keywords Missing

**The bug:** The lexer didn't recognize `in` and `step` as keywords, so they were lexed as identifiers. The parser expected keyword tokens and failed.

**The fix:** Added `in` and `step` to the keyword map in `Token.cpp`.

**How we test it:**
- **Unit test:** `test_lexer.cpp` has sections for `in` and `step` keywords
- **Integration test:** `tests/programs/valid/for_loop_step.pxl` exercises `for i in 0..10 step 2 {}`

### Regression: Issue 05f768e — FunctionDeclaration Not a Statement

**The bug:** `FunctionDeclaration` inherited from `AstNode` instead of `Statement`, so nested function declarations failed to parse.

**The fix:** Changed `FunctionDeclaration` to inherit from `Statement`, allowing it to appear in block statements.

**How we test it:**
- **Unit test:** `test_parser.cpp` verifies `dynamic_cast<AST::Statement*>(func_decl)` succeeds
- **Integration test:** Implicitly tested by `minimal.pxl`, which has two top-level functions

### Regression: bc84dc0 — Lexer Error Messages Were Uppercase

**The bug:** Lexer error messages started with capital letters (`"Unexpected token"`), inconsistent with GCC/Clang style.

**The fix:** Lowercase all lexer error messages.

**How we test it:**
- **Unit test:** `test_lexer.cpp` has a section that catches `CompilerException` and verifies the error message starts with lowercase after `"error: "`

---

## Build System Integration

### Root CMakeLists.txt

At the end of the root `CMakeLists.txt`:
```cmake
enable_testing()
add_subdirectory(tests)
```

This enables CTest and adds the `tests/` subdirectory.

### tests/CMakeLists.txt

Defines two main components:

1. **Unit test binary** (`pxl_tests`):
   - Includes Catch2 via FetchContent
   - Links lexer, parser, and AST source files
   - Requires LLVM because `TypeNode.h` includes `<llvm/IR/Type.h>`
   - Uses `catch_discover_tests()` to register each test case with CTest

2. **Compilation test helper** (`pxl_add_compile_test` function):
   - Creates a CTest entry that runs `pxl <file>`
   - Sets `WILL_FAIL TRUE` for invalid programs

---

## GitHub Actions Workflow

**File:** `.github/workflows/ci.yml`

**Runs on:**
- Every push to `main`
- Every pull request against `main`
- Runs on Ubuntu 22.04

**Steps:**
1. Checkout code
2. Install LLVM 18 (required for the compiler)
3. Install build tools (ninja-build)
4. Configure: `cmake -B build -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm`
5. Build: `cmake --build build --parallel`
6. Test: `ctest --test-dir build --output-on-failure`

**Why the LLVM_DIR flag?** On Ubuntu 22.04, LLVM installed via `llvm-18-dev` goes to `/usr/lib/llvm-18/`, which isn't on CMake's default search path. We have to hint it explicitly.

---

## File Structure

```
pixel-compiler/
├── CMakeLists.txt                  # Root build config — added enable_testing()
├── TESTING.md                      # How to run and write tests (NEW)
├── TEST_ARCHITECTURE.md            # This file (NEW)
├── .github/
│   └── workflows/
│       └── ci.yml                  # GitHub Actions CI config (NEW)
├── src/                            # Compiler source — unchanged
├── runtime/                        # SDL3 runtime — unchanged
└── tests/                          # All tests (NEW)
    ├── CMakeLists.txt              # Test build rules
    ├── unit/
    │   ├── test_lexer.cpp          # 50+ lexer test cases
    │   └── test_parser.cpp         # 40+ parser test cases
    └── programs/
        ├── valid/
        │   ├── minimal.pxl
        │   ├── float_incdec.pxl
        │   ├── for_loop_step.pxl
        │   └── hello_world.pxl
        └── invalid/
            ├── missing_semicolon.pxl
            ├── undefined_var.pxl
            └── unexpected_char.pxl
```

---

## Testing Principles

1. **Unit tests cover lexer and parser** — These are deterministic and fast.
2. **Integration tests cover the full pipeline** — Catch regressions in semantic analysis, IR generation, and linking.
3. **Regression tests are mandatory** — Every bug fix includes a test that would have caught the bug.
4. **Tests are simple and maintainable** — No complex fixtures or elaborate setup; each test is a small, independent case.
5. **CI is transparent** — Every PR shows test results immediately; CI failures block merge.

---

## Maintenance Notes for Contributors

- **Adding a feature?** Add a unit test if it touches the lexer or parser. Add an integration test to exercise the full pipeline.
- **Fixing a bug?** Add a test that reproduces the bug (should fail before the fix, pass after).
- **Refactoring?** Don't change test structure unless the refactoring changes what's being tested.
- **CI fails?** Run `ctest --test-dir build --output-on-failure` locally to debug. The CI uses the same commands.
- **Test is flaky?** Investigate timing issues, randomness, or platform-specific behavior. Make the test deterministic.

---

## Performance Notes

- **Unit tests:** ~100 test cases, run in <1 second total
- **Integration tests:** 7 test cases, ~5-10 seconds total (compiler invocations are the bottleneck)
- **CI build:** ~3-5 minutes total (LLVM installation is the bottleneck)
- **Local development:** `cmake --build build` is incremental; rebuilding after a change is fast

---

## Future Improvements (Beyond Phase 1)

1. **Semantic analyzer unit tests** — Direct testing of type checking, symbol resolution, etc.
2. **Error message tests** — Assert that specific errors produce expected messages (not just non-zero exit).
3. **IR correctness tests** — Verify generated LLVM IR is correct (use LLVM's `verifyModule`).
4. **Binary execution tests** — Use Xvfb to run compiled programs and verify output.
5. **Benchmarks** — Track compiler speed over time (e.g., compilation time for large programs).
