/*
 * CGUI - Single-Header Immediate Mode GUI Library
 * C17 | Zero Dependencies (Frontend) | ~10K Lines Target
 *
 * Usage:
 *   #define CGUI_IMPLEMENTATION
 *   #include "cgui.h"
 *
 * License: MIT
 */

#ifndef CGUI_H
#define CGUI_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// CONFIGURATION
// =============================================================================

#ifndef CGUI_MAX_VERTICES
#define CGUI_MAX_VERTICES 65536
#endif

#ifndef CGUI_MAX_INDICES
#define CGUI_MAX_INDICES 98304
#endif

#ifndef CGUI_MAX_DRAW_COMMANDS
#define CGUI_MAX_DRAW_COMMANDS 1024
#endif

#ifndef CGUI_FRAME_ALLOCATOR_SIZE
#define CGUI_FRAME_ALLOCATOR_SIZE (1024 * 1024 * 4) // 4 MB
#endif

#ifndef CGUI_MAX_CLIP_STACK
#define CGUI_MAX_CLIP_STACK 32
#endif

#ifndef CGUI_MAX_LAYOUT_STACK
#define CGUI_MAX_LAYOUT_STACK 64
#endif

// =============================================================================
// TYPES & STRUCTURES
// =============================================================================

typedef uint32_t gui_id_t;
typedef void *gui_texture_id_t;

// Color (RGBA)
typedef struct {
    uint8_t r, g, b, a;
} gui_color_t;

// Predefined colors
#define GUI_COLOR_WHITE ((gui_color_t){255, 255, 255, 255})
#define GUI_COLOR_BLACK ((gui_color_t){0, 0, 0, 255})
#define GUI_COLOR_RED ((gui_color_t){255, 0, 0, 255})
#define GUI_COLOR_GREEN ((gui_color_t){0, 255, 0, 255})
#define GUI_COLOR_BLUE ((gui_color_t){0, 0, 255, 255})
#define GUI_COLOR_YELLOW ((gui_color_t){255, 255, 0, 255})
#define GUI_COLOR_CYAN ((gui_color_t){0, 255, 255, 255})
#define GUI_COLOR_MAGENTA ((gui_color_t){255, 0, 255, 255})
#define GUI_COLOR_GRAY ((gui_color_t){128, 128, 128, 255})
#define GUI_COLOR_LIGHT_GRAY ((gui_color_t){200, 200, 200, 255})
#define GUI_COLOR_DARK_GRAY ((gui_color_t){64, 64, 64, 255})

// Vector2
typedef struct {
    float x, y;
} gui_vec2_t;

// Rectangle
typedef struct {
    float x, y, w, h;
} gui_rect_t;

// Vertex (for rendering)
typedef struct {
    gui_vec2_t pos;
    gui_vec2_t uv;
    gui_color_t col;
} gui_vertex_t;

// Draw command
typedef enum {
    GUI_DRAW_CMD_TRIANGLES,
    GUI_DRAW_CMD_SET_CLIP_RECT,
} gui_draw_cmd_type_t;

typedef struct {
    gui_draw_cmd_type_t type;
    gui_texture_id_t texture;
    uint32_t idx_offset;
    uint32_t elem_count;
    gui_rect_t clip_rect;
} gui_draw_cmd_t;

// Mouse buttons
typedef enum {
    GUI_MOUSE_BUTTON_LEFT = 0,
    GUI_MOUSE_BUTTON_RIGHT = 1,
    GUI_MOUSE_BUTTON_MIDDLE = 2,
    GUI_MOUSE_BUTTON_COUNT = 3,
} gui_mouse_button_t;

// Input state
typedef struct {
    gui_vec2_t mouse_pos;
    bool mouse_down[GUI_MOUSE_BUTTON_COUNT];
    bool mouse_clicked[GUI_MOUSE_BUTTON_COUNT];
    bool mouse_released[GUI_MOUSE_BUTTON_COUNT];
    float mouse_wheel;
    bool keys[512];
    bool keys_pressed[512];
    char text_input[32];
} gui_input_t;

// Layout types
typedef enum {
    GUI_LAYOUT_NONE,
    GUI_LAYOUT_VBOX,
    GUI_LAYOUT_HBOX,
} gui_layout_type_t;

typedef struct {
    gui_layout_type_t type;
    gui_rect_t bounds;
    float cursor_x, cursor_y;
    float padding;
    float spacing;
    float item_width;
    float item_height;
} gui_layout_state_t;

// Frame allocator
typedef struct {
    uint8_t *buffer;
    size_t size;
    size_t used;
} gui_allocator_t;

// Style configuration
typedef struct {
    gui_color_t button_bg;
    gui_color_t button_bg_hovered;
    gui_color_t button_bg_active;
    gui_color_t button_text;
    gui_color_t text;
    gui_color_t slider_bg;
    gui_color_t slider_grab;
    gui_color_t slider_grab_active;
    float button_padding;
    float button_rounding;
    float slider_height;
    float slider_grab_size;
    float text_size;
} gui_style_t;

// Main context
typedef struct {
    // Memory management
    gui_allocator_t allocator;

    // Draw data
    gui_vertex_t *vertices;
    uint32_t vertex_count;
    uint32_t *indices;
    uint32_t index_count;
    gui_draw_cmd_t *draw_commands;
    uint32_t draw_command_count;

    // Clipping
    gui_rect_t clip_stack[CGUI_MAX_CLIP_STACK];
    int clip_stack_count;

    // Layout
    gui_layout_state_t layout_stack[CGUI_MAX_LAYOUT_STACK];
    int layout_stack_count;

    // Input
    gui_input_t input;
    gui_input_t prev_input;

    // State
    gui_id_t hot_item;
    gui_id_t active_item;
    gui_id_t focused_item;
    float time;
    float delta_time;

    // Style
    gui_style_t style;

    // Font texture
    gui_texture_id_t font_texture;
    float font_size;

    // Screen size
    float display_width;
    float display_height;
} gui_context_t;

// =============================================================================
// CORE API
// =============================================================================

// Context management
void gui_init(gui_context_t *ctx);
void gui_shutdown(gui_context_t *ctx);

// Frame lifecycle
void gui_begin_frame(gui_context_t *ctx, float display_width, float display_height);
void gui_end_frame(gui_context_t *ctx);

// Input handling
void gui_update_input(gui_context_t *ctx, float mouse_x, float mouse_y, const bool *mouse_buttons,
                      float mouse_wheel, float delta_time);

// =============================================================================
// LAYOUT API
// =============================================================================

void gui_begin_vbox(gui_context_t *ctx, float x, float y, float width, float padding,
                    float spacing);
void gui_end_vbox(gui_context_t *ctx);

void gui_begin_hbox(gui_context_t *ctx, float x, float y, float height, float padding,
                    float spacing);
void gui_end_hbox(gui_context_t *ctx);

void gui_same_line(gui_context_t *ctx);
void gui_spacing(gui_context_t *ctx, float amount);

// =============================================================================
// WIDGET API
// =============================================================================

void gui_label(gui_context_t *ctx, const char *text);
bool gui_button(gui_context_t *ctx, const char *label, float width, float height);
bool gui_slider_float(gui_context_t *ctx, const char *label, float *value, float min, float max,
                      float width);

// =============================================================================
// DRAW API (Low-Level Primitives)
// =============================================================================

void gui_add_rect(gui_context_t *ctx, float x, float y, float w, float h, gui_color_t color,
                  float thickness);
void gui_add_rect_filled(gui_context_t *ctx, float x, float y, float w, float h, gui_color_t color);
void gui_add_circle(gui_context_t *ctx, float cx, float cy, float radius, gui_color_t color,
                    float thickness);
void gui_add_circle_filled(gui_context_t *ctx, float cx, float cy, float radius, gui_color_t color);
void gui_add_line(gui_context_t *ctx, float x1, float y1, float x2, float y2, gui_color_t color,
                  float thickness);
void gui_add_triangle_filled(gui_context_t *ctx, float x1, float y1, float x2, float y2, float x3,
                             float y3, gui_color_t color);

void gui_add_text(gui_context_t *ctx, const char *text, float x, float y, gui_color_t color,
                  float font_size);

// Clipping
void gui_push_clip_rect(gui_context_t *ctx, float x, float y, float w, float h,
                        bool intersect_with_current);
void gui_pop_clip_rect(gui_context_t *ctx);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

gui_id_t gui_hash_string(const char *str);
gui_color_t gui_color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
bool gui_rect_contains(gui_rect_t rect, float x, float y);

// =============================================================================
// IMPLEMENTATION
// =============================================================================

#ifdef CGUI_IMPLEMENTATION

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============================================================================
// FRAME ALLOCATOR
// =============================================================================

static void gui_reset_allocator(gui_allocator_t *alloc) { alloc->used = 0; }

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

gui_id_t gui_hash_string(const char *str) {
    gui_id_t hash = 2166136261U;
    while (*str) {
        hash ^= (uint8_t)(*str++);
        hash *= 16777619U;
    }
    return hash;
}

gui_color_t gui_color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    gui_color_t col = {r, g, b, a};
    return col;
}

bool gui_rect_contains(gui_rect_t rect, float x, float y) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

static gui_rect_t gui_intersect_rects(gui_rect_t a, gui_rect_t b) {
    float x1 = fmaxf(a.x, b.x);
    float y1 = fmaxf(a.y, b.y);
    float x2 = fminf(a.x + a.w, b.x + b.w);
    float y2 = fminf(a.y + a.h, b.y + b.h);

    gui_rect_t result = {x1, y1, fmaxf(0.0F, x2 - x1), fmaxf(0.0F, y2 - y1)};
    return result;
}

static float gui_text_width(const char *text, float font_size) {
    // Simple monospace approximation for MVP
    return (float)strlen(text) * font_size * 0.6F;
}

// =============================================================================
// CONTEXT MANAGEMENT
// =============================================================================

void gui_init(gui_context_t *ctx) {
    memset(ctx, 0, sizeof(gui_context_t));

    // Allocate frame buffer
    ctx->allocator.size = (size_t)CGUI_FRAME_ALLOCATOR_SIZE;
    ctx->allocator.buffer = (uint8_t *)malloc(ctx->allocator.size);
    ctx->allocator.used = 0;

    // Allocate draw buffers
    ctx->vertices = (gui_vertex_t *)malloc(sizeof(gui_vertex_t) * CGUI_MAX_VERTICES);
    ctx->indices = (uint32_t *)malloc(sizeof(uint32_t) * CGUI_MAX_INDICES);
    ctx->draw_commands = (gui_draw_cmd_t *)malloc(sizeof(gui_draw_cmd_t) * CGUI_MAX_DRAW_COMMANDS);

    // Initialize style (modern flat design)
    ctx->style.button_bg = gui_color_from_rgba(70, 130, 180, 255);
    ctx->style.button_bg_hovered = gui_color_from_rgba(90, 150, 200, 255);
    ctx->style.button_bg_active = gui_color_from_rgba(50, 110, 160, 255);
    ctx->style.button_text = GUI_COLOR_WHITE;
    ctx->style.text = gui_color_from_rgba(240, 240, 240, 255);
    ctx->style.slider_bg = gui_color_from_rgba(60, 60, 60, 255);
    ctx->style.slider_grab = gui_color_from_rgba(70, 130, 180, 255);
    ctx->style.slider_grab_active = gui_color_from_rgba(90, 150, 200, 255);
    ctx->style.button_padding = 8.0F;
    ctx->style.button_rounding = 4.0F;
    ctx->style.slider_height = 20.0F;
    ctx->style.slider_grab_size = 16.0F;
    ctx->style.text_size = 14.0F;

    ctx->font_size = 14.0F;
}

void gui_shutdown(gui_context_t *ctx) {
    if (ctx->allocator.buffer) {
        free(ctx->allocator.buffer);
    }
    if (ctx->vertices) {
        free(ctx->vertices);
    }
    if (ctx->indices) {
        free(ctx->indices);
    }
    if (ctx->draw_commands) {
        free(ctx->draw_commands);
    }
    memset(ctx, 0, sizeof(gui_context_t));
}

void gui_begin_frame(gui_context_t *ctx, float display_width, float display_height) {
    ctx->display_width = display_width;
    ctx->display_height = display_height;

    // Reset allocator
    gui_reset_allocator(&ctx->allocator);

    // Reset draw data
    ctx->vertex_count = 0;
    ctx->index_count = 0;
    ctx->draw_command_count = 0;

    // Reset layout stack
    ctx->layout_stack_count = 0;

    // Reset clip stack
    ctx->clip_stack_count = 0;
    gui_rect_t full_screen = {0, 0, display_width, display_height};
    ctx->clip_stack[ctx->clip_stack_count++] = full_screen;

    // Update input state
    ctx->prev_input = ctx->input;
    for (int i = 0; i < GUI_MOUSE_BUTTON_COUNT; i++) {
        ctx->input.mouse_clicked[i] = ctx->input.mouse_down[i] && !ctx->prev_input.mouse_down[i];
        ctx->input.mouse_released[i] = !ctx->input.mouse_down[i] && ctx->prev_input.mouse_down[i];
    }

    // Clear hot item if no active item
    if (ctx->active_item == 0) {
        ctx->hot_item = 0;
    }
}

void gui_end_frame(gui_context_t *ctx) {
    // Create a single draw command for all geometry
    if (ctx->index_count > 0 && ctx->draw_command_count < CGUI_MAX_DRAW_COMMANDS) {
        gui_draw_cmd_t *cmd = &ctx->draw_commands[ctx->draw_command_count++];
        cmd->type = GUI_DRAW_CMD_TRIANGLES;
        cmd->texture = NULL;
        cmd->idx_offset = 0;
        cmd->elem_count = ctx->index_count;
        cmd->clip_rect = ctx->clip_stack[ctx->clip_stack_count - 1];
    }
}

void gui_update_input(gui_context_t *ctx, float mouse_x, float mouse_y, const bool *mouse_buttons,
                      float mouse_wheel, float delta_time) {
    ctx->input.mouse_pos.x = mouse_x;
    ctx->input.mouse_pos.y = mouse_y;

    for (int i = 0; i < GUI_MOUSE_BUTTON_COUNT; i++) {
        ctx->input.mouse_down[i] = mouse_buttons[i];
    }

    ctx->input.mouse_wheel = mouse_wheel;
    ctx->delta_time = delta_time;
    ctx->time += delta_time;
}

// =============================================================================
// LAYOUT SYSTEM
// =============================================================================

static gui_layout_state_t *gui_get_current_layout(gui_context_t *ctx) {
    if (ctx->layout_stack_count == 0) {
        return NULL;
    }
    return &ctx->layout_stack[ctx->layout_stack_count - 1];
}

void gui_begin_vbox(gui_context_t *ctx, float x, float y, float width, float padding,
                    float spacing) {
    if (ctx->layout_stack_count >= CGUI_MAX_LAYOUT_STACK) {
        return;
    }

    gui_layout_state_t *layout = &ctx->layout_stack[ctx->layout_stack_count++];
    layout->type = GUI_LAYOUT_VBOX;
    layout->bounds.x = x;
    layout->bounds.y = y;
    layout->bounds.w = width;
    layout->cursor_x = x + padding;
    layout->cursor_y = y + padding;
    layout->padding = padding;
    layout->spacing = spacing;
    layout->item_width = width - (padding * 2.0F);
}

void gui_end_vbox(gui_context_t *ctx) {
    if (ctx->layout_stack_count > 0) {
        ctx->layout_stack_count--;
    }
}

void gui_begin_hbox(gui_context_t *ctx, float x, float y, float height, float padding,
                    float spacing) {
    if (ctx->layout_stack_count >= CGUI_MAX_LAYOUT_STACK) {
        return;
    }

    gui_layout_state_t *layout = &ctx->layout_stack[ctx->layout_stack_count++];
    layout->type = GUI_LAYOUT_HBOX;
    layout->bounds.x = x;
    layout->bounds.y = y;
    layout->bounds.h = height;
    layout->cursor_x = x + padding;
    layout->cursor_y = y + padding;
    layout->padding = padding;
    layout->spacing = spacing;
    layout->item_height = height - (padding * 2.0F);
}

void gui_end_hbox(gui_context_t *ctx) {
    if (ctx->layout_stack_count > 0) {
        ctx->layout_stack_count--;
    }
}

void gui_spacing(gui_context_t *ctx, float amount) {
    gui_layout_state_t *layout = gui_get_current_layout(ctx);
    if (!layout) {
        return;
    }

    if (layout->type == GUI_LAYOUT_VBOX) {
        layout->cursor_y += amount;
    } else if (layout->type == GUI_LAYOUT_HBOX) {
        layout->cursor_x += amount;
    }
}

// =============================================================================
// DRAWING PRIMITIVES
// =============================================================================

static void gui_prim_reserve(gui_context_t *ctx, int vtx_count, int idx_count) {
    if (ctx->vertex_count + vtx_count > CGUI_MAX_VERTICES ||
        ctx->index_count + idx_count > CGUI_MAX_INDICES) {
        return; // Out of space
    }
}

static void gui_prim_rect_filled(gui_context_t *ctx, float x, float y, float w, float h,
                                 gui_color_t color) {
    gui_prim_reserve(ctx, 4, 6);

    uint32_t idx = ctx->vertex_count;

    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x, y}, {0, 0}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x + w, y}, {1, 0}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x + w, y + h}, {1, 1}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x, y + h}, {0, 1}, color};

    ctx->indices[ctx->index_count++] = idx + 0;
    ctx->indices[ctx->index_count++] = idx + 1;
    ctx->indices[ctx->index_count++] = idx + 2;
    ctx->indices[ctx->index_count++] = idx + 0;
    ctx->indices[ctx->index_count++] = idx + 2;
    ctx->indices[ctx->index_count++] = idx + 3;
}

void gui_add_rect_filled(gui_context_t *ctx, float x, float y, float w, float h,
                         gui_color_t color) {
    gui_prim_rect_filled(ctx, x, y, w, h, color);
}

void gui_add_rect(gui_context_t *ctx, float x, float y, float w, float h, gui_color_t color,
                  float thickness) {
    // Draw four lines to form a rectangle
    gui_add_line(ctx, x, y, x + w, y, color, thickness);
    gui_add_line(ctx, x + w, y, x + w, y + h, color, thickness);
    gui_add_line(ctx, x + w, y + h, x, y + h, color, thickness);
    gui_add_line(ctx, x, y + h, x, y, color, thickness);
}

void gui_add_line(gui_context_t *ctx, float x1, float y1, float x2, float y2, gui_color_t color,
                  float thickness) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf((dx * dx) + (dy * dy));

    if (len < 0.001F) {
        return;
    }

    float nx = -dy / len * thickness * 0.5F;
    float ny = dx / len * thickness * 0.5F;

    gui_prim_reserve(ctx, 4, 6);
    uint32_t idx = ctx->vertex_count;

    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x1 + nx, y1 + ny}, {0, 0}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x2 + nx, y2 + ny}, {0, 0}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x2 - nx, y2 - ny}, {0, 0}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x1 - nx, y1 - ny}, {0, 0}, color};

    ctx->indices[ctx->index_count++] = idx + 0;
    ctx->indices[ctx->index_count++] = idx + 1;
    ctx->indices[ctx->index_count++] = idx + 2;
    ctx->indices[ctx->index_count++] = idx + 0;
    ctx->indices[ctx->index_count++] = idx + 2;
    ctx->indices[ctx->index_count++] = idx + 3;
}

void gui_add_circle_filled(gui_context_t *ctx, float cx, float cy, float radius,
                           gui_color_t color) {
    const int segments = 32;
    gui_prim_reserve(ctx, segments + 1, segments * 3);

    uint32_t center_idx = ctx->vertex_count;
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{cx, cy}, {0.5F, 0.5F}, color};

    for (int i = 0; i <= segments; i++) {
        float angle = ((float)i / (float)segments) * 2.0F * 3.14159265359F;
        float x = cx + (cosf(angle) * radius);
        float y = cy + (sinf(angle) * radius);
        ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x, y}, {0, 0}, color};
    }

    for (int i = 0; i < segments; i++) {
        ctx->indices[ctx->index_count++] = center_idx;
        ctx->indices[ctx->index_count++] = center_idx + i + 1;
        ctx->indices[ctx->index_count++] = center_idx + i + 2;
    }
}

void gui_add_circle(gui_context_t *ctx, float cx, float cy, float radius, gui_color_t color,
                    float thickness) {
    const int segments = 32;
    for (int i = 0; i < segments; i++) {
        float angle1 = ((float)i / (float)segments) * 2.0F * 3.14159265359F;
        float angle2 = ((float)(i + 1) / (float)segments) * 2.0F * 3.14159265359F;
        float x1 = cx + (cosf(angle1) * radius);
        float y1 = cy + (sinf(angle1) * radius);
        float x2 = cx + (cosf(angle2) * radius);
        float y2 = cy + (sinf(angle2) * radius);
        gui_add_line(ctx, x1, y1, x2, y2, color, thickness);
    }
}

void gui_add_triangle_filled(gui_context_t *ctx, float x1, float y1, float x2, float y2, float x3,
                             float y3, gui_color_t color) {
    gui_prim_reserve(ctx, 3, 3);
    uint32_t idx = ctx->vertex_count;

    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x1, y1}, {0, 0}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x2, y2}, {0, 0}, color};
    ctx->vertices[ctx->vertex_count++] = (gui_vertex_t){{x3, y3}, {0, 0}, color};

    ctx->indices[ctx->index_count++] = idx + 0;
    ctx->indices[ctx->index_count++] = idx + 1;
    ctx->indices[ctx->index_count++] = idx + 2;
}

void gui_add_text(gui_context_t *ctx, const char *text, float x, float y, gui_color_t color,
                  float font_size) {
    // MVP: Simple text rendering using rectangles (placeholder for actual font rendering)
    float cursor_x = x;
    float char_width = font_size * 0.6F;
    float char_height = font_size;

    while (*text) {
        if (*text == ' ') {
            cursor_x += char_width;
        } else {
            // Draw a simple rectangle representing each character
            gui_add_rect_filled(ctx, cursor_x, y, char_width * 0.8F, char_height, color);
            cursor_x += char_width;
        }
        text++;
    }
}

void gui_push_clip_rect(gui_context_t *ctx, float x, float y, float w, float h,
                        bool intersect_with_current) {
    if (ctx->clip_stack_count >= CGUI_MAX_CLIP_STACK) {
        return;
    }

    gui_rect_t clip = {x, y, w, h};

    if (intersect_with_current && ctx->clip_stack_count > 0) {
        gui_rect_t current = ctx->clip_stack[ctx->clip_stack_count - 1];
        clip = gui_intersect_rects(current, clip);
    }

    ctx->clip_stack[ctx->clip_stack_count++] = clip;
}

void gui_pop_clip_rect(gui_context_t *ctx) {
    if (ctx->clip_stack_count > 1) {
        ctx->clip_stack_count--;
    }
}

// =============================================================================
// WIDGETS
// =============================================================================

void gui_label(gui_context_t *ctx, const char *text) {
    gui_layout_state_t *layout = gui_get_current_layout(ctx);

    float x;
    float y;
    if (layout) {
        x = layout->cursor_x;
        y = layout->cursor_y;

        float text_h = ctx->style.text_size;

        if (layout->type == GUI_LAYOUT_VBOX) {
            layout->cursor_y += text_h + layout->spacing;
        } else if (layout->type == GUI_LAYOUT_HBOX) {
            float text_w = gui_text_width(text, ctx->style.text_size);
            layout->cursor_x += text_w + layout->spacing;
        }
    } else {
        x = 10;
        y = 10;
    }

    gui_add_text(ctx, text, x, y, ctx->style.text, ctx->style.text_size);
}

bool gui_button(gui_context_t *ctx, const char *label, float width, float height) {
    gui_layout_state_t *layout = gui_get_current_layout(ctx);

    // Calculate button position and size
    float x;
    float y;
    float w;
    float h;
    if (layout) {
        x = layout->cursor_x;
        y = layout->cursor_y;

        if (layout->type == GUI_LAYOUT_VBOX) {
            w = (width > 0) ? width : layout->item_width;
            h = (height > 0) ? height : 30.0F;
            layout->cursor_y += h + layout->spacing;
        } else if (layout->type == GUI_LAYOUT_HBOX) {
            w = (width > 0) ? width : 100.0F;
            h = (height > 0) ? height : layout->item_height;
            layout->cursor_x += w + layout->spacing;
        } else {
            w = (width > 0) ? width : 100.0F;
            h = (height > 0) ? height : 30.0F;
        }
    } else {
        x = 10;
        y = 10;
        w = (width > 0) ? width : 100.0F;
        h = (height > 0) ? height : 30.0F;
    }

    // Generate unique ID
    gui_id_t id = gui_hash_string(label);

    // Check interaction
    gui_rect_t rect = {x, y, w, h};
    bool hovered = gui_rect_contains(rect, ctx->input.mouse_pos.x, ctx->input.mouse_pos.y);
    bool clicked = false;

    if (hovered) {
        ctx->hot_item = id;
        if (ctx->active_item == 0 && ctx->input.mouse_clicked[GUI_MOUSE_BUTTON_LEFT]) {
            ctx->active_item = id;
        }
    }

    if (ctx->active_item == id) {
        if (ctx->input.mouse_released[GUI_MOUSE_BUTTON_LEFT]) {
            if (hovered) {
                clicked = true;
            }
            ctx->active_item = 0;
        }
    }

    // Determine color based on state
    gui_color_t bg_color;
    if (ctx->active_item == id) {
        bg_color = ctx->style.button_bg_active;
    } else if (ctx->hot_item == id) {
        bg_color = ctx->style.button_bg_hovered;
    } else {
        bg_color = ctx->style.button_bg;
    }

    // Draw button
    gui_add_rect_filled(ctx, x, y, w, h, bg_color);
    gui_add_rect(ctx, x, y, w, h, GUI_COLOR_BLACK, 1.0F);

    // Draw label (centered)
    float text_w = gui_text_width(label, ctx->style.text_size);
    float text_x = x + ((w - text_w) * 0.5F);
    float text_y = y + ((h - ctx->style.text_size) * 0.5F);
    gui_add_text(ctx, label, text_x, text_y, ctx->style.button_text, ctx->style.text_size);

    return clicked;
}

bool gui_slider_float(gui_context_t *ctx, const char *label, float *value, float min, float max,
                      float width) {
    gui_layout_state_t *layout = gui_get_current_layout(ctx);

    // Calculate slider position and size
    float x;
    float y;
    float w;
    float h;
    if (layout) {
        x = layout->cursor_x;
        y = layout->cursor_y;

        if (layout->type == GUI_LAYOUT_VBOX) {
            w = (width > 0) ? width : layout->item_width;
            h = ctx->style.slider_height;
            layout->cursor_y += h + layout->spacing;
        } else if (layout->type == GUI_LAYOUT_HBOX) {
            w = (width > 0) ? width : 200.0F;
            h = ctx->style.slider_height;
            layout->cursor_x += w + layout->spacing;
        } else {
            w = (width > 0) ? width : 200.0F;
            h = ctx->style.slider_height;
        }
    } else {
        x = 10;
        y = 10;
        w = (width > 0) ? width : 200.0F;
        h = ctx->style.slider_height;
    }

    // Generate unique ID
    gui_id_t id = gui_hash_string(label);

    // Calculate grab position
    float normalized = (*value - min) / (max - min);
    if (normalized < 0.0F) {
        normalized = 0.0F;
    }
    if (normalized > 1.0F) {
        normalized = 1.0F;
    }

    float grab_w = ctx->style.slider_grab_size;
    float grab_x = x + ((w - grab_w) * normalized);
    float grab_y = y + ((h - grab_w) * 0.5F);

    gui_rect_t grab_rect = {grab_x, grab_y, grab_w, grab_w};
    gui_rect_t track_rect = {x, y, w, h};

    bool hovered = gui_rect_contains(grab_rect, ctx->input.mouse_pos.x, ctx->input.mouse_pos.y) ||
                   gui_rect_contains(track_rect, ctx->input.mouse_pos.x, ctx->input.mouse_pos.y);
    bool changed = false;

    if (hovered) {
        ctx->hot_item = id;
        if (ctx->active_item == 0 && ctx->input.mouse_clicked[GUI_MOUSE_BUTTON_LEFT]) {
            ctx->active_item = id;
        }
    }

    if (ctx->active_item == id) {
        if (ctx->input.mouse_down[GUI_MOUSE_BUTTON_LEFT]) {
            // Update value based on mouse position
            float mouse_norm = (ctx->input.mouse_pos.x - x) / w;
            if (mouse_norm < 0.0F) {
                mouse_norm = 0.0F;
            }
            if (mouse_norm > 1.0F) {
                mouse_norm = 1.0F;
            }

            float new_value = min + (mouse_norm * (max - min));
            if (new_value != *value) {
                *value = new_value;
                changed = true;
            }
        } else {
            ctx->active_item = 0;
        }
    }

    // Draw slider track
    gui_add_rect_filled(ctx, x, y, w, h, ctx->style.slider_bg);

    // Draw slider grab
    gui_color_t grab_color =
        (ctx->active_item == id) ? ctx->style.slider_grab_active : ctx->style.slider_grab;
    gui_add_circle_filled(ctx, grab_x + (grab_w * 0.5F), grab_y + (grab_w * 0.5F), grab_w * 0.5F,
                          grab_color);

    return changed;
}

#endif // CGUI_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // CGUI_H
