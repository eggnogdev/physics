#version 440 core

layout(location = 0) in vec3 position;
layout(binding = 0) buffer ModelToWorldMatrixBuffer {
    mat4 modelToWorldMatrices[];
};

layout (binding = 1) buffer ModelRadiusBuffer {
    float modelRadii[];
};

uniform mat4 worldToCameraMatrix;
uniform mat4 cameraToScreenMatrix;

out vec3 vertexPosition;
out float modelRadius;

void main() {
    int currentCircleIndex = gl_VertexID / 6;
    mat4 modelToWorldMatrix = modelToWorldMatrices[currentCircleIndex];  
    modelRadius = modelRadii[currentCircleIndex];

    gl_Position = cameraToScreenMatrix * worldToCameraMatrix * modelToWorldMatrix * vec4(position, 1.0);
    vertexPosition = position;
}
