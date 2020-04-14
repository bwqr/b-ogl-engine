#version 330 core

layout(location = 0) in vec3 inPosition;

struct CameraBufferObject {
    mat4 view;
    mat4 proj;
};

uniform CameraBufferObject camera;

out vec3 texDir;

void main() {
    gl_Position = (camera.proj * camera.view * vec4(inPosition, 1.0)).xyww;
    texDir = inPosition;
}