#include "shape/rglu_torus.h"

#include "rglu_math.h"

static void init(Torus* t)
{
    int prec = t->prec;
    int numVertices = (prec + 1) * (prec + 1);
    int numIndices = prec * prec * 6;

    // 初始化数组
    ARR_INIT_CAP(&t->base.indices,  int, numIndices);
    ARR_INIT_CAP(&t->base.vertices, vec3, numVertices);
    ARR_INIT_CAP(&t->base.texCoords,vec2, numVertices);
    ARR_INIT_CAP(&t->base.normals,  vec3, numVertices);
    ARR_INIT_CAP(&t->sTangents,     vec3, numVertices);
    ARR_INIT_CAP(&t->tTangents,     vec3, numVertices);

    for (int i = 0; i < numIndices; i++)
    {
        ARR_PUSH(&t->base.indices, int, 0);
    }

    for (int i = 0; i < numVertices; i++)
    {
        ARR_PUSH(&t->base.vertices,  vec3, ((vec3){0, 0, 0}));
        ARR_PUSH(&t->base.texCoords, vec2, ((vec2){0, 0}));
        ARR_PUSH(&t->base.normals,   vec3, ((vec3){0, 0, 0}));
        ARR_PUSH(&t->sTangents,      vec3, ((vec3){0, 0, 0}));
        ARR_PUSH(&t->tTangents,      vec3, ((vec3){0, 0, 0}));
    }

    mat4 rMat;
    vec4 v4;
    vec3 v3, v31, v32;
    vec2 v2;

    // 计算第一个环
    for (int i = 0; i <= prec; i++)
    {
        float amt = deg2Rad(i * 360.0f / prec);

        // 绕原点旋转点，形成环，然后将它们向外移动
        glm_mat4_identity(rMat);
        glm_rotate(rMat, amt, (vec3){0.0f, 0.0f, 1.0f});
        glm_mat4_mulv(rMat, (vec4){t->outer, 0.0f, 0.0f, 1.0f}, v4);
        glm_vec3_add((vec3){v4[0], v4[1], v4[2]}, (vec3){t->inner, 0.0f, 0.0f}, v3);

        array_set(&t->base.vertices, i, &v3);

        // 计算纹理坐标
        v2[0] = 0.0f;
        v2[1] = (float) i / (float) prec;
        array_set(&t->base.texCoords, i, &v2);

        // 计算切向量和法向量
        glm_mat4_mulv(rMat, (vec4){0.0f, -1.0f, 0.0f, 1.0f}, v4);
        glm_vec3(v4, v3);
        array_set(&t->tTangents, i, &v3);
        v31[0] = 0.0f; v31[1] = 0.0f; v31[2] = -1.0f;
        array_set(&t->sTangents, i, &v31);
        glm_cross(v3, v31, v32);
        array_set(&t->base.normals, i, &v32);
    }

    // 绕Y轴旋转最初的那个环，形成其他的环
    for (int ring = 1; ring < prec + 1; ring++)
    {
        for (int vert = 0; vert < prec + 1; vert++)
        {
            // 绕Y轴旋转最初的环的顶点坐标
            float amt = deg2Rad(ring * 360.0f / prec);

            glm_mat4_identity(rMat);
            glm_rotate(rMat, amt, (vec3){0.0f, 1.0f, 0.0f});

            array_get(&t->base.vertices, vert, &v3);
            glm_mat4_mulv(rMat, (vec4){v3[0], v3[1], v3[2], 1.0f}, v4);
            glm_vec3(v4, v31);
            array_set(&t->base.vertices, ring * (prec + 1) + vert, &v31);

            // 计算新顶点的纹理坐标
            array_get(&t->base.texCoords, vert, &v2);
            v2[0] = (float) ring * 2.0f / (float) prec;

            if (v2[0] > 1.0f)
                v2[0] -= 1.0f;

            array_set(&t->base.texCoords, ring * (prec + 1) + vert, &v2);

            // 计算新的切向量和副切向量
            glm_mat4_identity(rMat);
            glm_rotate(rMat, amt, (vec3){0.0f, 1.0f, 0.0f});
            array_get(&t->sTangents, vert, &v3);
            glm_mat4_mulv(rMat, (vec4){v3[0], v3[1], v3[2], 1.0f}, v4);
            glm_vec3(v4, v31);
            array_set(&t->sTangents, ring * (prec + 1) + vert, &v31);

            array_get(&t->tTangents, vert, &v3);
            glm_mat4_mulv(rMat, (vec4){v3[0], v3[1], v3[2], 1.0f}, v4);
            glm_vec3(v4, v31);
            array_set(&t->tTangents, ring * (prec + 1) + vert, &v31);

            // 绕Y轴旋转法向量
            glm_mat4_identity(rMat);
            glm_rotate(rMat, amt, (vec3){0.0f, 1.0f, 0.0f});
            array_get(&t->base.normals, vert, &v3);
            glm_mat4_mulv(rMat, (vec4){v3[0], v3[1], v3[2], 1.0f}, v4);
            glm_vec3(v4, v31);
            array_set(&t->base.normals, ring * (prec + 1) + vert, &v31);
        }
    }

    int idx;
    // 计算三角形索引
    for (int ring = 0; ring < prec; ring++)
    {
        for (int vert = 0; vert < prec; vert++)
        {
            idx = ring * (prec + 1) + vert;
            array_set(&t->base.indices, ((ring * prec + vert) * 2) * 3 + 0, &idx);
            idx = (ring + 1) * (prec + 1) + vert;
            array_set(&t->base.indices, ((ring * prec + vert) * 2) * 3 + 1, &idx);
            idx = ring * (prec + 1) + vert + 1;
            array_set(&t->base.indices, ((ring * prec + vert) * 2) * 3 + 2, &idx);
            idx = ring * (prec + 1) + vert + 1;
            array_set(&t->base.indices, ((ring * prec + vert) * 2 + 1) * 3 + 0, &idx);
            idx = (ring + 1) * (prec + 1) + vert;
            array_set(&t->base.indices, ((ring * prec + vert) * 2 + 1) * 3 + 1, &idx);
            idx = (ring + 1) * (prec + 1) + vert + 1;
            array_set(&t->base.indices, ((ring * prec + vert) * 2 + 1) * 3 + 2, &idx);
        }
    }
}

void torus_init(Torus* t, float innerR, float outerR, int prec)
{
    R_ASSERT(t);

    t->inner = innerR;
    t->outer = outerR;
    t->prec = prec;
    init(t);
}

void torus_free(Torus* t)
{
    R_ASSERT(t);

    array_free(&t->base.indices);
    array_free(&t->base.vertices);
    array_free(&t->base.texCoords);
    array_free(&t->base.normals);

    array_free(&t->sTangents);
    array_free(&t->tTangents);
}
