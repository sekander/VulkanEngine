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

// SDF for a unit sphere centered at push.sdfCenter
float sdf(vec3 p) {
    return length(p - push.sdfCenter) - 1.0;
}

float ambientOcclusion(vec3 p, vec3 n) {
    float ao = 0.0;
    float sca = 1.0;
    for (float t = 0.05; t < 1.0; t += 0.1) {
        float d = sdf(p + n * t);
        ao += (t - d) * sca;
        sca *= 0.95;
    }
    return clamp(1.0 - ao, 0.0, 1.0);
}

void main() {
    vec3 baseColor = texture(textureSampler, texCoords).rgb;
    float ao = ambientOcclusion(fragPosWorld, normalize(normalWS));
    outColor = vec4(baseColor * ao, 1.0);
}
