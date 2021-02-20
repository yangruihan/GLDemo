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

GLuint mvLoc, projLoc;
float aspect;

mat4 pMat, vMat, mMat, mvMat;

GLuint brickTexture;

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
    combinePath(SOURCE_PATH, "/res/brick1.jpg", &tpath);

    RLOG_INFO("Start loading texture: %s", tpath);
    brickTexture = loadTexture(tpath);
    if (!brickTexture)
    {
        FREE(char, tpath);
        RLOG_INFO("Load texture failed");
        return;
    }
        
    RLOG_INFO("Load texture success, id: %d", brickTexture);

    FREE(char, tpath);

    RLOG_DEBUG("End Init");
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

    // 设置变量
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (float *)mvMat);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)pMat);

    CHECK_OPENGL_ERROR();

    // 设置顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, brickTexture);

    // 绘制
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glDrawElements(GL_TRIANGLES, idxCount, GL_UNSIGNED_INT, 0);

    CHECK_OPENGL_ERROR();
}
