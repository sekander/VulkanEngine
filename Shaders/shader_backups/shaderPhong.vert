#version 450 		// Use GLSL 4.5
#extension GL_ARB_separate_shader_objects : enable

//Vertex
layout(location = 0) in vec3 pos;
//Colour Vertex
layout(location = 1) in vec3 col;
//TexCoords
layout(location = 2) in vec2 tex;




layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
	vec3 colour;
} uboModel;

layout(set = 0, binding = 2) uniform UboLightData {
	vec3 position;
	vec4 colour;
} uboLightData;


//Light Attributes
layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 lightDir;
layout(location = 2) out vec3 eyeDir;
layout(location = 3) out vec2 texCoords;

layout(location = 4) out vec4 lightColour;
layout(location = 5) out vec3 fragCol;


vec3 lightPos;
vec3 vertexNormal;

void main() {

	//lightPos = vec3(50.0, 50.0, 40.0);
	//lightColour = vec3(0.0, 0.0, 1.0);
	lightColour = uboLightData.colour;
	//lightColour = vec3(uboLightData.colour.x, uboLightData.colour.y, uboLightData.z);
	lightPos = uboLightData.position;

	vertexNormal = normalize(pos);

    texCoords = tex;

    mat3 normalMatrix = mat3(transpose(inverse(uboModel.model)));
	vertNormal = normalize(normalMatrix * vertexNormal); //Rotate the normal to the correct orientation

    vec3 vertPos = vec3(uboViewProjection.view * uboModel.model * vec4(pos, 1.0));
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;

	lightDir = normalize(lightPos - vertPos);
	//lightColour = _lightColour;









	gl_Position = uboViewProjection.projection * uboViewProjection.view * uboModel.model * vec4(pos, 1.0);
	
	//fragCol = uboLightData.colour;
	//fragCol = uboModel.colour;

}