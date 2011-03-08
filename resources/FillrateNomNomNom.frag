#version 110

varying float light;

uniform sampler2D texture;

void main()
{
    gl_FragColor = texture2D(texture, gl_TexCoord[0].st) * light * vec4(1, 1, 0, 1);
}

