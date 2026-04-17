#version 330 core

in vec2 vTexCoord;

uniform vec3      uColor;
uniform sampler2D uTexture;
uniform bool      uUseTexture;
uniform int       uLightMode; // 0=tour des blancs, 1=tour des noirs

out vec4 fragColor;

void main()
{
    vec4 base;
    if (uUseTexture)
        base = texture(uTexture, vTexCoord) * vec4(uColor, 1.0);
    else
        base = vec4(uColor, 1.0);

    // Mode noir : plateau assombri pour l'ambiance dramatique
    float brightness = (uLightMode == 0) ? 1.0 : 0.45;
    fragColor = base * brightness;
}
