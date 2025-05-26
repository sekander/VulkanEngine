#version 450

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

void main() {
    vec3 norm = texture(normalMap, texCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0); // Convert from [0,1] to [-1,1]
    vec3 normalWS = normalize(TBN * norm); // Tangent to world space

    vec3 viewDir = normalize(viewPos - fragPosWorld);

    vec3 result = vec3(0.0);
    for (int i = 0; i < 3; ++i) {
    // for (int i = 0; i < pushColour.maxLights; ++i) {
        vec3 lightDir = normalize(uboLightData.position[i] - fragPosWorld);
        vec3 reflectDir = reflect(-lightDir, normalWS);

        float diff = max(dot(normalWS, lightDir), 0.0);
        // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), uboLightData.shininess[i]);
        vec3 ambient = 0.1 * uboLightData.colour[i].rgb;
        vec3 diffuse = diff * uboLightData.colour[i].rgb;
        vec3 specular = spec * uboLightData.colour[i].rgb;

        // vec3 ambient = 0.1 * pushColour.colour.rgb;
        // vec3 ambient = uboLightData.ambientStrength[i] * uboLightData.colour[i].rgb;
        // vec3 diffuse = uboLightData.diffuseStrength[i] * uboLightData.colour[i].rgb * diff;
        // vec3 specular = uboLightData.specularStrength[i] * uboLightData.colour[i].rgb * spec;

        result += ambient + diffuse + specular;
    }

    vec4 baseColor = texture(textureSampler, texCoords);
    outColour = vec4(result, 1.0) * baseColor;

    



}