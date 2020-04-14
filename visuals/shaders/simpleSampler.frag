#version 330 core

in vec2 fragTexCoord;

out vec4 FragColor;

uniform sampler2D texture2D;

void main() {
    FragColor = texture(texture2D, fragTexCoord);
}