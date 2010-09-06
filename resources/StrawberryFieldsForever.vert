#version 110

uniform vec2 offset;
uniform float normalModifier;
uniform float offsetModifier;

varying vec3 diffuse;

void main()
{
    vec3 normal = normalize(vec3(offset * normalModifier, 1.0));
    diffuse = (dot(normal, vec3(0.0, 0.0, 1.0))) * vec3(1.0, 0.0, 0.0);
    
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (gl_Vertex + vec4(offset * offsetModifier, 0.0, 0.0));
}
