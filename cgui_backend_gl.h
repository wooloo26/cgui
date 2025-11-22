/*
 * CGUI - OpenGL 2.1 Backend Implementation
 * Handles rendering of draw commands using legacy OpenGL
 * 
 * Usage:
 *   #define CGUI_BACKEND_GL_IMPLEMENTATION
 *   #include "cgui_backend_gl.h"
 */

#ifndef CGUI_BACKEND_GL_H
#define CGUI_BACKEND_GL_H

#include "cgui.h"

#ifdef __cplusplus
extern "C" {
#endif

// Backend state
typedef struct {
    unsigned int vbo;
    unsigned int ebo;
    unsigned int shader_program;
    int attrib_pos;
    int attrib_uv;
    int attrib_color;
    int uniform_projection;
    float display_width;
    float display_height;
} gui_backend_gl_t;

// Initialize OpenGL backend
void gui_backend_gl_init(gui_backend_gl_t* backend);

// Shutdown OpenGL backend
void gui_backend_gl_shutdown(gui_backend_gl_t* backend);

// Render the GUI
void gui_backend_gl_render(gui_backend_gl_t* backend, gui_context_t* ctx);

#ifdef __cplusplus
}
#endif

#endif // CGUI_BACKEND_GL_H

// =============================================================================
// IMPLEMENTATION
// =============================================================================

#ifdef CGUI_BACKEND_GL_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

// OpenGL headers (cross-platform)
// Note: We rely on GLFW to load OpenGL, so we include GLFW first
#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// OpenGL extension loader (simple version for GL 2.1)
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84
#endif

// Function pointers for OpenGL extensions
typedef void (*PFNGLGENBUFFERSPROC)(int n, unsigned int* buffers);
typedef void (*PFNGLDELETEBUFFERSPROC)(int n, const unsigned int* buffers);
typedef void (*PFNGLBINDBUFFERPROC)(unsigned int target, unsigned int buffer);
typedef void (*PFNGLBUFFERDATAPROC)(unsigned int target, ptrdiff_t size, const void* data, unsigned int usage);
typedef unsigned int (*PFNGLCREATESHADERPROC)(unsigned int type);
typedef void (*PFNGLSHADERSOURCEPROC)(unsigned int shader, int count, const char** string, const int* length);
typedef void (*PFNGLCOMPILESHADERPROC)(unsigned int shader);
typedef void (*PFNGLGETSHADERIVPROC)(unsigned int shader, unsigned int pname, int* params);
typedef void (*PFNGLGETSHADERINFOLOGPROC)(unsigned int shader, int bufSize, int* length, char* infoLog);
typedef unsigned int (*PFNGLCREATEPROGRAMPROC)(void);
typedef void (*PFNGLATTACHSHADERPROC)(unsigned int program, unsigned int shader);
typedef void (*PFNGLLINKPROGRAMPROC)(unsigned int program);
typedef void (*PFNGLGETPROGRAMIVPROC)(unsigned int program, unsigned int pname, int* params);
typedef void (*PFNGLGETPROGRAMINFOLOGPROC)(unsigned int program, int bufSize, int* length, char* infoLog);
typedef void (*PFNGLUSEPROGRAMPROC)(unsigned int program);
typedef void (*PFNGLDELETESHADERPROC)(unsigned int shader);
typedef void (*PFNGLDELETEPROGRAMPROC)(unsigned int program);
typedef int (*PFNGLGETATTRIBLOCATIONPROC)(unsigned int program, const char* name);
typedef int (*PFNGLGETUNIFORMLOCATIONPROC)(unsigned int program, const char* name);
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, const void* pointer);
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(unsigned int index);
typedef void (*PFNGLDISABLEVERTEXATTRIBARRAYPROC)(unsigned int index);
typedef void (*PFNGLUNIFORMMATRIX4FVPROC)(int location, int count, unsigned char transpose, const float* value);

static PFNGLGENBUFFERSPROC glGenBuffers_ = NULL;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers_ = NULL;
static PFNGLBINDBUFFERPROC glBindBuffer_ = NULL;
static PFNGLBUFFERDATAPROC glBufferData_ = NULL;
static PFNGLCREATESHADERPROC glCreateShader_ = NULL;
static PFNGLSHADERSOURCEPROC glShaderSource_ = NULL;
static PFNGLCOMPILESHADERPROC glCompileShader_ = NULL;
static PFNGLGETSHADERIVPROC glGetShaderiv_ = NULL;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog_ = NULL;
static PFNGLCREATEPROGRAMPROC glCreateProgram_ = NULL;
static PFNGLATTACHSHADERPROC glAttachShader_ = NULL;
static PFNGLLINKPROGRAMPROC glLinkProgram_ = NULL;
static PFNGLGETPROGRAMIVPROC glGetProgramiv_ = NULL;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog_ = NULL;
static PFNGLUSEPROGRAMPROC glUseProgram_ = NULL;
static PFNGLDELETESHADERPROC glDeleteShader_ = NULL;
static PFNGLDELETEPROGRAMPROC glDeleteProgram_ = NULL;
static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation_ = NULL;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation_ = NULL;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer_ = NULL;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_ = NULL;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray_ = NULL;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv_ = NULL;

// Use GLFW's cross-platform function pointer loader
static void* gui_get_proc_address(const char* name) {
    return (void*)glfwGetProcAddress(name);
}

static void gui_load_gl_functions(void) {
    // Load all OpenGL function pointers using GLFW
    glGenBuffers_ = (PFNGLGENBUFFERSPROC)gui_get_proc_address("glGenBuffers");
    glDeleteBuffers_ = (PFNGLDELETEBUFFERSPROC)gui_get_proc_address("glDeleteBuffers");
    glBindBuffer_ = (PFNGLBINDBUFFERPROC)gui_get_proc_address("glBindBuffer");
    glBufferData_ = (PFNGLBUFFERDATAPROC)gui_get_proc_address("glBufferData");
    glCreateShader_ = (PFNGLCREATESHADERPROC)gui_get_proc_address("glCreateShader");
    glShaderSource_ = (PFNGLSHADERSOURCEPROC)gui_get_proc_address("glShaderSource");
    glCompileShader_ = (PFNGLCOMPILESHADERPROC)gui_get_proc_address("glCompileShader");
    glGetShaderiv_ = (PFNGLGETSHADERIVPROC)gui_get_proc_address("glGetShaderiv");
    glGetShaderInfoLog_ = (PFNGLGETSHADERINFOLOGPROC)gui_get_proc_address("glGetShaderInfoLog");
    glCreateProgram_ = (PFNGLCREATEPROGRAMPROC)gui_get_proc_address("glCreateProgram");
    glAttachShader_ = (PFNGLATTACHSHADERPROC)gui_get_proc_address("glAttachShader");
    glLinkProgram_ = (PFNGLLINKPROGRAMPROC)gui_get_proc_address("glLinkProgram");
    glGetProgramiv_ = (PFNGLGETPROGRAMIVPROC)gui_get_proc_address("glGetProgramiv");
    glGetProgramInfoLog_ = (PFNGLGETPROGRAMINFOLOGPROC)gui_get_proc_address("glGetProgramInfoLog");
    glUseProgram_ = (PFNGLUSEPROGRAMPROC)gui_get_proc_address("glUseProgram");
    glDeleteShader_ = (PFNGLDELETESHADERPROC)gui_get_proc_address("glDeleteShader");
    glDeleteProgram_ = (PFNGLDELETEPROGRAMPROC)gui_get_proc_address("glDeleteProgram");
    glGetAttribLocation_ = (PFNGLGETATTRIBLOCATIONPROC)gui_get_proc_address("glGetAttribLocation");
    glGetUniformLocation_ = (PFNGLGETUNIFORMLOCATIONPROC)gui_get_proc_address("glGetUniformLocation");
    glVertexAttribPointer_ = (PFNGLVERTEXATTRIBPOINTERPROC)gui_get_proc_address("glVertexAttribPointer");
    glEnableVertexAttribArray_ = (PFNGLENABLEVERTEXATTRIBARRAYPROC)gui_get_proc_address("glEnableVertexAttribArray");
    glDisableVertexAttribArray_ = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)gui_get_proc_address("glDisableVertexAttribArray");
    glUniformMatrix4fv_ = (PFNGLUNIFORMMATRIX4FVPROC)gui_get_proc_address("glUniformMatrix4fv");
}

// Simple vertex shader
static const char* vertex_shader_src = 
    "#version 120\n"
    "uniform mat4 u_projection;\n"
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "attribute vec4 a_color;\n"
    "varying vec2 v_uv;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "    gl_Position = u_projection * vec4(a_pos, 0.0, 1.0);\n"
    "    v_uv = a_uv;\n"
    "    v_color = a_color;\n"
    "}\n";

// Simple fragment shader
static const char* fragment_shader_src =
    "#version 120\n"
    "varying vec2 v_uv;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
    "    gl_FragColor = v_color;\n"
    "}\n";

static unsigned int gui_compile_shader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader_(type);
    glShaderSource_(shader, 1, &source, NULL);
    glCompileShader_(shader);
    
    int success;
    glGetShaderiv_(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog_(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation error: %s\n", info_log);
        return 0;
    }
    
    return shader;
}

static unsigned int gui_create_shader_program(void) {
    unsigned int vertex_shader = gui_compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    unsigned int fragment_shader = gui_compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    
    if (!vertex_shader || !fragment_shader) {
        return 0;
    }
    
    unsigned int program = glCreateProgram_();
    glAttachShader_(program, vertex_shader);
    glAttachShader_(program, fragment_shader);
    glLinkProgram_(program);
    
    int success;
    glGetProgramiv_(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog_(program, 512, NULL, info_log);
        fprintf(stderr, "Shader linking error: %s\n", info_log);
        return 0;
    }
    
    glDeleteShader_(vertex_shader);
    glDeleteShader_(fragment_shader);
    
    return program;
}

void gui_backend_gl_init(gui_backend_gl_t* backend) {
    memset(backend, 0, sizeof(gui_backend_gl_t));
    
    // Load OpenGL functions
    gui_load_gl_functions();
    
    // Create buffers
    glGenBuffers_(1, &backend->vbo);
    glGenBuffers_(1, &backend->ebo);
    
    // Create shader program
    backend->shader_program = gui_create_shader_program();
    if (!backend->shader_program) {
        fprintf(stderr, "Failed to create shader program\n");
        return;
    }
    
    // Get attribute/uniform locations
    backend->attrib_pos = glGetAttribLocation_(backend->shader_program, "a_pos");
    backend->attrib_uv = glGetAttribLocation_(backend->shader_program, "a_uv");
    backend->attrib_color = glGetAttribLocation_(backend->shader_program, "a_color");
    backend->uniform_projection = glGetUniformLocation_(backend->shader_program, "u_projection");
}

void gui_backend_gl_shutdown(gui_backend_gl_t* backend) {
    if (backend->vbo) {
        glDeleteBuffers_(1, &backend->vbo);
    }
    if (backend->ebo) {
        glDeleteBuffers_(1, &backend->ebo);
    }
    if (backend->shader_program) {
        glDeleteProgram_(backend->shader_program);
    }
    memset(backend, 0, sizeof(gui_backend_gl_t));
}

void gui_backend_gl_render(gui_backend_gl_t* backend, gui_context_t* ctx) {
    if (ctx->vertex_count == 0 || ctx->index_count == 0) {
        return;
    }
    
    // Setup render state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    
    // Setup viewport
    glViewport(0, 0, (int)ctx->display_width, (int)ctx->display_height);
    
    // Setup orthographic projection matrix
    float L = 0.0f;
    float R = ctx->display_width;
    float T = 0.0f;
    float B = ctx->display_height;
    float projection[16] = {
        2.0f/(R-L),     0.0f,           0.0f,   0.0f,
        0.0f,           2.0f/(T-B),     0.0f,   0.0f,
        0.0f,           0.0f,          -1.0f,   0.0f,
        (R+L)/(L-R),    (T+B)/(B-T),    0.0f,   1.0f,
    };
    
    // Use shader program
    glUseProgram_(backend->shader_program);
    glUniformMatrix4fv_(backend->uniform_projection, 1, 0, projection);
    
    // Upload vertex and index data
    glBindBuffer_(GL_ARRAY_BUFFER, backend->vbo);
    glBufferData_(GL_ARRAY_BUFFER, sizeof(gui_vertex_t) * ctx->vertex_count, ctx->vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, backend->ebo);
    glBufferData_(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * ctx->index_count, ctx->indices, GL_DYNAMIC_DRAW);
    
    // Setup vertex attributes
    glEnableVertexAttribArray_(backend->attrib_pos);
    glEnableVertexAttribArray_(backend->attrib_uv);
    glEnableVertexAttribArray_(backend->attrib_color);
    
    glVertexAttribPointer_(backend->attrib_pos, 2, GL_FLOAT, 0, sizeof(gui_vertex_t), 
                          (void*)offsetof(gui_vertex_t, pos));
    glVertexAttribPointer_(backend->attrib_uv, 2, GL_FLOAT, 0, sizeof(gui_vertex_t),
                          (void*)offsetof(gui_vertex_t, uv));
    glVertexAttribPointer_(backend->attrib_color, 4, GL_UNSIGNED_BYTE, 1, sizeof(gui_vertex_t),
                          (void*)offsetof(gui_vertex_t, col));
    
    // Render all draw commands
    uint32_t idx_offset = 0;
    for (uint32_t cmd_i = 0; cmd_i < ctx->draw_command_count; cmd_i++) {
        gui_draw_cmd_t* cmd = &ctx->draw_commands[cmd_i];
        
        if (cmd->type == GUI_DRAW_CMD_SET_CLIP_RECT) {
            // Set scissor rect
            glScissor((int)cmd->clip_rect.x, 
                     (int)(ctx->display_height - cmd->clip_rect.y - cmd->clip_rect.h),
                     (int)cmd->clip_rect.w, 
                     (int)cmd->clip_rect.h);
        } else if (cmd->type == GUI_DRAW_CMD_TRIANGLES) {
            glDrawElements(GL_TRIANGLES, cmd->elem_count, GL_UNSIGNED_INT, 
                          (void*)(uintptr_t)(idx_offset * sizeof(uint32_t)));
            idx_offset += cmd->elem_count;
        }
    }
    
    // Cleanup
    glDisableVertexAttribArray_(backend->attrib_pos);
    glDisableVertexAttribArray_(backend->attrib_uv);
    glDisableVertexAttribArray_(backend->attrib_color);
    glBindBuffer_(GL_ARRAY_BUFFER, 0);
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram_(0);
    glDisable(GL_SCISSOR_TEST);
}

#endif // CGUI_BACKEND_GL_IMPLEMENTATION

