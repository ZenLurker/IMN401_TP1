#version 460

uniform float elapsedTimeMS;

layout(location = 0) in vec3 vertexPosition; 

out vec2 fragPos; 

void main() {
    vec3 dynamicVertexPosition = vertexPosition;
    dynamicVertexPosition.x *= cos(elapsedTimeMS);
    dynamicVertexPosition.y *= cos(elapsedTimeMS);

    gl_Position = vec4(dynamicVertexPosition, 1.0); 
    fragPos = dynamicVertexPosition.xy; 
}