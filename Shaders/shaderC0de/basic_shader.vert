#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
	//mat3 normMat;
} uboModel;

//Push constant 
layout (push_constant) uniform PushColour {
	vec4 colour;
	mat3 normMat;
} pushColour;

void main() {
    //gl_Position = vec4(inPosition, 1.0);
	gl_Position = uboViewProjection.view * uboModel.model * vec4(inPosition, 1.0);
    //fragColor = inColor;
    fragColor = pushColour.colour.rgb;
}