#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

uniform mat4 lightSpaceMatrix;
uniform mat4 M;

out vec2 UV;

void main() {
    UV = vertexUV;

    gl_Position = lightSpaceMatrix * M * vec4(vertexPosition_modelspace, 1.0);
}

