#version 330 core
in  vec3 vDirection;
out vec4 fragColor;

void main()
{
    float t      = clamp(normalize(vDirection).y * 0.5 + 0.5, 0.0, 1.0);
    vec3  top    = vec3(0.05, 0.10, 0.50); 
    vec3  horiz  = vec3(0.60, 0.75, 0.95); 
    vec3  ground = vec3(0.25, 0.18, 0.10); 

    vec3  sky    = mix(horiz, top, t);
    float below  = clamp(-normalize(vDirection).y * 0.5 + 0.5, 0.0, 1.0);
    fragColor    = vec4(mix(sky, ground, below * below), 1.0);
}
