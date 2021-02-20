#include "rglu_util.h"

bool printShaderLog(GLuint shader)
{
    int len = 0;
    int chWrittn = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shader, len, &chWrittn, log);
        printf("[GLError] Shader Info: %s\n", log);
        free(log);
        return true;
    }
    return false;
}

bool printProgramLog(int program)
{
    int len = 0;
    int chWrittn = 0;
    char *log;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0)
    {
        log = (char *)malloc(len);
        glGetProgramInfoLog(program, len, &chWrittn, log);
        printf("[GLError] Program Info: %s\n", log);
        free(log);
        return true;
    }
    return false;
}

bool checkOpenGLError(const char* file, int line)
{
    GLenum status = glGetError();
    char* errorInfo = NULL;
    switch (status)
    {
        case GL_NO_ERROR:
            return false;

        case GL_INVALID_ENUM:
            errorInfo = "INVALID_ENUM";
            break;

        case GL_INVALID_VALUE:
            errorInfo = "INVALID_VALUE";
            break;

        case GL_INVALID_OPERATION:
            errorInfo = "INVALID_OPERATION";
            break;

        case GL_STACK_OVERFLOW:
            errorInfo = "STACK_OVERFLOW";
            break;

        case GL_STACK_UNDERFLOW:
            errorInfo = "STACK_UNDERFLOW";
            break;

        case GL_OUT_OF_MEMORY:
            errorInfo = "OUT_OF_MEMORY";
            break;

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorInfo = "INVALID_FRAMEBUFFER_OPERATION";
            break;
    }

    printf("[GLError] %s %d: %s\n", file, line, errorInfo);
    return true;
}