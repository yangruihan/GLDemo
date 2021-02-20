#ifndef __RGLU_SHADER_H__
#define __RGLU_SHADER_H__

#include "./rglu_common.h"
#include "./rglu_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool createShaderProgram(const char *vp, const char *fp, /* out */ GLuint *program);

#ifdef __cplusplus
}
#endif

#endif // __RGLU_SHADER_H__