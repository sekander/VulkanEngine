#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 tex;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoords;

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
    // mat4 flipY = mat4(1.0);
    // flipY[1][1] = -1.0;
    texCoords = tex;
	gl_Position = uboViewProjection.projection * uboViewProjection.view * uboModel.model * vec4(inPosition, 1.0);
    //fragColor = inColor;
    fragColor = pushColour.colour.rgb;
}