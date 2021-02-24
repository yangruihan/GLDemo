#include "fcommon.h"

#include <math.h>
#include <cglm/cglm.h>

#include "fconfig.h"
#include "framework.h"

#define numVAOs 2
#define numVBOs 8

float cameraX, cameraY, cameraZ;
float tLocX, tLocY, tLocZ;
float mLocX, mLocY, mLocZ;

GLuint renderingProgram;
GLuint renderingProgram2;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

float aspect;

GLuint mvLoc, projLoc, nLoc, sLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;

mat4 pMat, vMat, mMat, mvMat, invTrMat;

vec3 origin = (vec3){0.0f, 0.0f, 0.0f};
vec3 up     = (vec3){0.0f, 1.0f, 0.0f};

vec3 currentLightPos, lightPosV; // 模型和视图空间中光照位置
float lightPos[3]; // 光照位置数组

// 初始化光照位置
vec3 initialLigthLoc = (vec3){-3.8f, 2.2f, 1.1f};

// 全局白光特性
float globalAmbient[4] = {0.7f, 0.7f, 0.7f, 1.0f};
float lightAmbient[4]  = {0.0f, 0.0f, 0.0f, 1.0f};
float lightDiffuse[4]  = {1.0f, 1.0f, 1.0f, 1.0f};
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GLuint whiteTex;

// 黄金材质特性
float* goldMatAmb;
float* goldMatDif;
float* goldMatSpe;
float  goldMatShi;

// 青铜材质特性
float* bronzeMatAmb;
float* bronzeMatDif;
float* bronzeMatSpe;
float  bronzeMatShi;

Torus t;
Model m;
int idxCount1, idxCount2;

// 阴影相关变量
int screenSizeX, screenSizeY;
GLuint shadowTex, shadowBuffer;
mat4 lightVMatrix;
mat4 lightPMatrix;
mat4 shadowMVP1;
mat4 shadowMVP2;
mat4 b = (mat4){
    {0.5f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.5f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.5f, 0.0f},
    {0.5f, 0.5f, 0.5f, 1.0f}
};

/**
 * 初始化 Shader
 */
static void setupShader()
{
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

    combinePath(SOURCE_PATH, "/shader/shadow_tex_vert.glsl", &vpath);
    combinePath(SOURCE_PATH, "/shader/shadow_tex_frag.glsl", &fpath);

    RLOG_INFO("VShader path: %s", vpath);
    RLOG_INFO("FShader path: %s", fpath);

    if (!createShaderProgram(vpath, fpath, &renderingProgram2))
    {
        RLOG_ERROR("createShaderProgram2 Failed!");

        FREE(char, vpath);
        FREE(char, fpath);
        return;
    }

    FREE(char, vpath);
    FREE(char, fpath);

    CHECK_OPENGL_ERROR();

    RLOG_INFO("Program2 Id: %d", renderingProgram);
}

/**
 * 初始化顶点
 */
static void setupVertices()
{
    Torus* tp = &t;
    torus_init(tp, 0.5f, 0.2f, 48);

    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    // 环面顶点数据

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

    idxCount1 = t.base.indices.count;

    torus_free(tp);

    // 模型顶点数据

    Model* mp = &m;
    model_init(mp);
    char* modelPath;
    combinePath(SOURCE_PATH, "/res/test.obj", &modelPath);
    if (!model_load(mp, modelPath))
    {
        FREE(char, modelPath);
        return;
    }

    FREE(char, modelPath);

    glBindVertexArray(vao[1]);

    // 顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m.vertices.count, m.vertices.data, GL_STATIC_DRAW);

    // 纹理坐标
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * m.texCoords.count, m.texCoords.data, GL_STATIC_DRAW);

    // 法向量
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m.normals.count, m.normals.data, GL_STATIC_DRAW);

    // 索引
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m.indices.count, m.indices.data, GL_STATIC_DRAW);

    idxCount2 = m.indices.count;

    model_free(mp);
}

/**
 * 初始化阴影贴图
 */
static void setupShadowBuffers(GLFWwindow* window)
{
    glfwGetFramebufferSize(window, &screenSizeX, &screenSizeY);

    // 创建自定义帧缓冲区
    glGenFramebuffers(1, &shadowBuffer);

    // 创建阴影纹理并让它存储深度信息
    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, screenSizeX, screenSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
}

void init(GLFWwindow *window)
{
    RLOG_DEBUG("Start Init");

    tLocX =  1.6f; tLocY = 0.0f; tLocZ = -0.3f;
    mLocX = -1.0f; mLocY = 0.1f; mLocZ =  0.3f;
    cameraX = 0.0f; cameraY = 0.2f; cameraZ = 6.0f;

    goldMatAmb = material_goldAmbient();
    goldMatDif = material_goldDiffuse();
    goldMatSpe = material_goldSpecular();
    goldMatShi = material_goldShininess();

    bronzeMatAmb = material_bronzeAmbient();
    bronzeMatDif = material_bronzeDiffuse();
    bronzeMatSpe = material_bronzeSpecular();
    bronzeMatShi = material_bronzeShininess();

    // 初始化 Shader
    setupShader();

    // 配置顶点
    setupVertices();

    // 配置阴影Buffer
    setupShadowBuffers(window);

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

static void installLights(GLuint renderingProgram, mat4 vMat)
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
    glProgramUniform4fv(renderingProgram, mAmbLoc,      1, goldMatAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc,     1, goldMatDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc,     1, goldMatSpe);
    glProgramUniform1f(renderingProgram,  mShiLoc,         goldMatShi);
}

static void passOne()
{
    glUseProgram(renderingProgram2);
    CHECK_OPENGL_ERROR();

    // 通过从光源角度渲染环面获得深度缓冲区
    glm_translate(mMat, (vec3){tLocX, tLocY, tLocZ});
    // 轻微旋转以便查看
    glm_rotate(mMat, deg2Rad(25.0f), V3_RIGHT);

    // 从光源角度绘制，使用光源P、V矩阵
    glm_mul(lightVMatrix, mMat, shadowMVP1);
    glm_mul(lightPMatrix, shadowMVP1, shadowMVP1);
    int loc = glGetUniformLocation(renderingProgram2, "shadowMVP");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*) shadowMVP1);

    // 绘制环面

    glBindVertexArray(vao[0]);

    // 第一轮中，我们只需要顶点数据，而不需要纹理和法向量
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glDrawElements(GL_TRIANGLES, idxCount1, GL_UNSIGNED_INT, 0);

    CHECK_OPENGL_ERROR();

    // 绘制模型

    glBindVertexArray(vao[1]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
    glDrawElements(GL_TRIANGLES, idxCount2, GL_UNSIGNED_INT, 0);

    CHECK_OPENGL_ERROR();
}

static void passTwo()
{
    glUseProgram(renderingProgram);

    // 绘制环面，这次需要加上光照、材质、法向量等
    // 同时需要为相机空间和光照空间都提供MVP变换
    mvLoc   = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    nLoc    = glGetUniformLocation(renderingProgram, "norm_matrix");
    sLoc    = glGetUniformLocation(renderingProgram, "shadowMVP");

    glm_mat4_identity(vMat);
    glm_translate(vMat, (vec3){-cameraX, -cameraY, -cameraZ});

    glm_vec3_copy(lightPos, currentLightPos);
    installLights(renderingProgram, vMat);

    // ----------
    // 绘制环面
    // ---------

    glm_mat4_identity(mMat);
    glm_translate(mMat, (vec3){tLocX, tLocY, tLocZ});
    // 轻微旋转以便查看
    glm_rotate(mMat, deg2Rad(25.0f), V3_RIGHT);

    // 构建相机视角环面的MV矩阵
    glm_mul(vMat, mMat, mvMat);
    glm_mat4_inv(mvMat, invTrMat);
    glm_mat4_transpose(invTrMat);

    // 构建光源视角环面的MV矩阵
    glm_mul(lightVMatrix, mMat, shadowMVP2);
    glm_mul(lightPMatrix, shadowMVP2, shadowMVP2);
    glm_mul(b, shadowMVP2, shadowMVP2);

    // 将 MV 及 Proj 矩阵传入相应变量
    glUniformMatrix4fv(mvLoc,   1, GL_FALSE, (float*)mvMat);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)pMat);
    glUniformMatrix4fv(nLoc,    1, GL_FALSE, (float*)invTrMat);
    glUniformMatrix4fv(sLoc,    1, GL_FALSE, (float*)shadowMVP2);

    glBindVertexArray(vao[0]);

    // 初始化环面顶点数据
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
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    CHECK_OPENGL_ERROR();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glDrawElements(GL_TRIANGLES, idxCount1, GL_UNSIGNED_INT, 0);
    CHECK_OPENGL_ERROR();


    // ----------
    // 绘制模型
    // ----------

    glm_mat4_identity(mMat);
    glm_translate(mMat, (vec3){mLocX, mLocY, mLocZ});
    // 轻微旋转以便查看
    glm_rotate(mMat, deg2Rad(25.0f), V3_RIGHT);

    // 构建相机视角环面的MV矩阵
    glm_mul(vMat, mMat, mvMat);
    glm_mat4_inv(mvMat, invTrMat);
    glm_mat4_transpose(invTrMat);

    // 构建光源视角环面的MV矩阵
    glm_mul(lightVMatrix, mMat, shadowMVP2);
    glm_mul(lightPMatrix, shadowMVP2, shadowMVP2);
    glm_mul(b, shadowMVP2, shadowMVP2);

    // 将 MV 及 Proj 矩阵传入相应变量
    glUniformMatrix4fv(mvLoc,   1, GL_FALSE, (float*)mvMat);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)pMat);
    glUniformMatrix4fv(nLoc,    1, GL_FALSE, (float*)invTrMat);
    glUniformMatrix4fv(sLoc,    1, GL_FALSE, (float*)shadowMVP2);

    glBindVertexArray(vao[1]);

    // 初始化环面顶点数据
    // 设置顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, whiteTex);

    // 绘制
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[7]);
    glDrawElements(GL_TRIANGLES, idxCount1, GL_UNSIGNED_INT, 0);

    CHECK_OPENGL_ERROR();
}

void display(GLFWwindow *window, double time)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    CHECK_OPENGL_ERROR();

    // 从光源视角初始化视觉矩阵以及透视矩阵，以便在第一轮使用
    glm_lookat(currentLightPos, origin, up, lightVMatrix); // 从光源到原点的矩阵
    glm_perspective(deg2Rad(60.0f), aspect, 0.1f, 1000.0f, lightPMatrix);

    // 使用自定义帧缓冲区，将阴影纹理附着到其上
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);

    CHECK_OPENGL_ERROR();

    // 关闭绘制颜色，同时开启深度计算
    glDrawBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);

    CHECK_OPENGL_ERROR();

    passOne();

    // 使用显示缓冲区，并重新开启绘制
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glDrawBuffer(GL_FRONT); // 重新开启绘制颜色

    passTwo();
}
