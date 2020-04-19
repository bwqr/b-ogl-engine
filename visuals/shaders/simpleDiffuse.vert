#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 camera;
uniform mat4 model;
uniform vec4 diffuseColor;

out vec4 fragDiffuseColor;

void main() {
    gl_Position = camera * model * vec4(inPos, 1.0);
    fragDiffuseColor = diffuseColor;
}