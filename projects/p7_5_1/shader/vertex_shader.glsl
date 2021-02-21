#version 410 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 vertNormal;

out vec2 tc;
out vec4 varingColor;

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

void main()
{
    tc = texCoord;

    // 将顶点位置转换到视觉空间
    // 将法向量转换到视觉空间
    // 计算视觉空间光照向量（从顶点到光源）
    vec4 P = mv_matrix * vec4(vertPos, 1.0);
    vec3 N = normalize((norm_matrix * vec4(vertNormal, 1.0)).xyz);
    vec3 L = normalize(light.position - P.xyz);

    // 视觉向量等于视觉空间中的负顶点位置
    vec3 V = normalize(-P.xyz);

    // R 是 -L 的相对于表面向量 N 的镜像
    vec3 R = reflect(-L, N);

    // 环境光、漫反射和镜面反射分量
    vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(dot(N, L), 0.0);
    vec3 specular = material.specular.xyz * light.specular.xyz * pow(max(dot(R, V), 0.0), material.shininess);

    // 将计算好的光照颜色输出到片段着色器
    varingColor = vec4((ambient + diffuse + specular), 1.0);

    // 计算位置信息
    gl_Position = proj_matrix * mv_matrix * vec4(vertPos, 1.0);
}