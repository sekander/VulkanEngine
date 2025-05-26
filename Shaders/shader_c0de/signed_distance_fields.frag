#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec3 fragPosWorld;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

// Simple signed distance function for a circle in UV space
float sdCircle(vec2 p, float radius) {
    return length(p - vec2(0.5)) - radius;
}

layout(location = 0) out vec4 outColor;

void main() {
    // Sample model texture
    vec4 baseColor = texture(textureSampler, fragUV);

    // Compute SDF circle distance from center of UV space
    float dist = sdCircle(fragUV, 0.3);

    // Smooth edge (anti-aliasing)
    float edgeWidth = 0.01;
    float sdfAlpha = 1.0 - smoothstep(0.0, edgeWidth, dist);

    // Blend SDF circle effect color (e.g. red outline) with base texture
    vec3 sdfColor = vec3(1.0, 0.0, 0.0); // red color for the circle outline

    // Final color blends base texture with SDF overlay by alpha
    vec3 finalColor = mix(baseColor.rgb, sdfColor, sdfAlpha);

    outColor = vec4(finalColor, baseColor.a);
}
