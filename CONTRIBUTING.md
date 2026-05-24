# Contributing to Pixel

Welcome - and thank you for your interest in contributing to Pixel, a compiled creative-coding language that targets native binaries via LLVM.

---

## Table of Contents

- [Building from Source](#building-from-source)
- [How the Compiler Pipeline Works](#how-the-compiler-pipeline-works)
- [Coding Conventions](#coding-conventions)
- [Picking Up a Good First Issue](#picking-up-a-good-first-issue)
- [Submitting a Pull Request](#submitting-a-pull-request)

---

## Building from Source

### Prerequisites

| Tool         | Version              |
| ------------ | -------------------- |
| CMake        | ≥ 3.20               |
| C++ compiler | C++23 (clang or gcc) |
| LLVM         | 18                   |

SDL3 and SDL3_ttf are fetched automatically by CMake - you do not need to install them.

**macOS**

```bash
brew install llvm@18 cmake
```

**Ubuntu / Debian**

```bash
wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 18
sudo apt install cmake build-essential
```

### Build

```bash
git clone https://github.com/sagivmarzini/pixel-compiler.git
cd pixel-compiler
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

This produces two artifacts inside `build/`:

- `pxl` - the compiler executable
- `libs/libpxl_runtime.a` - the static runtime that is linked into every compiled program

### Run a Pixel program

```bash
./build/pxl source.pxl
./out
```

### CLion

Open the root `CMakeLists.txt` as a project. Everything is pre-configured.

---

## How the Compiler Pipeline Works

Pixel is a single-pass, ahead-of-time compiler. A `.pxl` source file goes through five stages before it becomes a native binary.

```
Source (.pxl)
     │
     ▼
  Lexer                  src/lex/
     │  token stream
     ▼
  Parser                 src/parse/
     │  AST
     ▼
Semantic Analyzer        src/semantic/
     │  typed AST + symbol table
     ▼
LLVM IR Generator        src/IR/
     │  LLVM IR
     ▼
 LLVM Backend + Linker
     │  object file → native binary
     ▼
Native Binary (out)  ◄── libpxl_runtime.a  runtime/
```

### Lexer - `src/lex/`

`Lexer.cpp` converts the raw source text into a flat `std::vector<Token>`. Each `Token` holds a `TokenType` variant (see `Token.h`) and a `TokenMetadata` struct with line, column, and original lexeme. The keyword map at the bottom of `Token.h` drives keyword recognition.

To add a new keyword or operator: add an entry to `Token.h` (the `Keyword` enum or `Operator` enum, plus the keyword map), then update `Lexer.cpp` to emit it.

### Parser - `src/parse/`

`Parser.cpp` is a hand-written recursive-descent parser. It consumes the token stream and produces an AST composed of nodes from `src/parse/AST/`. Statements live in `Statement.h`; expressions in `Expression.h`. Every node implements two virtual methods: `accept(AstVisitor&)` for semantic passes and `acceptIR(IRGeneratorLLVM&)` for code generation.

`TypeContext` (`src/types/`) is the authoritative allocator for `TypeNode` objects; the parser calls into it to create and intern type nodes so the same logical type is always the same pointer.

To add a new statement or expression:

1. Define the AST node in `Statement.h` / `Expression.h`
2. Implement the `accept` and `acceptIR` method bodies in the corresponding `.cpp`
3. Add the production rule in `Parser.cpp`
4. Add visitor overloads in `AstVisitor.h` and `AstPrinter.cpp`

### Semantic Analyzer - `src/semantic/`

The semantic phase runs two visitors over the AST in order:

1. **`DeclarationPassVisitor`** - a first pass that registers all top-level function and global declarations into the symbol table before any type checking. This allows functions to call each other regardless of declaration order.
2. **`TypeCheckerVisitor`** - a full tree walk that resolves types, checks assignments, validates function calls, and annotates every expression node with its `TypeNode*`.

Built-in functions are registered in `FunctionRegistry.cpp`. Built-in global variables (like `mouseX`, `mouseY`, `keyCode`) are registered in `GlobalRegistry.cpp`. Adding a new built-in means adding an entry to both the registry and the corresponding runtime implementation in `runtime/pxl_graphics.c`.

`SymbolTable` and `Scope` manage lexical scoping. Each `Block` AST node owns a `Scope*`; scopes form a parent chain.

### IR Generator - `src/IR/IRGeneratorLLVM.cpp`

`IRGeneratorLLVM` implements `AstVisitor` and walks the typed AST to emit LLVM IR via `llvm::IRBuilder`. It holds a `_namedValues` map from `Symbol*` to `llvm::AllocaInst*` for local variables. Global variables are emitted as `llvm::GlobalVariable`.

All calling conventions follow the `FunctionInfo` records from `FunctionRegistry`. Built-in functions are declared as external symbols; their definitions live in the C runtime.

`castToType()` handles all safe numeric promotions (Int → Float). Adding a new built-in type would require a new `toLLVMType()` implementation in `TypeNode`.

### Runtime - `runtime/`

Written in C. `pxl_graphics.c` implements the event loop, the SDL3 renderer, transform stack, all drawing primitives, input polling, and the `noise`/`noise2` functions. It is compiled into `libpxl_runtime.a` and statically linked into every `out` binary - the user's machine needs no runtime dependencies.

---

## Coding Conventions

- **C++23** throughout the compiler. Use `std::format`, ranges, and structured bindings freely.
- **No raw owning pointers in the AST.** Use `std::unique_ptr` for child nodes. Non-owning pointers (e.g. `Symbol*`, `TypeNode*`) are fine where the owner is clear.
- **Visitor pattern** for all AST traversals. New passes implement `AstVisitor`.
- **Error reporting.** Use `logError(ErrorType, node)` rather than throwing or printing directly. The compiler collects all errors and reports them after the pass completes.
- **No comments describing what the code does.** Name things well instead. Add a comment only when a non-obvious constraint or workaround is present.
- **Runtime is C, not C++.** Keep `runtime/` as plain C99 - no C++ constructs.

---

## Picking Up a Good First Issue

1. Browse issues labelled [`good first issue`](https://github.com/sagivmarzini/pixel-compiler/issues?q=is%3Aopen+label%3A%22good+first+issue%22).
2. Leave a comment on the issue so others know you are working on it.
3. Each issue body contains a **Where** section naming which pipeline stage(s) to touch, and an **Acceptance test** - a `.pxl` snippet that must compile and produce the correct output once the feature is done.

---

## Submitting a Pull Request

1. Fork the repository and create a feature branch: `git checkout -b feat/my-feature`
2. Make your changes following the coding conventions above.
3. Verify the build passes: `cmake --build build`
4. Write or update a `.pxl` test sketch that exercises your change.
5. Open a pull request against `main`. The PR description should explain:
   - What changed and why
   - Which pipeline stage(s) were modified
   - How to test it manually
6. Link the related issue with `Closes #N` in the PR body.

---

Thank you for contributing to Pixel.
