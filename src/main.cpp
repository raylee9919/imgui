#include <stdio.h>
#include <stdlib.h>

#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"

int main()
{
    if (!glfwInit())
    {
    }
    else
    {
        fprintf(stderr, "ERROR: Couldn't init glfw.\n");
    }
}
