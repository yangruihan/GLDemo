#version 410 core

in vec2 tc;

out vec4 color;

uniform sampler2D samp;

void main()
{
    color = texture(samp, tc);
}