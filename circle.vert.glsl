#version 440 core

layout(location = 0) in vec3 screenBasedVertexPosition;

uniform mat4 worldBasedModelTransform;
uniform mat4 viewMatrix;

out vec3 modelBasedFragmentPosition;

void main() {
    gl_Position = viewMatrix * worldBasedModelTransform * vec4(screenBasedVertexPosition, 1.0);
    modelBasedFragmentPosition = screenBasedVertexPosition;
}
