#version 410 core

in vec4 varyingColor;

out vec4 color;

void main()
{
    color = varyingColor;
}