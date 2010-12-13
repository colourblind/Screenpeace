#version 110

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    vec4 frag = vec4(0.0);
    frag = texture2D(texture0, gl_TexCoord[0].st) * (1.0 - texture2D(texture1, vec2(1.0, 1.0) - gl_TexCoord[0].st).r);
    gl_FragColor = frag;
}
