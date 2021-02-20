#ifndef __RGLU_MODLE_H__
#define __RGLU_MODLE_H__

#include "./rglu_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct Model
{
    Array vertices;
    Array indices;
    Array texCoords;
    Array normals;
    Array tangents;
} Model;

void model_init(Model* m);
void model_free(Model* m);

bool model_load(Model* m, const char* path);

#ifdef __cplusplus
}
#endif

#endif // __RGLU_MODLE_H__
