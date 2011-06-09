#version 110

uniform sampler2D texture;
uniform float focalDistance;
uniform float focalRange;

varying float z;

void main()
{
    gl_FragColor = texture2D(texture, gl_TexCoord[0].st);
    gl_FragColor.a = clamp(abs(-z - focalDistance) / focalRange, 0.0, 1.0);

}
