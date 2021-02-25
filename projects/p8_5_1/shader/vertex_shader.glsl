#version 410 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 vertNormal;

out vec2 tc;               // 纹理坐标
out vec3 varingNormal;     // 视觉空间顶点法向量
out vec3 varingLightDir;   // 指向光源的向量
out vec3 varingVertPos;    // 视觉空间中的顶点位置
out vec3 varingHalfVector; // 角平分向量
out vec4 shadow_coord;

struct PositionalLight
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;
uniform mat4 norm_matrix;
uniform mat4 shadowMVP2;

void main()
{
    // 纹理坐标
    tc = texCoord;

    // 输出顶点位置、光照方向和法向量到光栅器以进行插值
    varingVertPos = (mv_matrix * vec4(vertPos, 1.0)).xyz;
    varingLightDir = (light.position - varingVertPos);
    varingNormal = (norm_matrix * vec4(vertNormal, 1.0)).xyz;
    varingHalfVector = (varingLightDir + (-varingVertPos)).xyz;

    shadow_coord = shadowMVP2 * vec4(vertPos, 1.0);

    // 计算位置信息
    gl_Position = proj_matrix * mv_matrix * vec4(vertPos, 1.0);
}