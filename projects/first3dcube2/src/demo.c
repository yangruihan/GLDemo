#include "fcommon.h"

#include <math.h>

#include <cglm/cglm.h>

#include "fconfig.h"
#include "framework.h"

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc;
float aspect;

mat4 pMat, vMat, mMat, mvMat;

void setupVertices()
{
    float vertexPositions[108] = {
        -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f};

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    CHECK_OPENGL_ERROR();

    glGenBuffers(numVBOs, vbo);

    CHECK_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

    CHECK_OPENGL_ERROR();
}

void Init(GLFWwindow *window)
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

    CHECK_OPENGL_ERROR();

    RLOG_INFO("Program Id: %d", renderingProgram);

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
    cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;

    setupVertices();

    RLOG_DEBUG("End Init");
}

void Display(GLFWwindow *window, double time)
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

    // 构建视图矩阵、模型矩阵和视图-模型矩阵
    mat4 tMat, rMat;
    glm_mat4_identity(tMat);
    glm_mat4_identity(rMat);
    glm_translate(tMat, (vec3){sin(0.35f * time) * 2.0f, cos(0.52f * time) * 2.0f, sin(0.7f * time) * 2.0f});
    glm_rotate(rMat, 1.75f * (float) time, (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(rMat, 1.75f * (float) time, (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(rMat, 1.75f * (float) time, (vec3){0.0f, 0.0f, 1.0f});
    glm_mat4_mul(tMat, rMat, mMat);

    glm_mat4_identity(vMat);
    glm_translate(vMat, (vec3){-cameraX, -cameraY, -cameraZ});
    glm_mat4_mul(vMat, mMat, mvMat);

    // 设置变量
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (float *)&mvMat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)&pMat[0][0]);

    CHECK_OPENGL_ERROR();

    // 设置顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    CHECK_OPENGL_ERROR();

    // 绘制
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    CHECK_OPENGL_ERROR();
}
