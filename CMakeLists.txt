cmake_minimum_required(VERSION 3.10)
project(cgui C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(cgui_demo example.c)

target_link_libraries(
    cgui_demo 
    glfw
    OpenGL::GL
)

if(WIN32)
    target_compile_definitions(cgui_demo PRIVATE _CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    target_link_libraries(cgui_demo "-framework Cocoa" "-framework IOKit")
elseif(UNIX)
    target_link_libraries(cgui_demo m dl)
endif()

if(MSVC)
    target_compile_options(cgui_demo PRIVATE /W4)
else()
    target_compile_options(cgui_demo PRIVATE -Wall -Wextra -pedantic)
endif()

install(TARGETS cgui_demo DESTINATION bin)
install(FILES cgui.h cgui_backend_gl.h DESTINATION include)

