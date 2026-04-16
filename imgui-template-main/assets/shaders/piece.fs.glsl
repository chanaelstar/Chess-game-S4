#version 330 core

in vec3 vNormal;

uniform vec3 uColor;

out vec4 fragColor;

void main()
{
    vec3 lightDir = normalize(vec3(2.0, 5.0, 3.0));
    float ambient = 0.35;
    float diffuse = max(dot(normalize(vNormal), lightDir), 0.0) * 0.65;
    fragColor = vec4(uColor * (ambient + diffuse), 1.0);
}
