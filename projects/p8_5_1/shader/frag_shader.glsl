#version 410 core

in vec2 tc;               // 纹理坐标
in vec3 varingNormal;     // 视觉空间顶点法向量
in vec3 varingLightDir;   // 指向光源的向量
in vec3 varingVertPos;    // 视觉空间中的顶点位置
in vec3 varingHalfVector; // 角平分向量
in vec4 shadow_coord;

out vec4 color;

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
uniform sampler2D samp;
uniform sampler2DShadow shTex;

void main()
{
    // 正规化光照向量、法向量、视觉向量
    vec3 L = normalize(varingLightDir);
    vec3 N = normalize(varingNormal);
    vec3 V = normalize(-varingVertPos);
    vec3 H = normalize(varingHalfVector);

    float notInShadow = textureProj(shTex, shadow_coord);

    // 计算光照向量基于 N 的反射向量
    vec3 R = normalize(reflect(-L, N));
    // 计算光照与平面法向量间的角度
    float cosTheta = dot(L, N);
    // 计算视觉向量与反射光向量的角度
    float cosPhi = dot(H, N);

    color = texture(samp, tc) * (globalAmbient * material.ambient + light.ambient * material.ambient);

    if (notInShadow == 1.0)
    {
        color += light.diffuse * material.diffuse * max(dot(L, N), 0.0)
                 + light.specular * material.specular
                 * pow(max(dot(H, N), 0.0), material.shininess * 3.0);
    }
}