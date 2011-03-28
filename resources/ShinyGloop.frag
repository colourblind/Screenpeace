#version 110

uniform samplerCube environ;

varying vec3 cubeTexNormal;

void main()
{
    gl_FragColor = textureCube(environ, cubeTexNormal);
}
