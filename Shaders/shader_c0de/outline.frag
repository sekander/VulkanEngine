#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 5) in vec3 fragPosWorld;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(push_constant) uniform PushConstants {
    vec4 colour;           
    mat4 normalMat;        
    int maxLights;         
    vec3 rimLightColour;   
    float rimLightStrength;

    float outlineThickness;
    float time;            // Unused now
    vec3 sdfCenter;
    float sdfRadius;
} push;

// Outline color
const vec4 outlineColor = vec4(1.0, 0.0, 0.0, 1.0); // Red

// Distance from a sphere SDF
float sdfSphere(vec3 p, float r) {
    return length(p) - r;
}

// Outline mask using smoothstep
float sdfOutline(float d, float thickness) {
    return smoothstep(thickness * 1.5, 0.0, d);
}

void main() {
    float distance = sdfSphere(fragPosWorld - push.sdfCenter, push.sdfRadius);

    // Increase base thickness for stronger outline effect
    float outlineMask = sdfOutline(distance, push.outlineThickness * 2.0);

    vec4 baseColor = texture(textureSampler, texCoords);
    vec4 finalColor = mix(baseColor, outlineColor, outlineMask);
    outColor = vec4(finalColor.rgb, finalColor.a);


    //Debug
    // float maxDist = 0.5; // tweak this value

    // // Map distance to grayscale
    // float gray = clamp(distance / maxDist * 0.5 + 0.5, 0.0, 1.0);

    // outColor = vec4(vec3(gray), 1.0);


    // outColor = vec4(vec3(distance), 1.0);
	// outColor = vec4(push.colour) * texture(textureSampler, texCoords);
}
