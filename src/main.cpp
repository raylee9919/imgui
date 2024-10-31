#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#define STB_TRUETYPE_IMPLEMENTATION
#include "include/stb_truetype.h"

#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"

#include "types.h"
#include "math.h"
#include "utility.h"
#include "memory.h"
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

enum Render_Type
{
    RECT,
};
struct Render_Element
{
    Render_Type type;
    void *data;
};
struct Render_Rect
{
    V3 cen;
    V2 dim;
    V4 color;
};

static void
push_rect(V3 cen, V2 dim, V4 color,
          std::vector<Render_Element> &batch, Memory_Arena *arena)
{
    Render_Element e;
    e.type = Render_Type::RECT;
    e.data = push_struct(arena, Render_Rect);

    Render_Rect *data = (Render_Rect *)e.data;
    data->cen = cen;
    data->dim = dim;
    data->color = color;

    batch.push_back(e);
}

#define GET_UNIFORM_LOCATION(PROGRAM, NAME)\
    PROGRAM.NAME = glGetUniformLocation(PROGRAM.id, #NAME);
struct Rect_Program
{
    s32 id;

    s32 P;
};
Rect_Program rect_program;

struct UI_ID
{
    s32 idx;
};
struct UI_Context
{
    UI_ID hot;
    UI_ID active;
};

int main()
{
    if (glfwInit())
    {
        glfwWindowHint(GLFW_SAMPLES, 1);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow *window = glfwCreateWindow(1280, 720, "IMGUI", 0, 0);
        if (window)
        {
            glfwMakeContextCurrent(window);

            glewExperimental = true; // Needed for core profile
            
            // @TEMPORARY
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_TEXTURE_2D);

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

                    // Ensure we can capture the escape key being pressed below
                    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

                    GLuint vertex_array_id;
                    glGenVertexArrays(1, &vertex_array_id);
                    glBindVertexArray(vertex_array_id);

                    // @TODO: this sucks.
                    rect_program.id = LoadShaders("shader/simple.vs", "shader/simple.fs");
                    GET_UNIFORM_LOCATION(rect_program, P);

                    GLuint vertexbuffer;
                    glGenBuffers(1, &vertexbuffer);
                    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

                    std::vector<Render_Element> render_batch;

                    b32 should_close = 0;

                    //
                    // Application Loop
                    //
                    while (!should_close)
                    {
                        //
                        // Process inputs.
                        //
                        if (glfwGetKey(window, GLFW_KEY_ESCAPE) ||
                            glfwWindowShouldClose(window))
                        {
                            should_close = true;
                        }

                        //
                        // Simulate
                        //
                        s32 iW, iH;
                        f32 W, H;
                        glfwGetWindowSize(window, &iW, &iH);
                        W = (f32)iW;
                        H = (f32)iH;

                        f32 a = 1.0f / W;
                        f32 b = 1.0f / H;
                        M4x4 orthographic_projection = M4x4{{
                            a, 0, 0, 0,
                            0, b, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1,
                        }};
                        push_rect(V3{0, 0, 0}, V2{500, 500}, V4{1, 0, 1, 1},
                                  render_batch, &transient_state->transient_arena);


                        //
                        // Render
                        //
                        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
                        glClear(GL_COLOR_BUFFER_BIT);

                        for (Render_Element &element : render_batch)
                        {
                            if (element.type == Render_Type::RECT)
                            {
                                Render_Rect *data = (Render_Rect *)element.data;

                                Vertex Vs[4] = {
                                    {data->cen + V3{+data->dim.x * 0.5f, -data->dim.y * 0.5f, 0}, data->color},
                                    {data->cen + V3{+data->dim.x * 0.5f, +data->dim.y * 0.5f, 0}, data->color},
                                    {data->cen + V3{-data->dim.x * 0.5f, -data->dim.y * 0.5f, 0}, data->color},
                                    {data->cen + V3{-data->dim.x * 0.5f, +data->dim.y * 0.5f, 0}, data->color},
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
