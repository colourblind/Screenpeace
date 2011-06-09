#version 110

uniform sampler2D texture;
uniform float textureWidth;

void main()
{
    float s = 1.0 / textureWidth;
    vec4 frag = vec4(0.0);
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(-4.0 * s, 0.0)) * 0.01;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(-3.0 * s, 0.0)) * 0.03;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(-2.0 * s, 0.0)) * 0.11;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(-1.0 * s, 0.0)) * 0.22;
    frag += texture2D(texture, gl_TexCoord[0].st) * 0.26;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(1.0 * s, 0.0)) * 0.22;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(2.0 * s, 0.0)) * 0.11;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(3.0 * s, 0.0)) * 0.03;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(4.0 * s, 0.0)) * 0.01;
    gl_FragColor = frag;
}
