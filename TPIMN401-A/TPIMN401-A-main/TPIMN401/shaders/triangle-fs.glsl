#version 460

out vec4 FragColor;

uniform vec3 triangleColor;

void main() {
    FragColor = vec4(triangleColor, 1.0);
}