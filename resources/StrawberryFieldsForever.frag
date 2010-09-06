#version 110

varying vec3 diffuse;

void main()
{
    gl_FragColor = vec4(diffuse, 1.0);
}
