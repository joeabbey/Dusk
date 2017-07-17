#version 330 core

varying vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main()
{
    vec4 c = u_Color * texture(u_Texture, v_TexCoord).r;
    gl_FragColor = vec4(c.r, c.g, c.b, c.a);
}
