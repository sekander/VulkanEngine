#version 450

layout(location = 0) in vec2 texCoords;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in mat3 TBN;
layout(location = 8) in vec3 viewPos;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;
layout(set = 1, binding = 1) uniform sampler2D normalMap;

layout(set = 0, binding = 2) uniform UboLightData {
    vec4 colour[3];
    vec3 position[3];
    float ambientStrength[3];
    float diffuseStrength[3];
    float specularStrength[3];
    float shininess[3];
} uboLightData;

layout(push_constant) uniform PushConstants {
    vec4 colour; // You can keep or discard this if unused
    mat4 normalMat; // If you're using it
    int maxLights;
    vec3 rimLightColour;
    float rimLightStrength;
} push;

layout(location = 0) out vec4 outColour;

void main() {
    // int i = 0; // use index 0 for point light
    vec3 normal = normalize(TBN * (texture(normalMap, texCoords).rgb * 2.0 - 1.0));
    vec3 viewDir = normalize(viewPos - fragPosWorld);
    vec3 totalLighting = vec3(0.0);

    for(int i = 0; i < 3; i++) {
        vec3 lightDir = normalize(uboLightData.position[i] - fragPosWorld);
        vec3 reflectDir = reflect(-lightDir, normal);

        float diff = max(dot(normal, lightDir), 0.0);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), uboLightData.shininess[i]);

        float distance = length(uboLightData.position[i] - fragPosWorld);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

        vec3 ambient = uboLightData.ambientStrength[i] * uboLightData.colour[i].rgb;
        vec3 diffuse = uboLightData.diffuseStrength[i] * diff * uboLightData.colour[i].rgb;
        vec3 specular = uboLightData.specularStrength[i] * spec * uboLightData.colour[i].rgb;

        // Rim lighting
        float rim = 1.0 - max(dot(normal, viewDir), 0.0);
        rim = pow(rim, 2.0);
        vec3 rimLighting = push.rimLightStrength * rim * push.rimLightColour;

        totalLighting += (ambient + diffuse + specular + rimLighting) * attenuation;
    }
    vec4 texColor = texture(textureSampler, texCoords);
    outColour = vec4(totalLighting * texColor.rgb, 1.0);

}
