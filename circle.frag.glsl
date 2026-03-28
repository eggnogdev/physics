#version 440 core

in vec3 vertexPosition;
in float modelRadius;

out vec4 FragColor;

void main() {
    if (length(vertexPosition) > modelRadius) discard;
    FragColor = vec4(1.0);
}
