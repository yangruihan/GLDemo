#ifndef __RGLU_UTIL_H__
#define __RGLU_UTIL_H__

#include "./rglu_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool printShaderLog(GLuint shader);
bool printProgramLog(int program);

#define CHECK_OPENGL_ERROR() checkOpenGLError(__FILE__, __LINE__)
bool checkOpenGLError(const char* file, int line);

#ifdef __cplusplus
}
#endif

#endif // __RGLU_UTIL_H__