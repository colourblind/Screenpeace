#version 110

uniform sampler2D texture;
uniform float textureHeight;

void main()
{
    float s = 1.0 / textureHeight;
    vec4 frag = vec4(0.0);
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, -4.0 * s)) * 0.01;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, -3.0 * s)) * 0.03;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, -2.0 * s)) * 0.11;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, -1.0 * s)) * 0.22;
    frag += texture2D(texture, gl_TexCoord[0].st) * 0.26;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, 1.0 * s)) * 0.22;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, 2.0 * s)) * 0.11;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, 3.0 * s)) * 0.03;
    frag += texture2D(texture, gl_TexCoord[0].st + vec2(0.0, 4.0 * s)) * 0.01;
    gl_FragColor = frag;
}
