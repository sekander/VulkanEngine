// #version 450

// layout(location = 0) in vec3 fragPosWorld;   // Fragment world-space position
// layout(location = 1) in vec2 texCoords;
// layout(location = 2) in mat3 TBN;            // Tangent-Bitangent-Normal matrix
// layout(location = 5) in vec3 normalWS;       // World-space surface normal

// layout(location = 0) out vec4 outColor;

// // Scene texture
// layout(set = 1, binding = 0) uniform sampler2D textureSampler;

// layout(set = 0, binding = 2) uniform UboLightData {
//     vec4 colour[3];
//     vec3 position[3];
// 	float radius[3];
//     float ambientStrength[3];
//     float diffuseStrength[3];
//     float specularStrength[3];
//     float shininess[3];
// } uboLightData;

// // SDF-based light source
// layout(push_constant) uniform PushConstants {
// 	vec4 colour;
// 	mat3 normMat;
//     // vec3 sdfCenter;   // Light center
//     // float sdfRadius;  // Sphere radius
//     // vec3 lightColor;  // Light color (e.g., white, yellow)
//     // float intensity;  // Light strength
// } push;

// void main() {
//     // Sample surface base color
//     vec4 baseColor = texture(textureSampler, texCoords);

//     // Vector from fragment to SDF light center
//     vec3 toLight = push.sdfCenter - fragPosWorld;
//     float dist = length(toLight);

//     // Light only affects fragments within a certain radius + falloff
//     float falloffRange = push.sdfRadius * 2.0;

//     // Soft attenuation: 1 near center, 0 at edge
//     float attenuation = 1.0 - clamp((dist - push.sdfRadius) / (falloffRange - push.sdfRadius), 0.0, 1.0);

//     // Normalize light direction
//     vec3 lightDir = normalize(toLight);

//     // Diffuse lighting (Lambert)
//     float diff = max(dot(normalWS, lightDir), 0.0);

//     // Final light contribution
//     // vec3 lighting = push.colour.rgb * diff * attenuation * push.intensity;
//     vec3 lighting = push.colour.rgb * diff * attenuation * 2.0;

//     // Output final color with lighting applied
//     outColor = vec4(baseColor.rgb * lighting, baseColor.a);

//     // Debug normal view (optional)
//     // outColor = vec4(normalWS * 0.5 + 0.5, 1.0);
// }


#version 450

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in vec3 normalWS;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(set = 0, binding = 2) uniform UboLightData {
    vec4 colour[3];            // Light colors
    vec3 position[3];          // Light positions
    float radius[3];           // Light radii
    float ambientStrength[3];  // Ambient coefficient
    float diffuseStrength[3];  // Diffuse coefficient
    float specularStrength[3]; // Specular (unused here)
    float shininess[3];        // Shininess (unused here)
} uboLightData;

layout(push_constant) uniform PushConstants {
	vec4 colour;
	mat3 normMat;
    vec3 sdfCenter;   // Light center
    // float sdfRadius;  // Sphere radius
    // vec3 lightColor;  // Light color (e.g., white, yellow)
    // float intensity;  // Light strength
} push;
void main() {
    vec4 baseColor = texture(textureSampler, texCoords);
    vec3 totalLight = vec3(0.0);

    // Hardcoded test light parameters
    vec3 lightPositions[3] = vec3[](
        vec3(5.0, 5.0, 5.0),
        vec3(-3.0, 2.0, 1.0),
        vec3(0.0, 10.0, 0.0)
    );

    vec3 lightColors[3] = vec3[](
        vec3(1.0, 0.5, 0.5),
        vec3(0.5, 1.0, 0.5),
        vec3(0.5, 0.5, 1.0)
    );

    float lightRadius[3] = float[](4.0, 3.0, 6.0);
    float ambientStrength[3] = float[](0.1, 0.1, 0.1);
    float diffuseStrength[3] = float[](1.0, 1.0, 1.0);

    // vec3 totalLight = vec3(0.0);

    for (int i = 0; i < 3; ++i) {
        // Transform light position to world space if needed (example: using normMat)
        // If uboLightData.position[i] is in local/object space, convert to world space:
        vec3 lightPosWorld = push.normMat * uboLightData.position[i];
        vec3 toLight = lightPosWorld - fragPosWorld;
        float dist = length(toLight);
        // float falloffRange = lightRadius[i] * 2.0;
        float falloffRange = uboLightData.radius[i] * 2.0;

        // float attenuation = 1.0 - clamp((dist - lightRadius[i]) / (falloffRange - lightRadius[i]), 0.0, 1.0);
        float attenuation = 1.0 - clamp((dist - uboLightData.radius[i]) / (falloffRange - uboLightData.radius[i]), 0.0, 1.0);

        vec3 lightDir = normalize(toLight);
        float diff = max(dot(normalWS, lightDir), 0.0);

        // vec3 diffuse = lightColors[i] * diff * diffuseStrength[i] * attenuation;
        // vec3 ambient = lightColors[i] * ambientStrength[i];
        
        // vec3 diffuse = uboLightData.colour[i].rgb * diff * diffuseStrength[i] * attenuation;
        vec3 diffuse = uboLightData.colour[i].rgb * diff * uboLightData.diffuseStrength[i] * attenuation;
        vec3 ambient = uboLightData.colour[i].rgb * diff * ambientStrength[i];
        //
        // vec3 ambient = uboLightData.colour[i].rgb * diff * uboLightData.ambientStrength[i];

        totalLight += diffuse + ambient;
    }
    outColor = vec4(baseColor.rgb * totalLight, baseColor.a);
}