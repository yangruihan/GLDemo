#include "framework.h"

int main()
{
    if (!glfwInit())
    {
        RLOG_ERROR("glfwInit Failed!");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, DEMO_TITLE, NULL, NULL);

    if (!window)
    {
        RLOG_ERROR("glfwCreateWindow Failed!");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        RLOG_ERROR("gladLoadGL Failed!");
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1);

    Init(window);

    while (!glfwWindowShouldClose(window))
    {
        Display(window, glfwGetTime());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}