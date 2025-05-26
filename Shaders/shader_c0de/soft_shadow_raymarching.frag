#version 450

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in vec3 normalWS;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(push_constant) uniform PushConstants {
    vec4 colour;
    mat3 normMat;
    vec3 sdfCenter;
} push;

// SDF for a unit sphere
float sdf(vec3 p) {
    return length(p - push.sdfCenter) - 1.0;
}

float softShadow(vec3 ro, vec3 rd, float tMin, float tMax) {
    float shadow = 1.0;
    float t = tMin;
    for (int i = 0; i < 32; ++i) {
        float h = sdf(ro + rd * t);
        if (h < 0.001) return 0.0; // Hit shadow object
        shadow = min(shadow, 10.0 * h / t);
        t += clamp(h, 0.01, 0.2);
        if (t > tMax) break;
    }
    return clamp(shadow, 0.0, 1.0);
}

void main() {
    vec3 baseColor = texture(textureSampler, texCoords).rgb;
    vec3 lightDir = normalize(push.sdfCenter - fragPosWorld);
    float s = softShadow(fragPosWorld, lightDir, 0.05, 3.0);
    outColor = vec4(baseColor * s, 1.0);
}

