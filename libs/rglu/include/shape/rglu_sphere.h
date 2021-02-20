#ifndef __RGLU_SPERE_H__
#define __RGLU_SPERE_H__

#include "../rglu_common.h"

typedef struct Sphere
{
    Array indices;
    Array vertices;
    Array texCoords;
    Array normals;
} Sphere;

void sphere_init(Sphere* s, int prec);
void sphere_free(Sphere* s);
const int sphere_getVerticesNum(Sphere* s);
const int sphere_getIndicesNum(Sphere* s);

#endif // __RGLU_SPERE_H__
