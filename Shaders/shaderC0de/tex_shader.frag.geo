#version 450

layout(location = 0) in vec2 geomTexCoords;
layout(location = 1) in vec3 geomColor;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

void main() {
    outColor = vec4(geomColor, 1.0) * texture(textureSampler, geomTexCoords);
}
