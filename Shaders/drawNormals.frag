#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;
//layout(location = 1) in vec2 texCoords;

//layout(set = 1, binding = 0) uniform sampler2D textureSampler;

void main() {
    //fragColor = vec4(0.0, 1.0, 1.0, 0.0) * texture(textureSampler, texCoords);
    fragColor = vec4(0.0, 1.0, 1.0, 0.0);
}