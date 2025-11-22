/*
 * CGUI Example/Demo Program
 * Demonstrates basic usage of the immediate mode GUI library
 */

#define CGUI_IMPLEMENTATION
#include "cgui.h"

#define CGUI_BACKEND_GL_IMPLEMENTATION
#include "cgui_backend_gl.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

// Global state
static gui_context_t gui_ctx;
static gui_backend_gl_t backend;
static bool mouse_buttons[3] = {false, false, false};
static float last_time = 0.0f;

// Demo state
static float slider_value = 0.5f;
static int button_click_count = 0;

void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)window;
    (void)mods;
    
    if (button >= 0 && button < 3) {
        mouse_buttons[button] = (action == GLFW_PRESS);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    gui_ctx.input.mouse_wheel = (float)yoffset;
}

int main(void) {
    // Initialize GLFW
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    // Create window with OpenGL 2.1 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(1280, 720, "CGUI Demo - Immediate Mode GUI", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    // Setup input callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Initialize GUI
    gui_init(&gui_ctx);
    gui_backend_gl_init(&backend);
    
    printf("CGUI Demo Started\n");
    printf("- C17 Immediate Mode GUI Library\n");
    printf("- Zero dependencies (frontend)\n");
    printf("- Single-header design\n");
    printf("- GLFW + OpenGL 2.1 backend\n\n");
    
    last_time = (float)glfwGetTime();
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll events
        glfwPollEvents();
        
        // Get window size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        
        // Get mouse position
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        
        // Calculate delta time
        float current_time = (float)glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;
        
        // Update input
        gui_update_input(&gui_ctx, (float)mouse_x, (float)mouse_y, mouse_buttons, 
                        gui_ctx.input.mouse_wheel, delta_time);
        gui_ctx.input.mouse_wheel = 0.0f; // Reset scroll
        
        // Begin frame
        gui_begin_frame(&gui_ctx, (float)display_w, (float)display_h);
        
        // =============================================================================
        // GUI CODE (Immediate Mode)
        // =============================================================================
        
        // Main vertical layout container
        gui_begin_vbox(&gui_ctx, 20, 20, 400, 10, 10);
        {
            // Title
            gui_label(&gui_ctx, "CGUI Demo - Immediate Mode");
            gui_spacing(&gui_ctx, 10);
            
            // Button example
            gui_label(&gui_ctx, "Button Example:");
            if (gui_button(&gui_ctx, "Click Me!", 0, 0)) {
                button_click_count++;
                printf("Button clicked! Count: %d\n", button_click_count);
            }
            
            // Display click count
            char count_text[64];
            snprintf(count_text, sizeof(count_text), "Clicks: %d", button_click_count);
            gui_label(&gui_ctx, count_text);
            
            gui_spacing(&gui_ctx, 20);
            
            // Slider example
            gui_label(&gui_ctx, "Slider Example:");
            if (gui_slider_float(&gui_ctx, "slider1", &slider_value, 0.0f, 1.0f, 0)) {
                printf("Slider value: %.2f\n", slider_value);
            }
            
            // Display slider value
            char value_text[64];
            snprintf(value_text, sizeof(value_text), "Value: %.2f", slider_value);
            gui_label(&gui_ctx, value_text);
            
            gui_spacing(&gui_ctx, 20);
            
            // Multiple buttons in vertical layout
            gui_label(&gui_ctx, "Multiple Buttons:");
            if (gui_button(&gui_ctx, "Button A", 0, 0)) {
                printf("Button A pressed\n");
            }
            if (gui_button(&gui_ctx, "Button B", 0, 0)) {
                printf("Button B pressed\n");
            }
            if (gui_button(&gui_ctx, "Button C", 0, 0)) {
                printf("Button C pressed\n");
            }
        }
        gui_end_vbox(&gui_ctx);
        
        // Horizontal layout example
        gui_begin_hbox(&gui_ctx, 20, 400, 50, 10, 10);
        {
            if (gui_button(&gui_ctx, "H1", 100, 0)) {
                printf("H1 pressed\n");
            }
            if (gui_button(&gui_ctx, "H2", 100, 0)) {
                printf("H2 pressed\n");
            }
            if (gui_button(&gui_ctx, "H3", 100, 0)) {
                printf("H3 pressed\n");
            }
        }
        gui_end_hbox(&gui_ctx);
        
        // Custom drawing example (showcase low-level draw API)
        float canvas_x = 500;
        float canvas_y = 20;
        float canvas_w = 400;
        float canvas_h = 400;
        
        // Draw canvas background
        gui_add_rect_filled(&gui_ctx, canvas_x, canvas_y, canvas_w, canvas_h, 
                           gui_color_from_rgba(30, 30, 30, 255));
        gui_add_rect(&gui_ctx, canvas_x, canvas_y, canvas_w, canvas_h, 
                     GUI_COLOR_WHITE, 2.0f);
        
        // Draw some shapes
        gui_add_circle_filled(&gui_ctx, canvas_x + 100, canvas_y + 100, 40, GUI_COLOR_RED);
        gui_add_circle(&gui_ctx, canvas_x + 100, canvas_y + 100, 50, GUI_COLOR_WHITE, 2.0f);
        
        gui_add_rect_filled(&gui_ctx, canvas_x + 200, canvas_y + 50, 80, 80, GUI_COLOR_GREEN);
        
        gui_add_triangle_filled(&gui_ctx, 
                               canvas_x + 300, canvas_y + 200,
                               canvas_x + 250, canvas_y + 300,
                               canvas_x + 350, canvas_y + 300,
                               GUI_COLOR_BLUE);
        
        // Draw animated circle based on slider value
        float anim_x = canvas_x + 50 + slider_value * (canvas_w - 100);
        float anim_y = canvas_y + 300;
        gui_add_circle_filled(&gui_ctx, anim_x, anim_y, 20, GUI_COLOR_YELLOW);
        
        // Draw some lines
        for (int i = 0; i < 10; i++) {
            float t = (float)i / 10.0f;
            gui_add_line(&gui_ctx, 
                        canvas_x + t * canvas_w, canvas_y + canvas_h - 50,
                        canvas_x + canvas_w - t * canvas_w, canvas_y + canvas_h - 10,
                        gui_color_from_rgba(255, (uint8_t)(t * 255), 255, 255), 2.0f);
        }
        
        // Label for custom drawing area
        gui_add_text(&gui_ctx, "Custom Draw API Demo", canvas_x + 10, canvas_y + canvas_h + 10,
                    GUI_COLOR_WHITE, 14.0f);
        
        // End frame
        gui_end_frame(&gui_ctx);
        
        // =============================================================================
        // RENDERING
        // =============================================================================
        
        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Render GUI
        gui_backend_gl_render(&backend, &gui_ctx);
        
        // Swap buffers
        glfwSwapBuffers(window);
    }
    
    // Cleanup
    gui_backend_gl_shutdown(&backend);
    gui_shutdown(&gui_ctx);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    printf("\nCGUI Demo Terminated\n");
    return 0;
}

