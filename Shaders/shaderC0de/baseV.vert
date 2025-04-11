#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragNormal; // Output normal to geometry shader
//layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform UboViewProjection {
    mat4 projection;
    mat4 view;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
    mat4 model;
} uboModel;

//Push constant 
layout (push_constant) uniform PushColour {
	vec4 colour;
	mat3 normMat;
} pushColour;


void main() {
    gl_Position = uboViewProjection.projection * uboViewProjection.view * uboModel.model * vec4(inPosition, 1.0);
    fragNormal = mat3(uboModel.model) * inNormal; // Transform normal to world space and pass to geometry shader
    //fragNormal = pushColour.colour.rgb;
}
