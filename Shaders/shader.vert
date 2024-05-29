#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;
layout(location = 2) in vec2 tex;

layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
//	vec3 colour;
} uboModel;

layout(set = 0, binding = 2) uniform UboLightData {
	mat4 position;
	vec3 colour;
} uboLightData;

layout(location = 0) out vec3 fragCol;
layout(location = 1) out vec2 fragTex;

void main() {
	gl_Position = uboViewProjection.projection * uboViewProjection.view * uboModel.model * vec4(pos, 1.0);
	//gl_Position = uboViewProjection.projection * uboLightData.position * uboModel.model * vec4(pos, 1.0);
	//gl_Position = uboViewProjection.projection * uboViewProjection.view * uboLightData.position * vec4(uboLightData.colour, 1.0);
	
	fragCol = uboLightData.colour;
	//fragCol = uboModel.colour;

	fragTex = tex;
}