#include "rglu_model.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

typedef C_STRUCT aiScene aiScene;
typedef C_STRUCT aiNode aiNode;
typedef C_STRUCT aiMesh aiMesh;
typedef C_STRUCT aiFace aiFace;

void model_init(Model* m)
{
    R_ASSERT(m);

    ARR_INIT_CAP(&m->vertices,  vec3, 0);
    ARR_INIT_CAP(&m->indices,   int,  0);
    ARR_INIT_CAP(&m->texCoords, vec2, 0);
    ARR_INIT_CAP(&m->normals,   vec3, 0);
    ARR_INIT_CAP(&m->tangents,  vec3, 0);
}

void model_free(Model* m)
{
    R_ASSERT(m);

    array_free(&m->vertices);
    array_free(&m->indices);
    array_free(&m->texCoords);
    array_free(&m->normals);
    array_free(&m->tangents);
}

static void process_mesh(Model* m, aiMesh* mesh, const aiScene* s)
{
    vec3 v3;
    vec2 v2;

    for (int i = 0, count = mesh->mNumVertices; i < count; i++)
    {
        // 顶点坐标
        v3[0] = mesh->mVertices[i].x;
        v3[1] = mesh->mVertices[i].y;
        v3[2] = mesh->mVertices[i].z;
        array_push(&m->vertices, &v3);

        // 法线向量
        v3[0] = mesh->mNormals[i].x;
        v3[1] = mesh->mNormals[i].y;
        v3[2] = mesh->mNormals[i].z;
        array_push(&m->normals, &v3);

        // 纹理坐标
        if (mesh->mTextureCoords[0])
        {
            v2[0] = mesh->mTextureCoords[0][i].x;
            v2[1] = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            v2[0] = 0.0f; v2[1] = 0.0f;
        }
        array_push(&m->texCoords, &v2);

        // 切线向量
        v3[0] = mesh->mTangents[i].x;
        v3[1] = mesh->mTangents[i].y;
        v3[2] = mesh->mTangents[i].z;
        array_push(&m->tangents, &v3);
    }

    // 索引
    int idx;
    for (int i = 0, count = mesh->mNumFaces; i < count; i++)
    {
        const aiFace face = mesh->mFaces[i];
        for (int j = 0, count = face.mNumIndices; j < count; j++)
        {
            idx = face.mIndices[j];
            array_push(&m->indices, &idx);
        }
    }
}

static void process_node(Model* m, aiNode* n, const aiScene* s)
{
    for (int i = 0, count = n->mNumMeshes; i < count; i++)
    {
        aiMesh* mesh = s->mMeshes[n->mMeshes[i]];
        process_mesh(m, mesh, s);
    }

    for (int i = 0, count = n->mNumChildren; i < count; i++)
    {
        aiNode* node = n->mChildren[i];
        process_node(m, node, s);
    }
}

bool model_load(Model* m, const char* path)
{
    const aiScene* s = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
    if (!s)
        return false;

    if (s->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !s->mRootNode)
        return false;

    process_node(m, s->mRootNode, s);

    return true;
}
