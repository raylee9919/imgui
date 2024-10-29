#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>


#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"

#include "types.h"
#include "shader.cpp"

int main()
{
    if (glfwInit())
    {
        glfwWindowHint(GLFW_SAMPLES, 1);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow *window = glfwCreateWindow( 1024, 768, "IMGUI", 0, 0);
        if (window)
        {
            glfwMakeContextCurrent(window);

            glewExperimental = true; // Needed for core profile
            if (glewInit() == GLEW_OK) 
            {
                // Ensure we can capture the escape key being pressed below
                glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

                // Dark blue background
                glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

                GLuint vertex_array_id;
                glGenVertexArrays(1, &vertex_array_id);
                glBindVertexArray(vertex_array_id);

                // @TODO: this sucks.
                GLuint programID = LoadShaders("shader/simple.vs", "shader/simple.fs");

                const GLfloat g_vertex_buffer_data[] = 
                { 
                    -1.0f, -1.0f, 0.0f,
                    1.0f, -1.0f, 0.0f,
                    0.0f,  1.0f, 0.0f,
                };

                GLuint vertexbuffer;
                glGenBuffers(1, &vertexbuffer);
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

                for (;;)
                {
                    //
                    // Process inputs.
                    //
                    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
                    {
                        glfwWindowShouldClose(window);
                        break;
                    }



                    // Clear the screen
                    glClear( GL_COLOR_BUFFER_BIT );

                    // Use our shader
                    glUseProgram(programID);

                    // 1rst attribute buffer : vertices
                    glEnableVertexAttribArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                    glVertexAttribPointer(
                                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                                          3,                  // size
                                          GL_FLOAT,           // type
                                          GL_FALSE,           // normalized?
                                          0,                  // stride
                                          (void*)0            // array buffer offset
                                         );

                    // Draw the triangle !
                    glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

                    glDisableVertexAttribArray(0);

                    // Swap buffers
                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }

                // Cleanup VBO
                glDeleteBuffers(1, &vertexbuffer);
                glDeleteVertexArrays(1, &vertex_array_id);
                glDeleteProgram(programID);

                // Close OpenGL window and terminate GLFW
                glfwTerminate();
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
