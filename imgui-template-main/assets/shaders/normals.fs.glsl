#version 330 core

in vec3 vPosition_vs; 
in vec3 vNormal_vs; 
in vec2 vTexCoords;

out vec3 fragmentColor;

void main() {
    
    vec3 normal = normalize(vNormal_vs);
    
    fragmentColor = normal;
}