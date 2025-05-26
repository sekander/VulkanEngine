// Phong Fragment Shader
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColour; // Final output colour

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 lightDir[3];
layout(location = 4) in vec3 eyeDir;
layout(location = 5) in vec2 texCoords;
layout(location = 6) in vec4 lightColour[3];

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

void main() {
    // Spotlight position
    vec3 spotlight_pos = vec3(1.0, 0.0, 0.0);

    float diff[3];
    vec3 reflection[3];
    float spec[3];
    vec3 result[3];
    // Debug: Render a small colored sphere at each light position
    // This is a fragment shader, so we can only "fake" this by coloring fragments near the light positions in view space

    for(uint i = 0; i < 3; i++) {
        vec3 ambient = 0.1 * lightColour[i].xyz;

        diff[i] = max(dot(vertNormal, lightDir[i]), 0.0);
        reflection[i] = normalize(reflect(-lightDir[i], vertNormal));
        spec[i] = max(dot(eyeDir, reflection[i]), 0.0);
        spec[i] = pow(spec[i], 32.0);
        result[i] = (ambient + (diff[i] * lightColour[i].xyz) + spec[i]);

        if(i == 0) {
            outColour = vec4(result[i], 1.0) * texture(textureSampler, texCoords);
        } else if(i > 0) {
            outColour += vec4(result[i], 1.0) * texture(textureSampler, texCoords);
        }
    }
}
