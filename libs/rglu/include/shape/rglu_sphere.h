#ifndef __RGLU_SPERE_H__
#define __RGLU_SPERE_H__

#include "../rglu_common.h"

#include "./rglu_shapebase.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Sphere
{
    ShapeBase base;
} Sphere;

void sphere_init(Sphere* s, int prec);
void sphere_free(Sphere* s);
const int sphere_getVerticesNum(Sphere* s);
const int sphere_getIndicesNum(Sphere* s);

#ifdef __cplusplus
}
#endif

#endif // __RGLU_SPERE_H__
