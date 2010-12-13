#version 110

varying vec3 normal;

void main()
{
    float diffuse = dot(normal, normalize(vec3(-0.5, 0.5, 0)));
    gl_FragColor.rgb = vec3(1.0 - diffuse, 1.0 - diffuse, 1.0 - diffuse);
    gl_FragColor.a = 1.0;
}
