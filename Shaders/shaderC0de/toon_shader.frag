#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColour;

layout(location = 9) in VertexStage {
    vec3 normal;
    vec2 texCoord;
    vec3 fragPos;
} fs_in;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

void main() {
    // Ambient light intensity
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse light intensity
    vec3 lightColor = vec3(1.0);
    vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0)); // Directional light direction
    float diff = max(dot(fs_in.normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Calculate specular highlights (optional)
    vec3 viewDir = normalize(-fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, fs_in.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(0.5) * spec;

    // Combine ambient, diffuse, and specular light components
    vec3 result = (ambient + diffuse + specular);

    // Apply toon shading
    float levels = 3.0; // Number of intensity levels
    result = floor(result * levels) / levels;

    // Sample texture and apply shading
    vec4 texColor = texture(textureSampler, fs_in.texCoord);
    outColour = vec4(result, 1.0) * texColor;
}

