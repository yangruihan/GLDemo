#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

out vec4 varyingColor;

void main()
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}