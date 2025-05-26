#version 450
// Same inputs...
layout(location = 0) out vec4 outColour;

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

layout(push_constant) uniform PushColour {
	vec4 colour;
	mat3 normMat;
    int maxLights;
        // float parallaxHeightScale; // Strength of the parallax effect
    vec3 rimColor;           // Color for rim lighting
    float rimPower;          // Power for rim lighting
} pushColour;

const vec3 spotDir = normalize(vec3(0.0, -1.0, 0.0));
const float innerCut = cos(radians(12.5));
const float outerCut = cos(radians(17.5));

void main() {
    vec3 normal = normalize(TBN * (texture(normalMap, texCoords).rgb * 2.0 - 1.0));
    vec3 viewDir = normalize(viewPos - fragPosWorld);
    vec3 totalLighting = vec3(0.0);

    for(int i = 0; i < 3; i++) {
        vec3 lightDir = normalize(uboLightData.position[i] - fragPosWorld);
        vec3 reflectDir = reflect(-lightDir, normal);

        float theta = dot(lightDir, -spotDir);
        float epsilon = innerCut - outerCut;
        float intensity = clamp((theta - outerCut) / epsilon, 0.0, 1.0);

        float diff = max(dot(normal, lightDir), 0.0);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), uboLightData.shininess[i]);

        float distance = length(uboLightData.position[i] - fragPosWorld);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

        vec3 ambient = uboLightData.ambientStrength[i] * uboLightData.colour[i].rgb;
        vec3 diffuse = uboLightData.diffuseStrength[i] * diff * uboLightData.colour[i].rgb;
        vec3 specular = uboLightData.specularStrength[i] * spec * uboLightData.colour[i].rgb;

        // Rim lighting
        float rim = 1.0 - max(dot(normal, viewDir), 0.0);
        rim = pow(rim, pushColour.rimPower);
        vec3 rimLighting = pushColour.rimColor * rim;

        vec3 lighting = (ambient + (diffuse + specular) * intensity + rimLighting) * attenuation;

        totalLighting += lighting;
    }

    vec4 texColor = texture(textureSampler, texCoords);
    outColour = vec4(totalLighting * texColor.rgb, 1.0);

}
