#version 450

layout(location = 0) in vec3 fragNormal;  // Normal from vertex shader (should be normalized)
layout(location = 1) in vec2 texCoords;
layout(location = 5) in vec3 vertPos;    // Vertex position in world space
layout(location = 6) in vec3 normalWS;    // Vertex position in world space

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(set = 0, binding = 2) uniform UboLightData {
    vec4 colour[3];
    vec3 position[3];
} uboLightData;

layout(push_constant) uniform PushConstants {
    vec4 colour;           // Base color multiplier
    mat4 normalMat;        // To transform normals if needed
    // int maxLights;         // Unused here
    // vec3 rimLightColour;   // Unused here
    // float rimLightStrength;// Unused here
    // vec3 lightDir;         // Directional light direction in world space (normalized)
} push;

void main() {
     // Use the first light only for toon shading
    vec3 lightPos = uboLightData.position[0];
    vec3 lightColor = uboLightData.colour[0].rgb;

    // Light direction
    vec3 lightDir = normalize(lightPos - vertPos);

    // Normalize the interpolated normal
    vec3 normal = normalize(normalWS);

    // Compute Lambertian diffuse intensity
    float diff = max(dot(normal, lightDir), 0.0);

    // Quantize the intensity for toon effect
    float levels = 4.0; // e.g., 4 shades
    float toonDiffuse = floor(diff * levels) / (levels - 1.0);

    // Sample the texture
    vec4 texColor = texture(textureSampler, texCoords);

    // Combine texture with toon shading and base color
    vec3 finalColor = texColor.rgb * push.colour.rgb * toonDiffuse * lightColor;

    // Output final shaded color
    outColor = vec4(finalColor, texColor.a);
}
