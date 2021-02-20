#ifndef __RGLU_SHAPE_BASE_H__
#define __RGLU_SHAPE_BASE_H__

#include "../rglu_common.h"

typedef struct ShapeBase
{
    Array indices;
    Array vertices;
    Array texCoords;
    Array normals;
} ShapeBase;

#endif // __RGLU_SHAPE_BASE_H__
