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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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

    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        RLOG_INFO("current platform support gl debug");
    else
        RLOG_INFO("current platform not support gl debug");

    glfwSwapInterval(1);

    init(window);

    while (!glfwWindowShouldClose(window))
    {
        display(window, glfwGetTime());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
