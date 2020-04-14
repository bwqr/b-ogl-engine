#version 330 core

in vec4 fragDiffuseColor;

out vec4 fragColor;

void main() {
    fragColor = fragDiffuseColor;
}