#version 410 core

in vec2 tc;               // 纹理坐标
in vec3 varingNormal;     // 视觉空间顶点法向量
in vec3 varingLightDir;   // 指向光源的向量
in vec3 varingVertPos;    // 视觉空间中的顶点位置
in vec3 varingHalfVector; // 角平分向量

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

void main()
{
    // 正规化光照向量、法向量、视觉向量
    vec3 L = normalize(varingLightDir);
    vec3 N = normalize(varingNormal);
    vec3 V = normalize(-varingVertPos);
    vec3 H = normalize(varingHalfVector);

    // 计算光照向量基于 N 的反射向量
    vec3 R = normalize(reflect(-L, N));
    // 计算光照与平面法向量间的角度
    float cosTheta = dot(L, N);
    // 计算视觉向量与反射光向量的角度
    float cosPhi = dot(H, N);

    // 计算 ADS 分量（按像素），并合并以构建输出颜色
    vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
    vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess * 3);        
        
    color = texture(samp, tc) * vec4((ambient + diffuse + specular), 1.0);
}