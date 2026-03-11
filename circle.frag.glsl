#version 330 core

in vec3 vertexPosition;

uniform float modelRadius;

out vec4 FragColor;

void main() {
    if (length(vertexPosition) > modelRadius) discard;
    FragColor = vec4(1.0);
}
