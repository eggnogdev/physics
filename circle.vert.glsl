#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToCameraMatrix;
uniform mat4 cameraToScreenMatrix;

out vec3 vertexPosition;

void main() {
    gl_Position = cameraToScreenMatrix * worldToCameraMatrix * modelToWorldMatrix * vec4(position, 1.0);
    vertexPosition = position;
}
