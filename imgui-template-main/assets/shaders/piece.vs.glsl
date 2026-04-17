#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;

out vec3 vNormal;
out vec3 vFragPos;

void main()
{
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
    vNormal   = mat3(transpose(inverse(uModelMatrix))) * aNormal;
    vFragPos  = vec3(uModelMatrix * vec4(aPosition, 1.0));
}
