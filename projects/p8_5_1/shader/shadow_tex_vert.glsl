#version 410 core
in vec3 vertPos;

uniform mat4 shadowMVP;

void main()
{
    gl_Position = shadowMVP * vec4(vertPos, 1.0);
}
