#include "fcommon.h"

#include <math.h>

#include <cglm/cglm.h>

#include "fconfig.h"
#include "framework.h"

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc, projLoc;
float aspect;

MatStack matstack;

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
        1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
    };

    float pyramidPositions[54] = {
        -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 前面
        1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 右面
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 后面
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 左面
        -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, // 底面1
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, // 底面2
    };

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    CHECK_OPENGL_ERROR();

    glGenBuffers(numVBOs, vbo);

    CHECK_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

    CHECK_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
}

static void windowSizeCallback(GLFWwindow* window, int newW, int newH)
{
    // 构建透视矩阵
    aspect = (float) WIDTH / (float) HEIGHT;
    glm_perspective(1.0472f, aspect, 0.1f, 1000.0f, pMat); // 1.0472 radians = 60 degrees
}

void init(GLFWwindow *window)
{
    RLOG_DEBUG("Start Init");

    // 监听事件
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    // 初始化矩阵堆栈
    matstack_init(&matstack);

    // 加载 Shader
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

    // 设置摄像机位置
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 15.0f;

    // 设置用到的模型顶点
    setupVertices();

    // 构建透视矩阵
    aspect = (float) WIDTH / (float) HEIGHT;
    glm_perspective(1.0472f, aspect, 0.1f, 1000.0f, pMat); // 1.0472 radians = 60 degrees

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

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *) pMat);

    CHECK_OPENGL_ERROR();

    glm_mat4_identity(vMat);
    glm_translate(vMat, (vec3){-cameraX, -cameraY, -cameraZ});
    matstack_push(&matstack, vMat);

    mat4 tempMat;

    // ----- 渲染金字塔代表的太阳 -----
    matstack_pushTop(&matstack);

    // 位置
    glm_mat4_identity(tempMat);
    glm_translate(tempMat, (vec3){0.0f, 0.0f, 0.0f});
    matstack_mul(&matstack, tempMat);
    matstack_pushTop(&matstack);

    // 旋转
    glm_mat4_identity(tempMat);
    glm_rotate(tempMat, (float) time, (vec3){1.0f, 0.0f, 0.0f});
    matstack_mul(&matstack, tempMat);

    // 渲染
    mat4 data;
    matstack_top(&matstack, data);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (float*) data);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    matstack_pop(&matstack, NULL);

    CHECK_OPENGL_ERROR();

    // ----- 渲染立方体代表地球 -----
    matstack_pushTop(&matstack);

    // 位置
    glm_mat4_identity(tempMat);
    glm_translate(tempMat, (vec3){sin((float) time * 2.0) * 5, 0.0f, cos((float) time * 2.0) * 5});
    matstack_mul(&matstack, tempMat);
    matstack_pushTop(&matstack);

    // 旋转
    glm_mat4_identity(tempMat);
    glm_rotate(tempMat, (float) time, (vec3){0.0f, 1.0f, 0.0f});
    matstack_mul(&matstack, tempMat);

    matstack_top(&matstack, data);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (float*) data);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    matstack_pop(&matstack, NULL);

    CHECK_OPENGL_ERROR();

    // ----- 渲染小立方体代表月球 -----
    matstack_pushTop(&matstack);
    glm_mat4_identity(tempMat);
    glm_translate(tempMat, (vec3){0.0f, sin((float) time * 2.0f) * 3, cos((float) time * 2.0f) * 3});
    matstack_mul(&matstack, tempMat);
    glm_mat4_identity(tempMat);
    glm_rotate(tempMat, (float) time, (vec3){0.0f, 0.0f, 1.0f});
    matstack_mul(&matstack, tempMat);
    glm_mat4_identity(tempMat);
    glm_scale(tempMat, (vec3){0.25f, 0.25f, 0.25f});
    matstack_mul(&matstack, tempMat);

    matstack_top(&matstack, data);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (float*) data);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    matstack_pop(&matstack, NULL);

    CHECK_OPENGL_ERROR();

    matstack_pop(&matstack, NULL);
    matstack_pop(&matstack, NULL);
    matstack_pop(&matstack, NULL);
}
