#version 110

varying vec3 cubeTexNormal;

void main()
{
	gl_Position = ftransform();
    cubeTexNormal = (gl_Vertex * gl_ModelViewMatrix).xyz;
}
