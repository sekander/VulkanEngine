#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    vec4 colour; // You can keep or discard this if unused
    mat4 normalMat; // If you're using it
    int maxLights;
    vec3 rimLightColour;
    float rimLightStrength;
} push;

void main() {
    outColor = push.colour; // White color for simplicity
    // outColor = vec4(fragColor, 1.0); // White color for simplicity
}