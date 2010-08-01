#version 110

uniform float normalFlip;

varying vec3 normal;
varying vec3 pos;

void main()
{
    normal = normalize(gl_NormalMatrix * gl_Normal).xyz * normalFlip;
	pos = gl_Vertex.xyz;
    
	gl_Position = ftransform();
}
