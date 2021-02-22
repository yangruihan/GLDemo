#include "fcommon.h"
#include <math.h>

#include <cglm/cglm.h>

#include "fconfig.h"
#include "framework.h"

#define numVAOs 1
#define numVBOs 4

float cameraX, cameraY, cameraZ;
float locX, locY, locZ;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

float aspect;

GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;

mat4 pMat, vMat, mMat, mvMat, invTrMat;

vec3 currentLightPos, lightPosV; // 模型和视图空间中光照位置
float lightPos[3]; // 光照位置数组

// 初始化光照位置
vec3 initialLigthLoc = (vec3){5.0f, 2.0f, 2.0f};

// 全局白光特性
float globalAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f};
float lightAmbient[4]  = {0.0f, 0.0f, 0.0f, 1.0f};
float lightDiffuse[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

// 黄金材质特性
float* matAmb;
float* matDif;
float* matSpe;
float  matShi;

GLuint whiteTex;

Torus t;
int idxCount;

void setupVertices()
{
    Torus* tp = &t;
    torus_init(tp, 0.5f, 0.2f, 48);

    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    // 顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * t.base.vertices.count, t.base.vertices.data, GL_STATIC_DRAW);

    // 纹理坐标
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * t.base.texCoords.count, t.base.texCoords.data, GL_STATIC_DRAW);

    // 法向量
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * t.base.normals.count, t.base.normals.data, GL_STATIC_DRAW);

    // 索引
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * t.base.indices.count, t.base.indices.data, GL_STATIC_DRAW);

    idxCount = t.base.indices.count;

    torus_free(tp);
}

void init(GLFWwindow *window)
{
    RLOG_DEBUG("Start Init");

    matAmb = material_goldAmbient();
    matDif = material_goldDiffuse();
    matSpe = material_goldSpecular();
    matShi = material_goldShininess();

    char *vpath;
    char *fpath;

    combinePath(SOURCE_PATH, "/shader/vertex_shader.glsl", &vpath);
    combinePath(SOURCE_PATH, "/shader/frag_shader.glsl", &fpath);

    RLOG_INFO("VShader path: %s", vpath);
    RLOG_INFO("FShader path: %s", fpath);

    if (!createShaderProgram(vpath, fpath, &renderingProgram))
    {
        RLOG_ERROR("createShaderProgram Failed!");

        FREE(char, vpath);
        FREE(char, fpath);
        return;
    }

    FREE(char, vpath);
    FREE(char, fpath);

    CHECK_OPENGL_ERROR();

    RLOG_INFO("Program Id: %d", renderingProgram);

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 3.0f;
    locX = 0.0f; locY = 0.0f; locZ = 0.0f;

    // 配置顶点
    setupVertices();

    char *tpath;
    combinePath(SOURCE_PATH, "/res/white.png", &tpath);

    RLOG_INFO("Start loading texture: %s", tpath);
    whiteTex = loadTexture(tpath);
    if (!whiteTex)
    {
        FREE(char, tpath);
        RLOG_INFO("Load texture failed");
        return;
    }

    RLOG_INFO("Load texture success, id: %d", whiteTex);

    FREE(char, tpath);

    RLOG_DEBUG("End Init");
}

static void installLights(mat4 vMat)
{
    // 将光源位置转换为视图控件坐标
    glm_mat4_mulv3(vMat, currentLightPos, 1.0f, lightPosV);
    glm_vec3_copy(lightPosV, (float*) lightPos);

    // 从着色器中获取光源位置和材质属性
    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");

    ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    specLoc = glGetUniformLocation(renderingProgram, "light.specular");
    posLoc = glGetUniformLocation(renderingProgram, "light.position");

    mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
    mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

    // 为着色器中变量赋值
    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(renderingProgram, ambLoc,       1, lightAmbient);
    glProgramUniform4fv(renderingProgram, diffLoc,      1, lightDiffuse);
    glProgramUniform4fv(renderingProgram, specLoc,      1, lightSpecular);
    glProgramUniform3fv(renderingProgram, posLoc,       1, lightPos);
    glProgramUniform4fv(renderingProgram, mAmbLoc,      1, matAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc,     1, matDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc,     1, matSpe);
    glProgramUniform1f(renderingProgram,  mShiLoc,         matShi);
}

void display(GLFWwindow *window, double time)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    CHECK_OPENGL_ERROR();

    glUseProgram(renderingProgram);

    CHECK_OPENGL_ERROR();

    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

    if (!mvLoc)
        RLOG_ERROR("mv_matrix uniform not found!");

    if (!projLoc)
        RLOG_ERROR("proj_matrix uniform not found!");

    if (!nLoc)
        RLOG_ERROR("norm_matrix uniform not found!");

    CHECK_OPENGL_ERROR();

    // 构建透视矩阵
    aspect = (float) WIDTH / (float) HEIGHT;
    glm_perspective(1.0472f, aspect, 0.1f, 1000.0f, pMat); // 1.0472 radians = 60 degrees

    glm_mat4_identity(vMat);
    glm_translate(vMat, (vec3){-cameraX, -cameraY, -cameraZ});

    // -------------------------------
    // 渲染对象
    // -------------------------------

    // 构建视图矩阵、模型矩阵和视图-模型矩阵
    glm_mat4_identity(mMat);
    glm_translate(mMat, (vec3){locX, locY, locZ});
    glm_rotate(mMat, (float) time, (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(mMat, (float) time, (vec3){0.0f, 0.0f, 1.0f});
    glm_rotate(mMat, (float) time, (vec3){1.0f, 0.0f, 0.0f});
    glm_mul(vMat, mMat, mvMat);

    // 基于当前光源位置，初始化光照
    glm_vec3_copy(initialLigthLoc, currentLightPos);
    installLights(vMat);

    glm_mat4_inv(mvMat, invTrMat);
    glm_mat4_transpose(invTrMat);

    // 设置变量
    glUniformMatrix4fv(mvLoc,   1, GL_FALSE, (float*) mvMat);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*) pMat);
    glUniformMatrix4fv(nLoc,    1, GL_FALSE, (float*) invTrMat);

    CHECK_OPENGL_ERROR();

    // 设置顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, whiteTex);

    // 绘制
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glDrawElements(GL_TRIANGLES, idxCount, GL_UNSIGNED_INT, 0);

    CHECK_OPENGL_ERROR();
}
