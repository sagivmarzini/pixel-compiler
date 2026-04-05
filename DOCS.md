# Pixel Language Documentation

## Version 1.0

---

# Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Language Basics](#language-basics)
    - [Comments](#comments)
    - [Variables](#variables)
    - [Arrays](#arrays)
    - [Types](#types)
    - [Operators](#operators)
4. [Control Flow](#control-flow)
    - [Conditionals](#conditionals)
    - [For Loops](#for-loops)
    - [While Loops](#while-loops)
5. [Functions](#functions)
6. [The Program Model](#the-program-model)
7. [Graphics API](#graphics-api)
    - [Canvas Setup](#canvas-setup)
    - [Color and Style](#color-and-style)
    - [2D Primitives](#2d-primitives)
    - [Transforms](#transforms)
    - [Math Utilities](#math-utilities)
    - [Input](#input)
    - [Typography](#typography)
8. [Complete Examples](#complete-examples)
9. [Quick Reference](#quick-reference)

---

# Introduction

Pixel is a creative coding language that compiles to native code. It is designed for building interactive visuals and
animations - drawing shapes, reacting to mouse and keyboard input, and animating things over time. If you have used
p5.js or Processing before, the graphics model will feel familiar. If you have not, this documentation will walk you
through everything from scratch.

Pixel programs are written in `.pxl` files. Every Pixel program has two special functions - `setup` and `draw` - that
define what happens when your program starts and what happens every frame.

---

# Getting Started

## Your First Program

Every Pixel program needs at minimum a `setup` function and a `draw` function. Here is the simplest possible program:

```pixel
func setup() -> Void {
}

func draw() -> Void {
    background(r: 0.0, g: 0.0, b: 0.0);
    fill(r: 255.0, g: 100.0, b: 50.0);
    circle(x: 400.0, y: 300.0, d: 100.0);
}
```

This draws an orange circle on a black background, centered on the canvas, every frame.

## Running a Pixel Program

```bash
pixel run myprogram.pxl
```

The canvas defaults to 800×600 pixels if you do not call `canvas()` in `setup`.

---

# Language Basics

## Comments

Single-line comments start with `//`. Multi-line comments use `/* */`.

```pixel
// This is a single-line comment

/*
  This is a multi-line comment.
  It can span as many lines as you need.
*/
```

## Variables

Variables are declared with `var`. The type is inferred automatically from the value, or you can write it explicitly.

```pixel
var x = 100;              // inferred as Int
var speed = 2.5;          // inferred as Float
var name: String = "Bob"; // explicit type
var moving = true;        // inferred as Bool
```

Variables can be reassigned after declaration:

```pixel
var x = 10;
x = 20;
x = x + 5;
```

Constants are declared with `const` and cannot be reassigned:

```pixel
const pi = 3.14159;
const gravity = 9.8;
```

Attempting to reassign a `const` is a compile-time error.

> **Note:** Only declarations are allowed at the global scope. You cannot write statements like `x = x + 1` outside of a
> function body.

## Arrays

Arrays hold a fixed-length sequence of values of the same type.

There are several declaration forms:
```pixel
var nums = [1, 2, 3];            // inferred as Int array, initialized from literal
var flags = [true, false, true]; // Bool array
var coords = [1.0, 2.0, 3.0];   // Float array
var buf[100]: Int;               // 100 Ints, all zeroed
var grid[64]: Float;             // 64 Floats, all zeroed
var filled[10] = 6;              // 10 Ints, all initialized to 6
var weights[8] = 0.5;            // 8 Floats, all initialized to 0.5
```

### Access and mutation

Arrays are zero-indexed. Elements are read and written using bracket notation:
```pixel
var x = nums[0]; // read element at index 0
nums[2] = 99;    // write to index 2
```

Accessing an index out of bounds is a runtime error.

### Iteration

Use a `for` loop with a range to iterate over an array:
```pixel
for i in 0..3 {
    print("%d\n", nums[i]);
}
```

### Constraints

Arrays cannot be resized after declaration.

> **Note:** Arrays cannot be initialized with a value at the global scope. A global array must be declared empty
> (size + type only), then populated inside a function such as `setup()`:
>
> ```pixel
> var globalArr[5]: Int; // global scope — declaration only
>
> fn setup() {
>     globalArr[0] = 1;  // initialization inside a function
>     globalArr[1] = 10;
> }
> ```

## Types

Pixel has six types:

| Type     | Description                                    | Example                 |
|----------|------------------------------------------------|-------------------------|
| `Int`    | Whole number                                   | `42`, `-7`, `0`         |
| `Float`  | Decimal number                                 | `3.14`, `-0.5`, `100.0` |
| `Bool`   | True or false                                  | `true`, `false`         |
| `String` | Text                                           | `"hello"`               |
| `Void`   | No value - only used as a function return type |                         |

### Int vs Float

`Int` and `Float` are distinct. When working with the graphics API, most functions take `Float` arguments. Int gets
promoted automatically to a float because there is no danger of losing precision, but passing a float to an int requires
an explicit cast.

```pixel
var x: Int = 10;
var y: Float = 10.0;  // different type — use this for graphics calls
```

### Strings and print

Pixel uses C-style `printf` formatting for `print`:

```pixel
print("Hello, world!\n");
print("x = %d\n", x);       // Int with %d
print("y = %f\n", y);       // Float with %f
print("name = %s\n", name); // String with %s
```

Strings can be concatenated with `+`:

```pixel
var greeting = "Hello, " + name + "!";
```

## Operators

### Arithmetic

| Operator | Meaning        | Example |
|----------|----------------|---------|
| `+`      | Addition       | `x + y` |
| `-`      | Subtraction    | `x - y` |
| `*`      | Multiplication | `x * y` |
| `/`      | Division       | `x / y` |
| `++`     | Increment by 1 | `x++`   |
| `--`     | Decrement by 1 | `x--`   |

### Comparison

| Operator | Meaning               |
|----------|-----------------------|
| `==`     | Equal to              |
| `!=`     | Not equal to          |
| `<`      | Less than             |
| `<=`     | Less than or equal    |
| `>`      | Greater than          |
| `>=`     | Greater than or equal |

### Logical

| Operator | Meaning | Example              |
|----------|---------|----------------------|
| `&&`     | And     | `x > 0 && y > 0`     |
| `\|\|`   | Or      | `x == 0 \|\| y == 0` |
| `!`      | Not     | `!moving`            |

### Assignment

```pixel
x = 10;       // assign
x = x + 1;   // reassign with expression
```

---

# Control Flow

## Conditionals

```pixel
if x > 100 {
    print("x is large\n");
} else if x > 50 {
    print("x is medium\n");
} else {
    print("x is small\n");
}
```

The condition does not need parentheses. The braces `{ }` are always required, even for single-line bodies.

```pixel
// Single-line bodies still require braces
if moving {
    x = x + speed;
}
```

## For Loops

The `for` loop iterates over an inclusive integer range using `..`:

```pixel
for i in 0..10 {
    print("%d\n", i);  // prints 0, 1, 2, ... 10
}
```

Both ends of the range are inclusive. You can use any integer expressions:

```pixel
var start = 5;
var end = 20;
for i in start..end {
    // i goes from 5 to 20
}
```

You can change the step size with `step`:

```pixel
for i in 0..100 step 5 {
    // i goes from 0 to 100, in steps of 5
}
```

## While Loops

```pixel
var x = 0;
while x < 100 {
    x = x + 1;
}
```

The condition is re-evaluated every iteration. Make sure something inside the loop changes the condition, or it will run
forever.

---

# Functions

Functions are declared with `func`, a name, named parameters, and a return type after `->`:

```pixel
func greet(name: String) -> Void {
    print("Hello, %s!\n", name);
}

func add(a: Int, b: Int) -> Int {
    return a + b;
}

func distance(x1: Float, y1: Float, x2: Float, y2: Float) -> Float {
    var dx = x2 - x1;
    var dy = y2 - y1;
    return sqrt(value: dx * dx + dy * dy);
}
```

You can mark parameters as unnamed with `_` before the parameter name:

```pixel
func add(_ a: Int, _ b: Int) -> Int {
    return a + b;
} 

...

var result = add(3, 4);
```

### Calling Functions

Named parameters are named at the call site:

```pixel
greet(name: "Sagiv");
var result = add(a: 3, b: 4);
circle(x: 400.0, y: 300.0, d: 50.0);
```

This makes calls self-documenting - you always know what each argument means.

### Return Values

Functions that return a value must have a `return` statement on every path. Functions that return `Void` do not need
`return`.

```pixel
func clamp(value: Float, low: Float, high: Float) -> Float {
    if value < low {
        return low;
    }
    if value > high {
        return high;
    }
    return value;
}
```

### Scope

Variables declared inside a function or block are local to that block. Global variables are accessible from any
function.

```pixel
var globalX = 400.0;  // accessible everywhere

func draw() -> Void {
    var localY = 300.0;  // only accessible inside draw
    circle(x: globalX, y: localY, d: 40.0);
}
```

---

# The Program Model

Every Pixel program revolves around two lifecycle functions: `setup` and `draw`.

```pixel
func setup() -> Void {
    // Runs once when the program starts.
    // Use it to configure the canvas, set initial values, etc.
    canvas(width: 800, height: 600);
}

func draw() -> Void {
    // Runs every frame (default: 60 times per second).
    // Everything you draw here appears on screen.
    background(r: 0.0, g: 0.0, b: 0.0);
    circle(x: 400.0, y: 300.0, d: 100.0);
}
```

**Both functions must be declared** in every Pixel program. If you do not call `background()` at the start of `draw`,
shapes from previous frames will accumulate on screen - this is sometimes intentional (trails), usually not.

### Animation

Because `draw` runs every frame, changing a global variable each frame creates animation:

```pixel
var x = 0.0;

func setup() -> Void {
}

func draw() -> Void {
    background(r: 20.0, g: 20.0, b: 20.0);
    circle(x: x, y: 300.0, d: 40.0);
    x = x + 2.0;
    if x > 800.0 {
        x = 0.0;
    }
}
```

### Controlling the Loop

```pixel
noLoop();   // stop calling draw after the current frame
loop();     // resume calling draw
```

`noLoop()` is useful for programs that only need to draw once, or when you want to pause animation.

---

# Graphics API

## Canvas Setup

### `canvas`

Sets the window size. Call this in `setup`. If not called, the default is 800×600.

```pixel
canvas(width: 800, height: 600);
```

### `frameRate`

Sets how many times per second `draw` is called. Default is 60.

```pixel
frameRate(fps: 30);
```

### `title`

Sets the window title bar text.

```pixel
title(t: "My Pixel Sketch");
```

---

## Color and Style

All color values are in the range **0–255** for RGB mode.

### `background`

Clears the entire canvas with a solid color. Call at the start of `draw` to prevent trails.

```pixel
background(r: 0.0, g: 0.0, b: 0.0);        // black
background(r: 255.0, g: 255.0, b: 255.0);  // white
background(r: 30.0, g: 30.0, b: 50.0);     // dark blue-grey
```

### `fill`

Sets the fill color for all subsequent shapes.

```pixel
fill(r: 255.0, g: 100.0, b: 50.0);  // orange
```

### `fillAlpha`

Sets the fill color with an alpha (transparency) value. 0 = fully transparent, 255 = fully opaque.

```pixel
fillAlpha(r: 255.0, g: 100.0, b: 50.0, a: 128.0);  // semi-transparent orange
```

### `noFill`

Disables fill. Shapes will be drawn outline-only.

```pixel
noFill();
```

### `stroke`

Sets the outline color for all subsequent shapes.

```pixel
stroke(r: 255.0, g: 255.0, b: 255.0);  // white outline
```

### `strokeAlpha`

Sets the stroke color with transparency.

```pixel
strokeAlpha(r: 255.0, g: 255.0, b: 255.0, a: 100.0);
```

### `noStroke`

Disables the outline. Shapes will be drawn filled only, with no edge.

```pixel
noStroke();
```

### `strokeWeight`

Sets the thickness of lines and shape outlines in pixels.

```pixel
strokeWeight(weight: 1.0);   // thin
strokeWeight(weight: 5.0);   // thick
```

### Fill and Stroke Together

Every shape respects the current fill and stroke state independently:

```pixel
fill(r: 200.0, g: 100.0, b: 50.0);
stroke(r: 255.0, g: 255.0, b: 255.0);
strokeWeight(weight: 2.0);
rect(x: 100.0, y: 100.0, w: 200.0, h: 100.0);  // filled orange with white border
```

---

## 2D Primitives

### `rect`

Draws a rectangle. `x` and `y` are the **top-left corner**.

```pixel
rect(x: 100.0, y: 100.0, w: 200.0, h: 150.0);
```

### `circle`

Draws a circle. `x` and `y` are the **center**. `d` is the **diameter**.

```pixel
circle(x: 400.0, y: 300.0, d: 100.0);  // diameter 100 = radius 50
```

### `ellipse`

Draws an ellipse. `x` and `y` are the **center**. `w` and `h` are width and height.

```pixel
ellipse(x: 400.0, y: 300.0, w: 200.0, h: 100.0);  // wide ellipse
```

### `line`

Draws a line between two points. Respects `stroke` and `strokeWeight`. Fill has no effect on lines.

```pixel
line(x1: 0.0, y1: 0.0, x2: 800.0, y2: 600.0);  // diagonal across canvas
```

### `triangle`

Draws a triangle defined by three points.

```pixel
triangle(x1: 400.0, y1: 100.0, x2: 200.0, y2: 500.0, x3: 600.0, y3: 500.0);
```

### `point`

Draws a single point. Its size is determined by `strokeWeight`.

```pixel
strokeWeight(weight: 3.0);
stroke(r: 255.0, g: 255.0, b: 0.0);
point(x: 400.0, y: 300.0);
```

---

## Transforms

Transforms change the coordinate system for subsequent drawing calls. They are cumulative — each transform is applied on
top of the current state.

Always use `push` and `pop` to isolate transforms so they do not affect unrelated drawing code.

### `translate`

Moves the origin point by `x` and `y`.

```pixel
translate(x: 400.0, y: 300.0);
circle(x: 0.0, y: 0.0, d: 50.0);  // draws at screen position 400, 300
```

### `rotate`

Rotates the coordinate system by `angle` radians, around the current origin.

```pixel
translate(x: 400.0, y: 300.0);  // rotate around canvas center
rotate(angle: 0.785);            // 45 degrees in radians
rect(x: -50.0, y: -50.0, w: 100.0, h: 100.0);
```

To convert degrees to radians: `radians = degrees * 3.14159 / 180.0`

### `scale`

Scales drawing by `sx` horizontally and `sy` vertically.

```pixel
scale(sx: 2.0, sy: 2.0);  // everything draws at double size
```

### `push` and `pop`

`push` saves the current transform state. `pop` restores it. Everything drawn between `push` and `pop` is affected by
transforms applied inside that block, without affecting anything drawn outside.

```pixel
// Draw 6 rectangles in a ring
for i in 0..5 {
    push();
        translate(x: 400.0, y: 300.0);
        rotate(angle: i * 1.047);       // 1.047 ≈ PI/3 = 60 degrees
        translate(x: 150.0, y: 0.0);
        rect(x: -20.0, y: -20.0, w: 40.0, h: 40.0);
    pop();
}
```

> **Important:** Every `push` must have a matching `pop`. Mismatched push/pop calls will cause a runtime error.

---

## Math Utilities

These functions are built into Pixel. They are the core toolkit for animation and generative work.

### Trigonometry

```pixel
sin(angle: x)   // sine of x (radians)
cos(angle: x)   // cosine of x (radians)
tan(angle: x)   // tangent of x (radians)
```

### General Math

```pixel
sqrt(value: x)              // square root
pow(base: x, exponent: y)   // x to the power of y
floor(value: x)             // round down to nearest Int
abs(value: x)               // absolute value
```

### Creative Coding Math

These are the functions you will reach for constantly:

#### `map`

Re-maps a value from one range to another. Essential for connecting data to visuals.

```pixel
// map mouse X (0..800) to a color component (0..255)
var r = map(value: mouseX, start1: 0.0, stop1: 800.0, start2: 0.0, stop2: 255.0);
```

#### `lerp`

Linearly interpolates between two values by an amount `amt` from 0.0 to 1.0.

```pixel
var x = lerp(start: 0.0, stop: 800.0, amt: 0.5);   // 400.0 — halfway
var x = lerp(start: currentX, stop: targetX, amt: 0.1);  // smooth follow
```

#### `constrain`

Clamps a value to stay within a range.

```pixel
var clamped = constrain(n: value, low: 0.0, high: 255.0);
```

#### `dist`

Returns the distance between two points.

```pixel
var d = dist(x1: x1, y1: y1, x2: x2, y2: y2);
if d < 50.0 {
    // collision!
}
```

#### `random`

Returns a random float between `low` (inclusive) and `high` (exclusive).

```pixel
var x = random(low: 0.0, high: 800.0);
var r = random(low: 100.0, high: 255.0);
```

#### `noise`

Returns smooth pseudo-random noise in the range 0.0–1.0 for a given input. Unlike `random`, nearby inputs return nearby
values, making it ideal for organic animation.

```pixel
var n = noise(x: t);                    // 1D — feed time for smooth animation
var n = noise2(x: col * 0.1, y: row * 0.1);  // 2D — feed grid position for textures
```

A common pattern for organic motion:

```pixel
var t = 0.0;

func draw() -> Void {
    t = t + 0.01;
    var x = map(value: noise(x: t), start1: 0.0, stop1: 1.0, start2: 0.0, stop2: 800.0);
    var y = map(value: noise(x: t + 100.0), start1: 0.0, stop1: 1.0, start2: 0.0, stop2: 600.0);
    circle(x: x, y: y, d: 20.0);
}
```

---

## Input

Input globals are updated automatically every frame. Read them anywhere inside `draw`.

### Mouse Position

```pixel
mouseX   // Float — current mouse X position in pixels
mouseY   // Float — current mouse Y position in pixels
```

```pixel
// Circle follows the mouse
circle(x: mouseX, y: mouseY, d: 40.0);
```

### Mouse Button

```pixel
mousePressed   // Bool — true while any mouse button is held down
```

```pixel
if mousePressed {
    fill(r: 255.0, g: 0.0, b: 0.0);
} else {
    fill(r: 100.0, g: 100.0, b: 100.0);
}
```

### Keyboard

```pixel
keyPressed   // Bool — true for one frame when a key is pressed
keyCode      // Int  — the key code of the last key pressed
```

`keyPressed` is `true` only for a single frame — the frame the key goes down. Use it for one-shot actions. For held-down
detection, track state yourself:

```pixel
var holding = false;

func draw() -> Void {
    if keyPressed {
        holding = true;
    }
    // to detect key-up you would track keyCode changing
}
```

Common key codes (SDL key values):

| Key         | Code       |
|-------------|------------|
| Space       | 32         |
| Enter       | 13         |
| Arrow Up    | 1073741906 |
| Arrow Down  | 1073741905 |
| Arrow Left  | 1073741904 |
| Arrow Right | 1073741903 |

```pixel
if keyPressed {
    if keyCode == 32 {
        // space bar pressed
    }
}
```

---

## Typography

### `text`

Draws a string at position `x`, `y`. Uses the current fill color.

```pixel
fill(r: 255.0, g: 255.0, b: 255.0);
text(str: "Hello, Pixel!", x: 100.0, y: 100.0);
```

### `textSize`

Sets the font size in points.

```pixel
textSize(size: 32.0);
```

### `textAlign`

Sets horizontal alignment. Values: `0` = left, `1` = center, `2` = right.

```pixel
textAlign(align: 1);  // center-align
text(str: "Centered", x: 400.0, y: 300.0);
```

---

# Complete Examples

## Bouncing Ball

```pixel
var x = 400.0;
var y = 300.0;
var vx = 3.0;
var vy = 2.5;
var radius = 30.0;

func setup() -> Void {
    canvas(width: 800, height: 600);
}

func draw() -> Void {
    background(r: 15.0, g: 15.0, b: 25.0);

    x = x + vx;
    y = y + vy;

    if x - radius < 0.0 || x + radius > 800.0 {
        vx = vx * -1.0;
    }
    if y - radius < 0.0 || y + radius > 600.0 {
        vy = vy * -1.0;
    }

    noStroke();
    fill(r: 100.0, g: 180.0, b: 255.0);
    circle(x: x, y: y, d: radius * 2.0);
}
```

## Interactive Paint

```pixel
func setup() -> Void {
    canvas(width: 800, height: 600);
    background(r: 255.0, g: 255.0, b: 255.0);
}

func draw() -> Void {
    // No background() call — drawing accumulates on canvas

    if mousePressed {
        var r = map(value: mouseX, start1: 0.0, stop1: 800.0, start2: 80.0, stop2: 255.0);
        var b = map(value: mouseY, start1: 0.0, stop1: 600.0, start2: 255.0, stop2: 80.0);
        noStroke();
        fill(r: r, g: 100.0, b: b);
        circle(x: mouseX, y: mouseY, d: 20.0);
    }
}
```

## Noise Landscape

```pixel
var t = 0.0;

func setup() -> Void {
    canvas(width: 800, height: 600);
    frameRate(fps: 60);
}

func draw() -> Void {
    background(r: 10.0, g: 8.0, b: 20.0);
    t = t + 0.008;

    noFill();
    strokeWeight(weight: 1.5);

    for i in 0..79 {
        var fi = i * 10.0;
        var n = noise2(x: fi * 0.008, y: t);
        var h = map(value: n, start1: 0.0, stop1: 1.0, start2: 50.0, stop2: 400.0);
        var g = map(value: n, start1: 0.0, stop1: 1.0, start2: 80.0, stop2: 220.0);

        stroke(r: 40.0, g: g, b: 180.0);
        line(x1: fi, y1: 600.0, x2: fi, y2: 600.0 - h);
    }
}
```

## Rotating Shapes with Push/Pop

```pixel
var angle = 0.0;

func setup() -> Void {
    canvas(width: 800, height: 600);
}

func draw() -> Void {
    background(r: 20.0, g: 20.0, b: 20.0);
    angle = angle + 0.02;

    for i in 0..5 {
        var fi = i * 1.0;
        push();
            translate(x: 400.0, y: 300.0);
            rotate(angle: angle + fi * 1.047);
            translate(x: 120.0 + fi * 20.0, y: 0.0);
            rotate(angle: angle * -2.0);

            var r = map(value: fi, start1: 0.0, stop1: 5.0, start2: 100.0, stop2: 255.0);
            fill(r: r, g: 150.0, b: 200.0);
            noStroke();
            rect(x: -20.0, y: -20.0, w: 40.0, h: 40.0);
        pop();
    }
}
```

---

# Quick Reference

## Variables and Types

```pixel
var x = 10;                  // Int
var y = 3.14;                // Float
var name: String = "Pixel";  // String
var flag = true;             // Bool
const PI = 3.14159;          // constant
```

## Control Flow

```pixel
if x > 0 { }
if x > 0 { } else { }
if x > 0 { } else if x == 0 { } else { }

for i in 0..10 { }     // 0 to 10 inclusive
while x < 100 { }
```

## Functions

```pixel
func name(param: Type) -> ReturnType {
    return value;
}
// Call with named parameters:
name(param: value);
```

## Lifecycle

```pixel
func setup() -> Void { }   // runs once
func draw() -> Void { }    // runs every frame
loop();                    // resume draw loop
noLoop();                  // pause draw loop
```

## Canvas

```pixel
canvas(width: 800, height: 600);
frameRate(fps: 60);
title(t: "My Sketch");
```

## Color

```pixel
background(r: 0.0, g: 0.0, b: 0.0);
fill(r: 255.0, g: 0.0, b: 0.0);
fillAlpha(r: 255.0, g: 0.0, b: 0.0, a: 128.0);
noFill();
stroke(r: 255.0, g: 255.0, b: 255.0);
strokeAlpha(r: 255.0, g: 255.0, b: 255.0, a: 100.0);
noStroke();
strokeWeight(weight: 2.0);
```

## Shapes

```pixel
rect(x: x, y: y, w: w, h: h);           // top-left corner
circle(x: x, y: y, d: d);               // center, diameter
ellipse(x: x, y: y, w: w, h: h);        // center, width, height
line(x1: x1, y1: y1, x2: x2, y2: y2);
triangle(x1: x1, y1: y1, x2: x2, y2: y2, x3: x3, y3: y3);
point(x: x, y: y);
```

## Transforms

```pixel
translate(x: x, y: y);
rotate(angle: radians);
scale(sx: sx, sy: sy);
push();   // save transform state
pop();    // restore transform state
```

## Math

```pixel
sin(angle: x)    cos(angle: x)    tan(angle: x)
sqrt(value: x)   pow(base: x, exponent: y)
floor(value: x)  abs(value: x)

map(value: v, start1: a, stop1: b, start2: c, stop2: d)
lerp(start: a, stop: b, amt: t)
constrain(n: v, low: a, high: b)
dist(x1: x1, y1: y1, x2: x2, y2: y2)
random(low: a, high: b)
noise(x: t)
noise2(x: x, y: y)
```

## Input

```pixel
mouseX          // Float — mouse X position
mouseY          // Float — mouse Y position
mousePressed    // Bool  — mouse button held
keyPressed      // Bool  — key just pressed (one frame only)
keyCode         // Int   — last key pressed
```

## Typography

```pixel
text(str: "hello", x: x, y: y);
textSize(size: 24.0);
textAlign(align: 0);   // 0=left, 1=center, 2=right
```

## print

```pixel
print("hello\n");
print("x = %d\n", x);    // Int
print("y = %f\n", y);    // Float
print("s = %s\n", s);    // String
```