cmake_minimum_required(VERSION 3.10)
project(cgui C)

# Set C17 standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Example executable
add_executable(cgui_demo example.c)

# Link libraries
target_link_libraries(cgui_demo 
    glfw
    OpenGL::GL
)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(cgui_demo PRIVATE _CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    # macOS-specific settings
    target_link_libraries(cgui_demo "-framework Cocoa" "-framework IOKit")
elseif(UNIX)
    # Linux-specific settings
    target_link_libraries(cgui_demo m dl)
endif()

# Enable warnings
if(MSVC)
    target_compile_options(cgui_demo PRIVATE /W4)
else()
    target_compile_options(cgui_demo PRIVATE -Wall -Wextra -pedantic)
endif()

# Install targets
install(TARGETS cgui_demo DESTINATION bin)
install(FILES cgui.h cgui_backend_gl.h DESTINATION include)

