#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <iostream>
#include <algorithm>
#include <cmath>

#define STB_TRUETYPE_IMPLEMENTATION
#include "include/stb_truetype.h"

#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"

#include "types.h"
#include "math.h"
#include "utility.h"
#include "memory.h"
#include "render.cpp"
//#include "asset.cpp"
#include "shader.cpp"

struct Transient_Storage
{
    void *base;
    size_t size;
};
struct Transient_State
{
    Memory_Arena transient_arena;
};

struct Vertex
{
    V3 P;
    V4 C;
};

#define GET_UNIFORM_LOCATION(PROGRAM, NAME)\
    PROGRAM.NAME = glGetUniformLocation(PROGRAM.id, #NAME);
struct Rect_Program
{
    s32 id;

    s32 P;
};
Rect_Program rect_program;


enum Mouse_Button
{
    LEFT,
    RIGHT,

    MOUSE_BUTTON_COUNT,
};
struct Mouse_State
{
    b32 is_down[Mouse_Button::MOUSE_BUTTON_COUNT];
    b32 toggle[Mouse_Button::MOUSE_BUTTON_COUNT];
    V2 P;
};



static b32
mouse_is_in_rect(V2 mouse_P, Rect2 rect)
{
    b32 result;

    if (mouse_P.x >= rect.min.x &&
        mouse_P.y >= rect.min.y &&
        mouse_P.x <= rect.max.x &&
        mouse_P.y <= rect.max.y)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}


// @TODO: sort by z?
static s32 ui_id_next = 1;
struct UI_Context
{
    s32 active;
    s32 hot;
};
struct UI
{
    s32 id;

    V3 cen;
    V2 half_dim;
    V4 cold_color;
    V4 hot_color;

    V2 prev_mouse_P;

    f32 active_t;

    void update_and_render(UI_Context *context, Mouse_State *mouse_state, f32 dt,
                           std::vector<Render_Element> &batch, Memory_Arena *arena)
    {
        V2 mouse_P = mouse_state->P;
        V4 color = cold_color;

        if (context->active == id)
        {
            if (!mouse_state->is_down[Mouse_Button::LEFT] &&
                mouse_state->toggle[Mouse_Button::LEFT])
            {
                context->active = 0;
                context->hot = 0;
                color = hot_color;
            }
            else
            {
                cen.xy += (mouse_P - prev_mouse_P);
                prev_mouse_P = mouse_P;

                f32 t = sin(active_t);
                t *= t;
                active_t += dt*3.0f;
                color = lerp(hot_color, t, V4{1, 1, 1, 1});
            }
        }
        else if (context->hot == id)
        {
            if (mouse_is_in_rect(mouse_P, Rect2{cen.xy - half_dim, cen.xy + half_dim}))
            {
                if (mouse_state->is_down[Mouse_Button::LEFT] &&
                    mouse_state->toggle[Mouse_Button::LEFT])
                {
                    context->active = id;
                    color = hot_color;

                    prev_mouse_P = mouse_P;
                    active_t = 0.0f;
                }
                else
                {
                    color = hot_color;
                }
            }
            else
            {
                context->hot = 0;
                color = cold_color;
            }
        }
        else // cold
        {
            if (mouse_is_in_rect(mouse_P, Rect2{cen.xy - half_dim, cen.xy + half_dim}))
            {
                context->hot = id;
                color = hot_color;
            }
            else
            {
                color = cold_color;
            }
        }

        // Render
        push_rect_to_batch(cen, half_dim, color, batch, arena);
    }
};

static void
push_ui(std::list<UI> &list, V3 cen, V2 half_dim, V4 cold_color, V4 hot_color)
{
    UI ui = {};

    ui.id = ui_id_next++;

    ui.cen = cen;
    ui.half_dim = half_dim;
    ui.cold_color = cold_color;
    ui.hot_color = hot_color;

    list.push_back(ui);
}

static V2
get_window_dim(GLFWwindow *window)
{
    s32 iW, iH;
    f32 W, H;
    glfwGetWindowSize(window, &iW, &iH);
    W = (f32)iW;
    H = (f32)iH;

    V2 result =  V2{W, H};
    return result;
}

int main()
{
    if (glfwInit())
    {
        glfwWindowHint(GLFW_SAMPLES, 1);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow *window = glfwCreateWindow(1280, 720, "SW_UNTITLED", 0, 0);
        if (window)
        {
            glfwMakeContextCurrent(window);

            glewExperimental = true; // Needed for core profile
            

            // @TEMPORARY
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_TEXTURE_2D);

            glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

            glEnable(GL_DEPTH_TEST);
            glClearDepth(1);
            glClear(GL_DEPTH_BUFFER_BIT);
            glDepthFunc(GL_LEQUAL);


            if (glewInit() == GLEW_OK) 
            { 
                // Memory
                Transient_Storage transient_storage = {};
                transient_storage.size = (size_t)GB(1);
                transient_storage.base = malloc(transient_storage.size);

                if (transient_storage.base)
                {
                    Transient_State *transient_state = (Transient_State *)transient_storage.base;
                    transient_state->transient_arena.base = (u8 *)transient_storage.base + sizeof(Transient_State);
                    transient_state->transient_arena.used = 0;
                    transient_state->transient_arena.size = transient_storage.size - sizeof(Transient_State);

                    Mouse_State mouse_state = {};

                    // Ensure we can capture the escape key being pressed below
                    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

                    GLuint vertex_array_id;
                    glGenVertexArrays(1, &vertex_array_id);
                    glBindVertexArray(vertex_array_id);

                    // @TEMPORARY
                    rect_program.id = load_and_compile_shader("shader/simple.vs", "shader/simple.fs");
                    GET_UNIFORM_LOCATION(rect_program, P);

                    GLuint vertexbuffer;
                    glGenBuffers(1, &vertexbuffer);
                    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

                    V2 window_dim = get_window_dim(window);

                    std::vector<Render_Element> render_batch;

                    UI_Context ui_context = {};
                    std::list<UI> ui_list;
                    push_ui(ui_list, V3{window_dim*0.5f, 2}, V2{100, 100}, V4{1, 0, 0, 0.5f}, V4{1, 0, 0, 1});
                    push_ui(ui_list, V3{window_dim*0.4f, 1}, V2{90, 70}, V4{0, 1, 0, 0.5f}, V4{0, 1, 0, 1});
                    push_ui(ui_list, V3{window_dim*0.3f, 0}, V2{120, 130}, V4{0, 0, 1, 0.5f}, V4{0, 0, 1, 1});

                    b32 should_close = 0;
                    f32 prev_time = 0;

                    //
                    // Application Loop
                    //
                    while (!should_close)
                    {
                        //
                        // Process inputs.
                        //
                        // @TODO: What method does it use?
                        f32 cur_time = (f32)glfwGetTime();
                        f32 dt = cur_time - prev_time;
                        prev_time = cur_time;
                        printf("DT: %.6f, FPS: %u\n", dt, (u32)(1.0f / dt));

                        if (glfwGetKey(window, GLFW_KEY_ESCAPE) ||
                            glfwWindowShouldClose(window))
                        {
                            should_close = true;
                        }

                        // Get window dimension.
                        window_dim = get_window_dim(window);

                        // Get mouse info.
                        f64 mouse_Px, mouse_Py;
                        glfwGetCursorPos(window, &mouse_Px, &mouse_Py);
                        mouse_state.P = V2{(f32)mouse_Px, window_dim.y - (f32)mouse_Py};

                        s32 s = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
                        if (s == GLFW_RELEASE)
                        {
                            mouse_state.toggle[Mouse_Button::LEFT] = (mouse_state.is_down[Mouse_Button::LEFT]) ? 1 : 0;
                            mouse_state.is_down[Mouse_Button::LEFT] = 0;
                        }
                        else if (s == GLFW_PRESS)
                        {
                            mouse_state.toggle[Mouse_Button::LEFT] = (!mouse_state.is_down[Mouse_Button::LEFT]) ? 1 : 0;
                            mouse_state.is_down[Mouse_Button::LEFT] = 1;
                        }

                        //
                        // Simulate
                        //

                        f32 a = (2.0f / window_dim.x);
                        f32 b = (2.0f / window_dim.y);
                        f32 half_z_length = 1000.0f;
                        f32 c = 1.0f / half_z_length;
                        M4x4 orthographic_projection = M4x4{{
                            a,  0,  0, -1,
                            0,  b,  0, -1,
                            0,  0,  c,  0,
                            0,  0,  0,  1,
                        }};

                        // IMGUI
                        for (UI &ui : ui_list)
                        {
                            ui.update_and_render(&ui_context, &mouse_state, dt, render_batch, &transient_state->transient_arena);
                        }

                        //printf("HOT:%d, ACTIVE:%d\n", ui_context.hot, ui_context.active);


                        //
                        // Render
                        //
                        glViewport(0, 0, (u32)window_dim.x, (u32)window_dim.y);

                        glClear(GL_COLOR_BUFFER_BIT);
                        glClear(GL_DEPTH_BUFFER_BIT);

                        for (Render_Element &element : render_batch)
                        {
                            if (element.type == Render_Type::RECT)
                            {
                                Render_Rect *data = (Render_Rect *)element.data;

                                Vertex Vs[4] = {
                                    {data->cen + V3{+data->half_dim.x, -data->half_dim.y, 0}, data->color},
                                    {data->cen + V3{+data->half_dim.x, +data->half_dim.y, 0}, data->color},
                                    {data->cen + V3{-data->half_dim.x, -data->half_dim.y, 0}, data->color},
                                    {data->cen + V3{-data->half_dim.x, +data->half_dim.y, 0}, data->color},
                                };

                                Rect_Program *program = &rect_program;
                                glUseProgram(program->id);
                                glBufferData(GL_ARRAY_BUFFER, sizeof(Vs), Vs, GL_STATIC_DRAW);

                                glEnableVertexAttribArray(0);
                                glEnableVertexAttribArray(1);

                                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset_of(Vertex, P));
                                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset_of(Vertex, C));

                                glUniformMatrix4fv(program->P, 1, true, &orthographic_projection.e[0][0]);

                                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                                glDisableVertexAttribArray(0);
                                glDisableVertexAttribArray(1);
                            }
                        }


                        render_batch.clear();
                        transient_state->transient_arena.used = 0;


                        glfwSwapBuffers(window);
                        glfwPollEvents();
                    }

                    glDeleteBuffers(1, &vertexbuffer);
                    glDeleteVertexArrays(1, &vertex_array_id);
                    glDeleteProgram(rect_program.id);

                    glfwTerminate();
                }
                else
                {
                    fprintf(stderr, "[ERROR]: malloc() fail.\n");
                }
            }
            else
            {
                fprintf(stderr, "[ERROR]: Couldn't init glew.\n");
                glfwTerminate();
            }
        }
        else
        {
            fprintf(stderr, "[ERROR]: Couldn't create window.\n");
            glfwTerminate();
        }
    }
    else
    {
        fprintf(stderr, "[ERROR]: Couldn't init glfw.\n");
    }
}
