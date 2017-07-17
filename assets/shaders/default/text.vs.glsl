#version 330 core

layout(location = 0) in vec3 i_Position;
layout(location = 1) in vec2 i_TexCoords;
varying vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(i_Position, 1);
    v_TexCoord = i_TexCoords;
}
