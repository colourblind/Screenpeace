#version 120

uniform mat4 cameraMat;

varying vec3 cubeTexNormal;

void main()
{
	gl_Position = ftransform();
    vec3 view = normalize(gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 dir = mat3(cameraMat) * view;
    vec3 normal = mat3(cameraMat) * normalize(gl_NormalMatrix * gl_Normal);
    cubeTexNormal = reflect(view, normal);
}
