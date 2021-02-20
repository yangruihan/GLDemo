#ifndef __RGLU_TORUS_H__
#define __RGLU_TORUS_H__

#include "../rglu_common.h"

#include "./rglu_shapebase.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Torus
{
    ShapeBase base;
    int prec; // 精度
    float inner;
    float outer;
    Array sTangents;
    Array tTangents;
} Torus;

void torus_init(Torus* t, float innerR, float outerR, int prec);
void torus_free(Torus* t);

#ifdef __cplusplus
}
#endif

#endif // __RGLU_TORUS_H__
