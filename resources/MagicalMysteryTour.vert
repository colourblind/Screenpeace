#version 110

varying vec3 normal;

void main()
{
    normal = normalize(gl_NormalMatrix * gl_Normal).xyz;
	gl_Position = ftransform();
}
