#ifndef __RGLU_MATH_H__
#define __RGLU_MATH_H__

#include "./rglu_common.h"

#define PI 3.14159f

#ifdef __cplusplus
extern "C"
{
#endif

#define V3_UP      (vec3){ 0.0f,  1.0f,  0.0f}
#define V3_RIGHT   (vec3){ 1.0f,  0.0f,  0.0f}
#define V3_FORWARD (vec3){ 0.0f,  0.0f,  1.0f}
#define V3_DOWN    (vec3){ 0.0f, -1.0f,  0.0f}
#define V3_LEFT    (vec3){-1.0f,  0.0f,  0.0f}
#define V3_BACK    (vec3){ 0.0f,  0.0f, -1.0f}

const float deg2Rad(const float deg);
const float rad2Deg(const float rad);

#ifdef __cplusplus
}
#endif

#endif // __RGLU_MATH_H__
