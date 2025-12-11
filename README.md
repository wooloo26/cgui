# CGUI

**A lightweight, immediate-mode GUI library for C17**

CGUI is a single-header, zero-dependency (frontend) immediate-mode GUI library written in pure C17. It provides an intuitive API for building interactive user interfaces with minimal overhead, making it ideal for tools, game development, embedded systems, and prototyping.

## Quick Start

### Prerequisites

- **CMake** 3.10 or higher
- **vcpkg** (for dependency management)
- **C17 compiler** (Visual Studio 2019+, GCC 8+, Clang 6+)

#### Windows (PowerShell)

```powershell
cmake --preset=default
cmake --build build
```

### Running the Demo

```bash
./build/cgui_demo          # Linux/macOS
.\build\cgui_demo.exe      # Windows
```

## Integration into Your Project

CGUI follows the single-header library pattern:

### 1. Copy Headers

```
your_project/
  include/
    cgui.h              # Core GUI library
    cgui_backend_gl.h   # OpenGL 2.1 backend
```

### 2. Define Implementation (in ONE .c file)

```c
#define CGUI_IMPLEMENTATION
#include "cgui.h"

#define CGUI_BACKEND_GL_IMPLEMENTATION
#include "cgui_backend_gl.h"
```

### 3. Include in Other Files

```c
#include "cgui.h"
#include "cgui_backend_gl.h"
```

### 4. Link Dependencies

- **GLFW3**: Window management and input
- **OpenGL**: Graphics rendering (2.1+ compatible)

**CMake example:**

```cmake
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(your_app glfw OpenGL::GL)
```

**Compiler flags:**

- Linux: `-lglfw -lGL -lm`
- macOS: `-lglfw -framework OpenGL -framework Cocoa -framework IOKit`
- Windows: `glfw3.lib opengl32.lib` (MSVC) or `-lglfw3 -lopengl32` (MinGW)

## License

MIT License - see source files for details.
