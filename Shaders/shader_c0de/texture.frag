#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(push_constant) uniform PushConstants {
    vec4 colour; // You can keep or discard this if unused
    mat4 normalMat; // If you're using it
    int maxLights;
    vec3 rimLightColour;
    float rimLightStrength;
} push;

void main() {
	outColor = vec4(push.colour) * texture(textureSampler, texCoords);
	// outColor = vec4(fragColor, 1.0) * texture(textureSampler, texCoords);
	//outColor = vec4(vec3(1.0, 1.0, 1.0), 1.0) * texture(textureSampler, texCoords);
}