#version 460

uniform float elapsedTime;

layout(location = 0) in vec3 vertexPosition; 

out vec2 fragPos; 

void main() {
    // Calculate the scaling factor based on the cosine of elapsedTime
    float scaleFactor = cos(elapsedTime); 

    // Scale the vertex position by the scaleFactor
    vec3 scaledVertexPosition = vertexPosition * scaleFactor;

    // Set the position of the vertex
    gl_Position = vec4(scaledVertexPosition, 1.0); 
    
    // Pass the scaled position to the fragment shader
    fragPos = scaledVertexPosition.xy; 
}
