#ifndef __RGLU_SHADER_H__
#define __RGLU_SHADER_H__

#include "./common.h"
#include "./util.h"

bool createShaderProgram(const char *vp, const char *fp, /* out */ GLuint *program);

#endif // __RGLU_SHADER_H__