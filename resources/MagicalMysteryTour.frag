#version 110

varying vec3 normal;

const vec3 foreground = vec3(0.0, 0.0, 0.0);
const vec3 background = vec3(1.0, 1.0, 1.0);

void main()
{
    float alpha = 1.0;
    gl_FragColor.rgb = mix(background, foreground, alpha);
    gl_FragColor.a = 1.0;
}
