#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

void main()
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}