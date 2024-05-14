#version 460

in vec2 fragPos; 

out vec4 FragColor; 

vec3 posToRGB(vec2 pos) {
    // Calculate the hue angle based on the position
    float a = 0.5 * atan(pos.y, pos.x) / 3.14159;
    if (a < 0.0)
        a += 1.0;

    // Calculate the distance from the origin
    float l = length(pos);

    // Calculate RGB color based on the hue and distance
    vec3 rgb = clamp(abs(mod(a * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);

    // Mix between white (1.0, 1.0, 1.0) and calculated RGB color based on the distance
    return mix(vec3(1.0), rgb, l);
}

void main() {
    vec3 color = posToRGB(fragPos);

    FragColor = vec4(color, 1.0);
}