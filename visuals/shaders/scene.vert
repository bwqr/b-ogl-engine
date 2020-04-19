#version 330 core

layout(location = 0) in vec3 inPosition;

uniform mat4 camera;

out vec3 texDir;

void main() {
    gl_Position = (camera * vec4(inPosition, 1.0)).xyww;
    texDir = inPosition;
}