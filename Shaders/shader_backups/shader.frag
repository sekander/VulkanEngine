#version 450

layout(location = 0) in vec3 fragCol;
layout(location = 1) in vec2 fragTex;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

layout (push_constant) uniform PushColour {
	vec3 colour;
} pushColour;

//layout(set = 0, binding = 2) uniform UboLightData {
//	mat4 position;
//	vec3 colour;
//} uboLightData;

void main() {
	//outColour = vec4(fragCol, 1.0);
	//outColour = vec4(pushColour.colour, 1.0) * texture(textureSampler, fragTex);
	outColour = vec4(fragCol, 1.0) * texture(textureSampler, fragTex);
	//outColour = vec4(uboLightData.colour, 1.0) * texture(textureSampler, fragTex);
	//outColour = texture(textureSampler, fragTex);
}
