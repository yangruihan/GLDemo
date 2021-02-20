#include "shape/rglu_sphere.h"

#include "rglu_math.h"

static void init(Sphere* s, int prec)
{
    int numVertices = (prec + 1) * (prec + 1);
    int numIndices = prec * prec * 6;

    // 初始化数组
    ARR_INIT_CAP(&s->base.indices,  int, numIndices);
    ARR_INIT_CAP(&s->base.vertices, vec3, numVertices);
    ARR_INIT_CAP(&s->base.texCoords,vec2, numVertices);
    ARR_INIT_CAP(&s->base.normals,  vec3, numVertices);

    for (int i = 0; i < numIndices; i++)
    {
        ARR_PUSH(&s->base.indices, int, 0);
    }

    for (int i = 0; i < numVertices; i++)
    {
        ARR_PUSH(&s->base.vertices,  vec3, ((vec3){0, 0, 0}));
        ARR_PUSH(&s->base.texCoords, vec2, ((vec2){0, 0}));
        ARR_PUSH(&s->base.normals,   vec3, ((vec3){0, 0, 0}));
    }

    // 计算三角形顶点
    for (int i = 0; i <= prec; i++)
    {
        for (int j = 0; j <= prec; j++)
        {
            float y =  cosf(deg2Rad(180.0f - i * 180.0f / prec));
            float x = -cosf(deg2Rad(j * 360.0f / prec)) * fabs(cosf(asinf(y)));
            float z =  sinf(deg2Rad(j * 360.0f / prec)) * fabs(cosf(asinf(y)));

            vec3 v = (vec3){x, y, z};
            vec2 v2 = (vec2){(float) j / prec, (float) i / prec};
            array_set(&s->base.vertices, i * (prec + 1) + j, &v);
            array_set(&s->base.texCoords, i * (prec + 1) + j, &v2);
            array_set(&s->base.normals, i * (prec + 1) + j, &v);
        }
    }

    // 计算三角形索引
    for (int i = 0; i < prec; i++)
    {
        for (int j = 0; j < prec; j++)
        {
            ARR_SET(&s->base.indices, int, 6 * (i * prec + j) + 0, i * (prec + 1) + j);
            ARR_SET(&s->base.indices, int, 6 * (i * prec + j) + 1, i * (prec + 1) + j + 1);
            ARR_SET(&s->base.indices, int, 6 * (i * prec + j) + 2, (i + 1) * (prec + 1) + j);
            ARR_SET(&s->base.indices, int, 6 * (i * prec + j) + 3, i * (prec + 1) + j + 1);
            ARR_SET(&s->base.indices, int, 6 * (i * prec + j) + 4, (i + 1) * (prec + 1) + j + 1);
            ARR_SET(&s->base.indices, int, 6 * (i * prec + j) + 5, (i + 1) * (prec + 1) + j);
        }
    }
}

void sphere_init(Sphere* s, int prec)
{
    R_ASSERT(s);

    init(s, prec);
}

void sphere_free(Sphere* s)
{
    R_ASSERT(s);

    array_free(&s->base.indices);
    array_free(&s->base.vertices);
    array_free(&s->base.texCoords);
    array_free(&s->base.normals);
}

const int sphere_getVerticesNum(Sphere* s)
{
    R_ASSERT(s);

    return s->base.vertices.count;
}

const int sphere_getIndicesNum(Sphere* s)
{
    R_ASSERT(s);

    return s->base.indices.count;
}
