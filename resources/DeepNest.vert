#version 110

uniform vec3 cameraPos;

varying vec3 normal;
varying vec3 pos;

void main()
{
	gl_Position = ftransform();
    normal = normalize(gl_NormalMatrix * gl_Normal).xyz;
	pos = (gl_ModelViewMatrix * gl_Vertex).xyz + cameraPos;    
}
