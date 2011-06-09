#version 110

uniform sampler2D texture;
uniform sampler2D blurred;

void main()
{
    vec4 texColour = texture2D(texture, gl_TexCoord[0].st);
    vec4 blurColour = texture2D(blurred, gl_TexCoord[0].st);
    gl_FragColor = texColour + texColour.a * (blurColour - texColour);
}
