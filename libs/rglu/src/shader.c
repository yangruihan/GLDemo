#include "shader.h"

static char *readShaderSource(const char *filePath)
{
    char *content;
    int fileSize;
    if (readFile(filePath, &content, &fileSize))
        return content;
    return NULL;
}

bool createShaderProgram(const char *vp, const char *fp, /* out */ GLuint *program)
{
    char* vc = readShaderSource(vp);
    if (!vc) return false;

    char* fc = readShaderSource(fp);
    if (!fc) return false;

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    R_ASSERT(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    R_ASSERT(fShader);

    glShaderSource(vShader, 1, (const GLchar *const*)&vc, NULL);
    glCompileShader(vShader);
    if (printShaderLog(vShader))
    {
        FREE(char, vc);
        FREE(char, fc);
        return false;
    }

    CHECK_OPENGL_ERROR();

    glShaderSource(fShader, 1, (const GLchar *const*)&fc, NULL);
    glCompileShader(fShader);
    if (printShaderLog(fShader))
    {
        FREE(char, vc);
        FREE(char, fc);
        return false;
    }

    CHECK_OPENGL_ERROR();

    FREE(char, vc);
    FREE(char, fc);

    GLuint p = glCreateProgram();
    R_ASSERT(p);
    glAttachShader(p, vShader);
    glAttachShader(p, fShader);
    glLinkProgram(p);
    if (printProgramLog(p))
        return false;

    CHECK_OPENGL_ERROR();

    *program = p;

    return true;
}