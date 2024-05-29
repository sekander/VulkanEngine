#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normVert;
layout(location = 2) in vec2 tex;

layout(location = 9) out VertexStage {
    vec3 normal;
} vs_out;


layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
	//mat3 normMat;
} uboModel;

layout(set = 0, binding = 2) uniform UboLightData {
	vec4 colour[3];
	vec3 position[3];
} uboLightData;

//Push constant 
layout (push_constant) uniform PushColour {
	vec4 colour;
	mat3 normMat;
} pushColour;

layout(location = 5) out vec2 texCoords;


void main() {
    texCoords = tex;
//    gl_Position = view * push.modelMatrix * vVertex;
//    vs_out.normal = mat3(push.normalMatrix) * vNormal;
	gl_Position = uboViewProjection.view * uboModel.model * vec4(pos, 1.0);
	vs_out.normal = mat3(pushColour.normMat) * normVert;
}