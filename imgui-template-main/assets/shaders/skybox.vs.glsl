#version 330 core
layout(location = 0) in vec3 aPosition;

uniform mat4 uViewProj; // proj * mat3(view) — translation supprimée

out vec3 vDirection;

void main()
{
    vDirection  = aPosition;
    vec4 pos    = uViewProj * vec4(aPosition, 1.0);
    gl_Position = pos.xyww; // force depth = 1.0 pour que la skybox soit toujours derrière
}
