# 2D Graphics Editor in C

A menu-driven 2D graphics editor written in C that allows you to draw and manage shapes (lines, rectangles, triangles, circles) on a 2D character canvas of $60 \times 20$ pixels. 

The application implements standard shape rasterization algorithms (such as Bresenham's line algorithm and the Midpoint circle algorithm) to render shapes onto a character array using user-selected characters (`*`, `_`, etc.).

---

## Features
- **Vector-Backed Rasterization**: Shapes are stored as a vector list, allowing you to add and delete objects dynamically. The canvas is re-rendered instantly on any modification.
- **Supported Shapes**:
  - **Lines**: Bresenham's line algorithm for arbitrary slopes.
  - **Rectangles**: Four-side border rendering.
  - **Triangles**: Three connected lines.
  - **Circles**: Midpoint Circle algorithm with an optional aspect ratio correction.
- **Aspect Ratio Correction**: A special toggle compensates for standard terminal cells (which are roughly twice as tall as they are wide) by scaling the horizontal radius of circles so they look circular on-screen.
- **Dual Interface Modes**:
  1. **TUI Mode (Interactive Ncurses)**: A beautiful split-screen dashboard layout with real-time canvas updating, scrollable lists, status bar help, and color-coded graphics.
  2. **CLI Mode (Standard text menus)**: A simple fallback console interface prompting for values, displaying canvas borders, and printing coordinates helper lines.
- **Self-Test Mode**: Run automated verification tests to ensure drawing logic and shape state operations work correctly.

---

## Compilation

To compile this project on Windows, you must have MSYS2 installed with UCRT64 `gcc` and `ncurses`.

1. Open PowerShell or Command Prompt.
2. Build the project using `make`:
   ```bash
   make
   ```
   This will generate `editor.exe`.

---

## Running the Application

### 1. Interactive TUI Mode (Default)
Run the executable directly:
```bash
.\editor.exe
```
- Use **Up/Down Arrow keys** or **W/S keys** to navigate the action menu on the right.
- Press **ENTER** or **SPACE** to select an action.
- Input prompts for shape coordinates and characters will appear dynamically in the status bar at the bottom.

### 2. Basic CLI Mode
If your terminal does not support ncurses windows, run in standard command line interface mode:
```bash
.\editor.exe --cli
```
You will be greeted with a classic numbered menu and text prompts.

### 3. Automated Test Suite
To verify the correct operation of shape rendering, list management, and canvas updates:
```bash
.\editor.exe --test
```

---

## File Structure
- `editor.c`: The entire implementation of the graphics logic, interfaces, and testing suite.
- `Makefile`: Script to automate compiling and linking `ncursesw`.
- `README.md`: This documentation file.
