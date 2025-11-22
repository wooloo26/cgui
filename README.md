# CGUI

## Prerequisites

### Windows

- CMake 3.10 or higher
- Visual Studio 2019 or later (with C++ build tools)
- GLFW3 library

Install GLFW via vcpkg:

```bash
vcpkg install glfw3:x64-windows
```

### macOS

```bash
brew install cmake glfw
```

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install cmake build-essential libglfw3-dev libgl1-mesa-dev
```

## Usage in Your Project

CGUI is a single-header library. To use it:

1. **Copy the headers to your project:**

   - `cgui.h` - Main GUI library
   - `cgui_backend_gl.h` - OpenGL backend (optional)

2. **In ONE .c file, define the implementation:**

   ```c
   #define CGUI_IMPLEMENTATION
   #include "cgui.h"

   #define CGUI_BACKEND_GL_IMPLEMENTATION
   #include "cgui_backend_gl.h"
   ```

3. **In other files, just include normally:**

   ```c
   #include "cgui.h"
   #include "cgui_backend_gl.h"
   ```

4. **Link against GLFW and OpenGL:**
   - GLFW: `-lglfw` or `-lglfw3`
   - OpenGL: `-lGL` (Linux), `-framework OpenGL` (macOS), `-lopengl32` (Windows)
