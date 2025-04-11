#version 450 		// Use GLSL 4.5
#extension GL_ARB_separate_shader_objects : enable

//Vertex
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

//Light Attributes
layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 lightDir[3];
layout(location = 4) out vec3 eyeDir;
layout(location = 5) out vec2 texCoords;

//layout(location = 6) out vec3 lightColour[3];
layout(location = 6) out vec4 lightColour[3];


vec3 lightPos[3];

void main() {

	//lightColour[0] = vec3(pushColour.colour);
	lightColour[0] = uboLightData.colour[0];
	lightColour[1] = uboLightData.colour[1];
	lightColour[2] = uboLightData.colour[2];
	
	lightPos[0] = uboLightData.position[0];
	lightPos[1] = vec3(0.0, 0.0, 0.0);
	lightPos[2] = vec3(0.0, 0.0, 0.0);

	//Edit
	//Create this on the CPU side and push it to the shader

    texCoords = tex;

    mat3 normalMatrix = pushColour.normMat;
	
	vertNormal = normalize(normalMatrix * normVert); //Rotate the normal to the correct orientation

    vec3 vertPos = vec3(uboViewProjection.view * uboModel.model * vec4(pos, 1.0));
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;

	lightDir[0] = normalize(lightPos[0] - vertPos);
	lightDir[1] = normalize(lightPos[1] - vertPos);
	lightDir[2] = normalize(lightPos[2] - vertPos);









//	gl_Position = uboViewProjection.projection * uboViewProjection.view * uboModel.model * vec4(pos, 1.0);
	gl_Position = uboViewProjection.view * uboModel.model * vec4(pos, 1.0);
	vs_out.normal = mat3(pushColour.normMat) * normVert;

}