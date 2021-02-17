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

    CHECK_OPENGL_ERROR();

    RLOG_INFO("Program Id: %d", renderingProgram);

    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 30.0f;
    cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;

    setupVertices();

    RLOG_DEBUG("End Init");
}

void display(GLFWwindow *window, double time)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    CHECK_OPENGL_ERROR();
    
    glUseProgram(renderingProgram);

    CHECK_OPENGL_ERROR();

    GLuint mLoc = glGetUniformLocation(renderingProgram, "m_matrix");
    GLuint vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
    GLuint projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    GLuint tfLoc = glGetUniformLocation(renderingProgram, "tf");

    CHECK_OPENGL_ERROR();

    // 构建透视矩阵
    aspect = (float) WIDTH / (float) HEIGHT;
    glm_perspective(1.0472f, aspect, 0.1f, 1000.0f, pMat); // 1.0472 radians = 60 degrees

    glm_mat4_identity(mMat);
    glm_mat4_identity(vMat);
    glm_translate(vMat, (vec3){-cameraX, -cameraY, -cameraZ});

    // 设置变量
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, (float *)mMat);
    glUniformMatrix4fv(vLoc, 1, GL_FALSE, (float *)vMat);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)pMat);
    glUniform1f(tfLoc, (float) time * 0.3);

    CHECK_OPENGL_ERROR();

    // 设置顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    CHECK_OPENGL_ERROR();

    // 绘制
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100);

    CHECK_OPENGL_ERROR();
}
