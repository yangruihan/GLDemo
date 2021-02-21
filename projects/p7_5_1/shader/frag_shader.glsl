#version 410 core

in vec2 tc;
in vec4 varingColor;

out vec4 color;

uniform sampler2D samp;

void main()
{
    color = texture(samp, tc) * varingColor;
}