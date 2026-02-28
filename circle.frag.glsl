#version 440 core

in vec3 modelBasedFragmentPosition;

uniform float modelRadius;

out vec4 FragColor;

void main() {
    /* if (length(modelBasedFragmentPosition) > modelRadius) discard; */
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
