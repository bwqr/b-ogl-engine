#version 330 core

in vec3 texDir;

out vec4 FragColor;

uniform samplerCube cubemap;

void main() {
    FragColor = texture(cubemap, texDir);
//    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}