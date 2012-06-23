#version 110

uniform vec3 lightPos;
uniform sampler2D texture;

varying vec3 colour;
varying vec3 pos;
varying vec3 normal;

void main()
{
    vec3 ambient = vec3(0.2);
    vec3 diffuse = vec3(abs(dot(normal, normalize(lightPos - pos))));
    
    gl_FragColor.rgb = texture2D(texture, gl_TexCoord[0].st).rgb * clamp(ambient.rgb + diffuse.rgb, 0.0, 1.0) * colour;
    gl_FragColor.a = gl_FragCoord.z;
}
