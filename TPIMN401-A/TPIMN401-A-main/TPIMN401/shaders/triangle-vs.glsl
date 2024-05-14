#version 460

layout(location = 0) in vec3 vertexPosition; 

out vec2 fragPos; 

void main() {
    gl_Position = vec4(vertexPosition, 1.0); 
    fragPos = vertexPosition.xy; 
}