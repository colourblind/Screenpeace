#version 110

varying float z;

void main()
{
	gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    z = (gl_ModelViewMatrix * gl_Vertex).z;
}
