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
void gui_backend_gl_init(gui_backend_gl_t *backend);

// Shutdown OpenGL backend
void gui_backend_gl_shutdown(gui_backend_gl_t *backend);

// Render the GUI
void gui_backend_gl_render(gui_backend_gl_t *backend, gui_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif // CGUI_BACKEND_GL_H

// =============================================================================
// IMPLEMENTATION
// =============================================================================

#ifdef CGUI_BACKEND_GL_IMPLEMENTATION

#include <stdio.h>

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
typedef void (*PFNGLGENBUFFERSPROC)(int n, unsigned int *buffers);
typedef void (*PFNGLDELETEBUFFERSPROC)(int n, const unsigned int *buffers);
typedef void (*PFNGLBINDBUFFERPROC)(unsigned int target, unsigned int buffer);
typedef void (*PFNGLBUFFERDATAPROC)(unsigned int target, ptrdiff_t size, const void *data,
                                    unsigned int usage);
typedef unsigned int (*PFNGLCREATESHADERPROC)(unsigned int type);
typedef void (*PFNGLSHADERSOURCEPROC)(unsigned int shader, int count, const char **string,
                                      const int *length);
typedef void (*PFNGLCOMPILESHADERPROC)(unsigned int shader);
typedef void (*PFNGLGETSHADERIVPROC)(unsigned int shader, unsigned int pname, int *params);
typedef void (*PFNGLGETSHADERINFOLOGPROC)(unsigned int shader, int bufSize, int *length,
                                          char *infoLog);
typedef unsigned int (*PFNGLCREATEPROGRAMPROC)(void);
typedef void (*PFNGLATTACHSHADERPROC)(unsigned int program, unsigned int shader);
typedef void (*PFNGLLINKPROGRAMPROC)(unsigned int program);
typedef void (*PFNGLGETPROGRAMIVPROC)(unsigned int program, unsigned int pname, int *params);
typedef void (*PFNGLGETPROGRAMINFOLOGPROC)(unsigned int program, int bufSize, int *length,
                                           char *infoLog);
typedef void (*PFNGLUSEPROGRAMPROC)(unsigned int program);
typedef void (*PFNGLDELETESHADERPROC)(unsigned int shader);
typedef void (*PFNGLDELETEPROGRAMPROC)(unsigned int program);
typedef int (*PFNGLGETATTRIBLOCATIONPROC)(unsigned int program, const char *name);
typedef int (*PFNGLGETUNIFORMLOCATIONPROC)(unsigned int program, const char *name);
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(unsigned int index, int size, unsigned int type,
                                             unsigned char normalized, int stride,
                                             const void *pointer);
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(unsigned int index);
typedef void (*PFNGLDISABLEVERTEXATTRIBARRAYPROC)(unsigned int index);
typedef void (*PFNGLUNIFORMMATRIX4FVPROC)(int location, int count, unsigned char transpose,
                                          const float *value);

static PFNGLGENBUFFERSPROC gl_gen_buffers = NULL;
static PFNGLDELETEBUFFERSPROC gl_delete_buffers = NULL;
static PFNGLBINDBUFFERPROC gl_bind_buffer = NULL;
static PFNGLBUFFERDATAPROC gl_buffer_data = NULL;
static PFNGLCREATESHADERPROC gl_create_shader = NULL;
static PFNGLSHADERSOURCEPROC gl_shader_source = NULL;
static PFNGLCOMPILESHADERPROC gl_compile_shader = NULL;
static PFNGLGETSHADERIVPROC gl_get_shaderiv = NULL;
static PFNGLGETSHADERINFOLOGPROC gl_get_shader_info_log = NULL;
static PFNGLCREATEPROGRAMPROC gl_create_program = NULL;
static PFNGLATTACHSHADERPROC gl_attach_shader = NULL;
static PFNGLLINKPROGRAMPROC gl_link_program = NULL;
static PFNGLGETPROGRAMIVPROC gl_get_programiv = NULL;
static PFNGLGETPROGRAMINFOLOGPROC gl_get_program_info_log = NULL;
static PFNGLUSEPROGRAMPROC gl_use_program = NULL;
static PFNGLDELETESHADERPROC gl_delete_shader = NULL;
static PFNGLDELETEPROGRAMPROC gl_delete_program = NULL;
static PFNGLGETATTRIBLOCATIONPROC gl_get_attrib_location = NULL;
static PFNGLGETUNIFORMLOCATIONPROC gl_get_uniform_location = NULL;
static PFNGLVERTEXATTRIBPOINTERPROC gl_vertex_attrib_pointer = NULL;
static PFNGLENABLEVERTEXATTRIBARRAYPROC gl_enable_vertex_attrib_array = NULL;
static PFNGLDISABLEVERTEXATTRIBARRAYPROC gl_disable_vertex_attrib_array = NULL;
static PFNGLUNIFORMMATRIX4FVPROC gl_uniform_matrix4fv = NULL;

// Use GLFW's cross-platform function pointer loader
static void *gui_get_proc_address(const char *name) { return (void *)glfwGetProcAddress(name); }

static void gui_load_gl_functions(void) {
    // Load all OpenGL function pointers using GLFW
    gl_gen_buffers = (PFNGLGENBUFFERSPROC)gui_get_proc_address("glGenBuffers");
    gl_delete_buffers = (PFNGLDELETEBUFFERSPROC)gui_get_proc_address("glDeleteBuffers");
    gl_bind_buffer = (PFNGLBINDBUFFERPROC)gui_get_proc_address("glBindBuffer");
    gl_buffer_data = (PFNGLBUFFERDATAPROC)gui_get_proc_address("glBufferData");
    gl_create_shader = (PFNGLCREATESHADERPROC)gui_get_proc_address("glCreateShader");
    gl_shader_source = (PFNGLSHADERSOURCEPROC)gui_get_proc_address("glShaderSource");
    gl_compile_shader = (PFNGLCOMPILESHADERPROC)gui_get_proc_address("glCompileShader");
    gl_get_shaderiv = (PFNGLGETSHADERIVPROC)gui_get_proc_address("glGetShaderiv");
    gl_get_shader_info_log = (PFNGLGETSHADERINFOLOGPROC)gui_get_proc_address("glGetShaderInfoLog");
    gl_create_program = (PFNGLCREATEPROGRAMPROC)gui_get_proc_address("glCreateProgram");
    gl_attach_shader = (PFNGLATTACHSHADERPROC)gui_get_proc_address("glAttachShader");
    gl_link_program = (PFNGLLINKPROGRAMPROC)gui_get_proc_address("glLinkProgram");
    gl_get_programiv = (PFNGLGETPROGRAMIVPROC)gui_get_proc_address("glGetProgramiv");
    gl_get_program_info_log =
        (PFNGLGETPROGRAMINFOLOGPROC)gui_get_proc_address("glGetProgramInfoLog");
    gl_use_program = (PFNGLUSEPROGRAMPROC)gui_get_proc_address("glUseProgram");
    gl_delete_shader = (PFNGLDELETESHADERPROC)gui_get_proc_address("glDeleteShader");
    gl_delete_program = (PFNGLDELETEPROGRAMPROC)gui_get_proc_address("glDeleteProgram");
    gl_get_attrib_location =
        (PFNGLGETATTRIBLOCATIONPROC)gui_get_proc_address("glGetAttribLocation");
    gl_get_uniform_location =
        (PFNGLGETUNIFORMLOCATIONPROC)gui_get_proc_address("glGetUniformLocation");
    gl_vertex_attrib_pointer =
        (PFNGLVERTEXATTRIBPOINTERPROC)gui_get_proc_address("glVertexAttribPointer");
    gl_enable_vertex_attrib_array =
        (PFNGLENABLEVERTEXATTRIBARRAYPROC)gui_get_proc_address("glEnableVertexAttribArray");
    gl_disable_vertex_attrib_array =
        (PFNGLDISABLEVERTEXATTRIBARRAYPROC)gui_get_proc_address("glDisableVertexAttribArray");
    gl_uniform_matrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)gui_get_proc_address("glUniformMatrix4fv");
}

// Simple vertex shader
static const char *vertex_shader_src = "#version 120\n"
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
static const char *fragment_shader_src = "#version 120\n"
                                         "varying vec2 v_uv;\n"
                                         "varying vec4 v_color;\n"
                                         "void main() {\n"
                                         "    gl_FragColor = v_color;\n"
                                         "}\n";

static unsigned int gui_compile_shader(unsigned int type, const char *source) {
    unsigned int shader = gl_create_shader(type);
    gl_shader_source(shader, 1, &source, NULL);
    gl_compile_shader(shader);

    int success;
    gl_get_shaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        gl_get_shader_info_log(shader, 512, NULL, info_log);
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

    unsigned int program = gl_create_program();
    gl_attach_shader(program, vertex_shader);
    gl_attach_shader(program, fragment_shader);
    gl_link_program(program);

    int success;
    gl_get_programiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        gl_get_program_info_log(program, 512, NULL, info_log);
        fprintf(stderr, "Shader linking error: %s\n", info_log);
        return 0;
    }

    gl_delete_shader(vertex_shader);
    gl_delete_shader(fragment_shader);

    return program;
}

void gui_backend_gl_init(gui_backend_gl_t *backend) {
    memset(backend, 0, sizeof(gui_backend_gl_t));

    // Load OpenGL functions
    gui_load_gl_functions();

    // Create buffers
    gl_gen_buffers(1, &backend->vbo);
    gl_gen_buffers(1, &backend->ebo);

    // Create shader program
    backend->shader_program = gui_create_shader_program();
    if (!backend->shader_program) {
        fprintf(stderr, "Failed to create shader program\n");
        return;
    }

    // Get attribute/uniform locations
    backend->attrib_pos = gl_get_attrib_location(backend->shader_program, "a_pos");
    backend->attrib_uv = gl_get_attrib_location(backend->shader_program, "a_uv");
    backend->attrib_color = gl_get_attrib_location(backend->shader_program, "a_color");
    backend->uniform_projection = gl_get_uniform_location(backend->shader_program, "u_projection");
}

void gui_backend_gl_shutdown(gui_backend_gl_t *backend) {
    if (backend->vbo) {
        gl_delete_buffers(1, &backend->vbo);
    }
    if (backend->ebo) {
        gl_delete_buffers(1, &backend->ebo);
    }
    if (backend->shader_program) {
        gl_delete_program(backend->shader_program);
    }
    memset(backend, 0, sizeof(gui_backend_gl_t));
}

void gui_backend_gl_render(gui_backend_gl_t *backend, gui_context_t *ctx) {
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
    float l = 0.0F;
    float r = ctx->display_width;
    float t = 0.0F;
    float b = ctx->display_height;
    float projection[16] = {
        2.0F / (r - l), 0.0F, 0.0F, 0.0F,  0.0F, 2.0F / (t - b),    0.0F,
        0.0F,           0.0F, 0.0F, -1.0F, 0.0F, (r + l) / (l - r), (t + b) / (b - t),
        0.0F,           1.0F,
    };

    // Use shader program
    gl_use_program(backend->shader_program);
    gl_uniform_matrix4fv(backend->uniform_projection, 1, 0, projection);

    // Upload vertex and index data
    gl_bind_buffer(GL_ARRAY_BUFFER, backend->vbo);
    gl_buffer_data(GL_ARRAY_BUFFER, (ptrdiff_t)sizeof(gui_vertex_t) * ctx->vertex_count,
                   ctx->vertices, GL_DYNAMIC_DRAW);

    gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, backend->ebo);
    gl_buffer_data(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)sizeof(uint32_t) * ctx->index_count,
                   ctx->indices, GL_DYNAMIC_DRAW);

    // Setup vertex attributes
    gl_enable_vertex_attrib_array(backend->attrib_pos);
    gl_enable_vertex_attrib_array(backend->attrib_uv);
    gl_enable_vertex_attrib_array(backend->attrib_color);

    gl_vertex_attrib_pointer(backend->attrib_pos, 2, GL_FLOAT, 0, sizeof(gui_vertex_t),
                             (void *)offsetof(gui_vertex_t, pos));
    gl_vertex_attrib_pointer(backend->attrib_uv, 2, GL_FLOAT, 0, sizeof(gui_vertex_t),
                             (void *)offsetof(gui_vertex_t, uv));
    gl_vertex_attrib_pointer(backend->attrib_color, 4, GL_UNSIGNED_BYTE, 1, sizeof(gui_vertex_t),
                             (void *)offsetof(gui_vertex_t, col));

    // Render all draw commands
    uint32_t idx_offset = 0;
    for (uint32_t cmd_i = 0; cmd_i < ctx->draw_command_count; cmd_i++) {
        gui_draw_cmd_t *cmd = &ctx->draw_commands[cmd_i];

        if (cmd->type == GUI_DRAW_CMD_SET_CLIP_RECT) {
            // Set scissor rect
            glScissor((int)cmd->clip_rect.x,
                      (int)(ctx->display_height - cmd->clip_rect.y - cmd->clip_rect.h),
                      (int)cmd->clip_rect.w, (int)cmd->clip_rect.h);
        } else if (cmd->type == GUI_DRAW_CMD_TRIANGLES) {
            glDrawElements(GL_TRIANGLES, (int)cmd->elem_count, GL_UNSIGNED_INT,
                           (void *)(uintptr_t)(idx_offset * sizeof(uint32_t)));
            idx_offset += cmd->elem_count;
        }
    }

    // Cleanup
    gl_disable_vertex_attrib_array(backend->attrib_pos);
    gl_disable_vertex_attrib_array(backend->attrib_uv);
    gl_disable_vertex_attrib_array(backend->attrib_color);
    gl_bind_buffer(GL_ARRAY_BUFFER, 0);
    gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    gl_use_program(0);
    glDisable(GL_SCISSOR_TEST);
}

#endif // CGUI_BACKEND_GL_IMPLEMENTATION
