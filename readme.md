# Pixel

The first creative coding language that compiles to native binaries via LLVM.

A compiled, statically-typed creative coding language inspired by [p5.js](https://p5js.org)
and [Processing](https://processing.org). Write expressive graphics programs in a clean, purpose-built syntax - Pixel
compiles them to native binaries via LLVM.

```pixel
var t = 0.0;

func setup() -> Void {
    canvas(width: 800, height: 600);
}

func draw() -> Void {
    background(r: 20.0, g: 20.0, b: 30.0);
    t = t + 0.02;

    for i in 0..8 {
        var fi = i * 1.0;
        push();
            translate(x: 400.0, y: 300.0);
            rotate(angle: t + fi * 0.785);
            translate(x: 120.0, y: 0.0);
            fill(r: fi * 28.0, g: 120.0, b: 220.0);
            noStroke();
            circle(x: 0.0, y: 0.0, d: 30.0);
        pop();
    }
}
```

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Dependencies](#dependencies)
- [Building from Source](#building-from-source)
- [Usage](#usage)
- [Language Overview](#language-overview)
- [Graphics API](#graphics-api)
- [Limitations](#limitations)
- [Authors](#authors)
- [License](#license)

---

## Features

- **Compiled to native code** — programs run as standalone binaries with no runtime dependencies on the target machine
- **p5.js-style graphics API** — `setup`/`draw` lifecycle, immediate-mode 2D primitives, fill/stroke state, transform
  stack
- **Static typing with inference** — types are checked at compile time; explicit annotations are optional
- **Named parameters** — all function calls use named arguments for self-documenting code
- **Interactive input** — mouse position, mouse button, and keyboard state available every frame
- **Built-in creative math** — `noise`, `noise2`, `map`, `lerp`, `constrain`, `dist`, `random`, and full trig
- **Cross-platform** — builds and runs on macOS, Linux, and Windows

---

## Architecture

Pixel is a single-pass compiled language with a full compiler pipeline and a separate C runtime library.

```
Source (.pxl)
     │
     ▼
  Lexer
     │  token stream
     ▼
  Parser
     │  AST
     ▼
Semantic Analyzer
     │  typed AST + symbol table
     ▼
LLVM IR Generator
     │  LLVM IR
     ▼
 LLVM Backend
     │  object file
     ▼
  Linker  ◄──── libpxl_runtime.a (compiled from pxl_graphics + SDL3)
     │
     ▼
Native Binary (out)
```

**Compiler** (`/compiler`) — written in C++23. Implements the lexer, parser, semantic analyzer, and LLVM IR code
generator. Built as the `pxl` executable.

**Runtime** (`/runtime`) — written in C. Implements the graphics engine (`pxl_graphics`), the event loop, transform
stack, noise, and all drawing primitives on top of SDL3. Compiled into `libpxl_runtime.a` and statically linked into
every output binary - no shared libraries required at runtime.

---

## Dependencies

| Dependency                                        | Version | Purpose                                |
|---------------------------------------------------|---------|----------------------------------------|
| [LLVM](https://llvm.org)                          | 18      | IR generation and native code emission |
| [SDL3](https://libsdl.org)                        | latest  | Window, renderer, input                |
| [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf) | latest  | Font rendering                         |
| [CMake](https://cmake.org)                        | ≥ 3.20  | Build system                           |
| C++ compiler                                      | C++23   | Building the compiler itself           |

SDL3 and SDL3_ttf are fetched and built automatically via CMake FetchContent — you do not need to install them manually.

---

## Building from Source

### 1. Install prerequisites

You need CMake ≥ 3.20, a C++23-capable compiler, and LLVM 18.

**macOS**

```bash
brew install llvm@18 cmake
```

**Ubuntu / Debian**

```bash
wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && sudo ./llvm.sh 18
sudo apt install cmake build-essential
```

**Windows**

Install [LLVM 18](https://github.com/llvm/llvm-project/releases) and [CMake](https://cmake.org/download/) and ensure
both are on your `PATH`.

### 2. Clone the repository

```bash
git clone https://github.com/sagivmarzini/pixel.git
cd pixel
```

### 3. Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

This builds two things:

- `pxl` — the compiler executable
- `libpxl_runtime.a` — the static runtime library that gets linked into every compiled program

The compiler automatically knows where `libpxl_runtime.a` lives relative to itself after building.

> **CLion users:** open the root `CMakeLists.txt` as a project and build normally. Everything is configured
> automatically.

---

## Usage

### Compile a Pixel program

```bash
pxl <source_file>
```

The `.pxl` extension is optional:

```bash
pxl mysketch.txt    # works
pxl mysketch.pxl    # also works
```

### Run the output

```bash
./out
```

The compiler produces a single self-contained binary called `out`. It has no dependency on the Pixel runtime, SDL3, or
any other library on the machine it runs on — everything is statically linked in at compile time.

### Example

```bash
pxl examples/bouncing_ball
./out
```

---

## Language Overview

### Variables and types

```pixel
var x = 10;               // Int, inferred
var speed = 2.5;          // Float, inferred
var name: String = "Pixel"; // explicit type
const PI = 3.14159;       // constant
```

Primitive types: `Int`, `Float`, `Bool`, `String`, `Void`

### Functions

All parameters are named at the call site:

```pixel
func lerp(start: Float, stop: Float, amt: Float) -> Float {
    return start + (stop - start) * amt;
}

var x = lerp(start: 0.0, stop: 100.0, amt: 0.5);  // 50.0
```

### Control flow

```pixel
if x > 0 {
    // ...
} else {
    // ...
}

for i in 0..10 { }    // inclusive range

while x < 100 {
    x = x + 1;
}
```

### Program structure

Every program declares `setup` (runs once) and `draw` (runs every frame):

```pixel
func setup() -> Void {
    canvas(width: 800, height: 600);
}

func draw() -> Void {
    background(r: 0.0, g: 0.0, b: 0.0);
    circle(x: mouseX, y: mouseY, d: 40.0);
}
```

For full language documentation, see [`DOCS.md`](DOCS.md).

---

## Graphics API

A summary of the built-in graphics functions. All coordinate and color values are `Float`.

| Category     | Functions                                                                                                           |
|--------------|---------------------------------------------------------------------------------------------------------------------|
| Canvas       | `canvas`, `frameRate`, `title`                                                                                      |
| Color        | `background`, `fill`, `fillAlpha`, `noFill`, `stroke`, `strokeAlpha`, `noStroke`, `strokeWeight`                    |
| Primitives   | `rect`, `circle`, `ellipse`, `line`, `triangle`, `point`                                                            |
| Transforms   | `translate`, `rotate`, `scale`, `push`, `pop`                                                                       |
| Math         | `map`, `lerp`, `constrain`, `dist`, `random`, `noise`, `noise2`, `sin`, `cos`, `tan`, `sqrt`, `pow`, `floor`, `abs` |
| Input        | `mouseX`, `mouseY`, `mousePressed`, `keyPressed`, `keyCode`                                                         |
| Typography   | `text`, `textSize`, `textAlign`                                                                                     |
| Loop control | `loop`, `noLoop`                                                                                                    |

---

## Limitations

Pixel 1.0 is a first-generation language with known constraints:

- **Single-file programs** — no import system or multi-file compilation
- **Arrays are fixed-size only** — the size must be a compile-time integer constant; no dynamic or resizable collections
- **No structs or user-defined types** — only the built-in primitives
- **No null / none value**
- **Functions cannot be stored in variables** or passed as first-class values in Pixel code
- **No standard library** — beyond the built-in math and graphics functions, there are no file I/O, networking, or
  string manipulation utilities
- **`for` loop ranges are integers only** — Float ranges require a `while` loop

These are intended starting points for future development, not permanent design decisions.

---

## Authors

**Sagiv Marzini** and **Ofri Kabalo**

Built as a final "Magshimim" project.

---

## License

MIT License — see [`LICENSE`](LICENSE) for details.
