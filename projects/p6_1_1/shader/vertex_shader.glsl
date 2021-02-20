#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

out vec2 tc;

void main()
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    tc = texCoord;
}