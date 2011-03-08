#version 110

varying float light;

uniform vec3 lightPos0;
uniform vec3 lightPos1;
uniform vec3 lightPos2;

void main()
{
    light = 0.0;
    light = dot(vec3(0, 0, 1), normalize(gl_Vertex.xyz - lightPos0));
    light += dot(vec3(0, 0, 1), normalize(gl_Vertex.xyz - lightPos1));
    light += dot(vec3(0, 0, 1), normalize(gl_Vertex.xyz - lightPos2));
    light *= 0.5;
    clamp(light, 0.0, 1.0);
    
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
