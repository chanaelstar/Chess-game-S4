#version 330 core

in vec3 vNormal;
in vec3 vFragPos;

uniform vec3  uColor;
uniform int   uPlayerMode; // 0 = tour des blancs, 1 = tour des noirs
uniform float uTime;

out vec4 fragColor;

vec3 diffuse(vec3 lightPos, vec3 lightColor, float intensity)
{
    vec3 dir  = normalize(lightPos - vFragPos);
    float d   = max(dot(normalize(vNormal), dir), 0.0);
    return lightColor * d * intensity;
}

void main()
{
    vec3 result;

    if (uPlayerMode == 0)
    {
        // --- Tour des Blancs ---
        // Lumière 1 : soleil chaud fixe, haut-droite
        vec3 sunPos   = vec3(4.0, 10.0, 3.0);
        vec3 sunColor = vec3(1.0, 0.92, 0.75);

        // Lumière 2 : halo bleu froid qui orbite autour du plateau
        vec3 orbitPos   = vec3(cos(uTime * 0.7) * 8.0, 4.0, sin(uTime * 0.7) * 8.0);
        vec3 orbitColor = vec3(0.45, 0.65, 1.0);

        float ambient = 0.25;
        result = uColor * (ambient
                 + diffuse(sunPos,   sunColor,   0.65)
                 + diffuse(orbitPos, orbitColor, 0.30));
    }
    else
    {
        // --- Tour des Noirs ---
        // Lumière 1 : lumière froide latérale fixe
        vec3 coldPos   = vec3(-6.0, 4.0, -2.0);
        vec3 coldColor = vec3(0.35, 0.45, 0.9);

        // Lumière 2 : brasier rouge/orange bas qui tourne rapidement
        vec3 firePos   = vec3(cos(uTime * 1.5) * 5.0, 0.6, sin(uTime * 1.5) * 5.0);
        vec3 fireColor = vec3(1.0, 0.22, 0.04);

        float ambient = 0.07;
        result = uColor * (ambient
                 + diffuse(coldPos, coldColor, 0.50)
                 + diffuse(firePos, fireColor, 0.55));
    }

    fragColor = vec4(result, 1.0);
}
