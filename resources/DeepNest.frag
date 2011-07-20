#version 110

uniform vec3 lightPos;
uniform vec3 colour;

varying vec3 pos;
varying vec3 normal;

void main()
{
    vec3 ambient = 0.2 * colour;
    vec3 diffuse = max(0.0, dot(normal, normalize(pos - lightPos))) * colour;
    
    gl_FragColor.rgb = clamp(ambient.rgb + diffuse.rgb, 0.0, 1.0);
    gl_FragColor.a = 1.0;
}
