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

float sdf(vec3 p) {
    return length(p - push.sdfCenter) - 1.0;
}

vec3 randomHemisphereVector(vec3 n, int i) {
    float phi = float(i) * 3.14159 * 0.5;
    float z = 0.5;
    float r = sqrt(1.0 - z * z);
    vec3 tangent = normalize(cross(n, vec3(0.0, 1.0, 0.0)));
    vec3 bitangent = cross(n, tangent);
    return normalize(r * cos(phi) * tangent + r * sin(phi) * bitangent + z * n);
}

float indirectDiffuse(vec3 p, vec3 n) {
    float result = 0.0;
    const int samples = 6;
    for (int i = 0; i < samples; ++i) {
        vec3 dir = randomHemisphereVector(n, i);
        float d = sdf(p + dir * 0.2);
        result += exp(-4.0 * abs(d));
    }
    return result / float(samples);
}

void main() {
    vec3 baseColor = texture(textureSampler, texCoords).rgb;
    float bounce = indirectDiffuse(fragPosWorld, normalize(normalWS));
    outColor = vec4(baseColor + bounce * 0.1, 1.0);
}
