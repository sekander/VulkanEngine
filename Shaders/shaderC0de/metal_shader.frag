#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColour;

layout(location = 9) in VertexStage {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
} fs_in;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

void main() {
    // Metal properties
    vec3 albedo = vec3(0.8, 0.8, 0.8); // Base color of the metal
    float roughness = 0.2; // Roughness of the metal surface (0.0 = smooth, 1.0 = rough)
    float metallic = 1.0; // Metallic intensity (0.0 = dielectric, 1.0 = fully metallic)

    // Calculate specular reflection
    vec3 viewDir = normalize(-fs_in.fragPos);
    vec3 reflectDir = reflect(-viewDir, normalize(fs_in.normal));
    vec3 specular = vec3(0.9) * pow(max(dot(viewDir, reflectDir), 0.0), 1.0 - roughness) * metallic;

    // Final color calculation (using Cook-Torrance BRDF)
    vec3 F0 = vec3(0.04); // Fresnel reflectance at normal incidence
    vec3 diffuse = (1.0 - metallic) * (1.0 - F0) * albedo;
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - dot(viewDir, reflectDir), 5.0);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = vec3(0.03); // Low ambient light for metals
    vec3 ambient = irradiance * albedo;

    vec3 color = (kD * albedo / 3.14159) + (specular * kS);
    vec3 finalColor = ambient + color;

    outColour = vec4(finalColor, 1.0) * texture(textureSampler, fs_in.texCoord);
}

