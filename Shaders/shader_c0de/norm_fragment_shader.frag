#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in vec3 fragPosWorld;
layout(location = 6) in vec3 normalWS;

// layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;
layout(set = 1, binding = 1) uniform sampler2D normalMap;

layout(push_constant) uniform PushConstants {
    vec4 colour; // You can keep or discard this if unused
    mat4 normalMat; // If you're using it
    int maxLights;
    vec3 rimLightColour;
    float rimLightStrength;
} push;

void main() {

    //Testing colour 
    // outColor = vec4(fragColor, 1.0);

    // outColor = vec4(color, 1.0);

    //Testing out normal map
    // outColor = vec4(normalTangent, 1.0);

    //Testing texture
    // outColor = vec4(fragColor, 1.0) * texture(textureSampler, texCoords);
    
    // outColor = vec4(normalize(normalWS) * 0.5 + 0.5, 1.0);
    outColor = vec4(push.colour.rgb, 1.0); // Red normals
}