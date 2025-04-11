#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 lightDir;
layout(location = 2) in vec3 eyeDir;
layout(location = 3) in vec2 texCoords;
layout(location = 4) in vec3 lightColour;

layout(location = 5) in vec3 fragCol;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;


layout (push_constant) uniform PushColour {
	vec3 colour;
} pushColour;


float diff;
vec3 reflection;
float spec;

void main() {
	
//Lighting Ouput
//#######################################################################################
	//ambient
	vec3 ambient = 0.1 * lightColour.xyz;


	diff = max(dot(vertNormal, lightDir), 0.0);
	reflection = normalize(reflect(-lightDir, vertNormal));
	
	spec = max(dot(eyeDir, reflection), 0.0);	
	spec = pow(spec, 32.0);	

	vec3 result = (ambient + (diff * lightColour) + spec);


	outColour = vec4(result, 1.0) * texture(textureSampler, texCoords);
	//outColour = vec4(result, 1.0) * vec4(fragCol, 1.0) * texture(textureSampler, texCoords);


//Normal Texture Output
//#######################################################################################


	//Push Constant Output
	//outColour = vec4(pushColour.colour, 1.0) * texture(textureSampler, texCoords);
	
	//UBO Colour Output
	//outColour = vec4(fragCol, 1.0) * texture(textureSampler, texCoords);
	
	//Normal Output
	//outColour = texture(textureSampler, texCoords);
}
