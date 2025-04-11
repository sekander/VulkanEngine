#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 lightDir[3];
layout(location = 4) in vec3 eyeDir;
layout(location = 5) in vec2 texCoords;
layout(location = 6) in vec4 lightColour[3];


layout(set = 1, binding = 0) uniform sampler2D textureSampler;


float diff[3];
vec3 reflection[3];
float spec[3];
vec3 result[3];
//vec4 result[3];


float exponent; // Angular attenuation exponent
float cutoff;
// Cutoff angle (between 0 and 90)


void main() {


//Spotlight stuff
vec3 position = vec3(3.0, 0.0, 0.0);
vec3 spotlight_pos = vec3(1.0, 0.0, 0.0);


for(uint i = 0; i < 3; i++)
{
		vec3 ambient = 0.1 * lightColour[i].xyz;


		diff[i] = max(dot(vertNormal, lightDir[i]), 0.0);
		reflection[i] = normalize(reflect(-lightDir[i], vertNormal));

		spec[i] = max(dot(eyeDir, reflection[i]), 0.0);	
		spec[i] = pow(spec[i], 32.0);	

		result[i] = (ambient + (diff[i] * lightColour[i].xyz) + spec[i]);

	if(i == 0)
		outColour = vec4(result[i], 1.0) * texture(textureSampler, texCoords);
	else if(i > 0)
		outColour += vec4(result[i], 1.0) * texture(textureSampler, texCoords);
}

//	outColour = vec4(result[0], 1.0) * texture(textureSampler, texCoords);
//	outColour += vec4(result[1], 1.0) * texture(textureSampler, texCoords);

/*
//Lighting Ouput 1
//#######################################################################################
{	//ambient
	vec3 ambient = 0.1 * lightColour[0].xyz;


	diff[0] = max(dot(vertNormal, lightDir[0]), 0.0);
	reflection[0] = normalize(reflect(-lightDir[0], vertNormal));
	
	spec[0] = max(dot(eyeDir, reflection[0]), 0.0);	
	spec[0] = pow(spec[0], 32.0);	

	vec3 result = (ambient + (diff[0] * lightColour[0]) + spec[0]);


	outColour = vec4(result, 1.0) * texture(textureSampler, texCoords);
}
//#######################################################################################

//Lighting Ouput 2
//#######################################################################################
{	//ambient
	vec3 ambient = 0.1 * lightColour[1].xyz;


	diff[1] = max(dot(vertNormal, lightDir[1]), 0.0);
	reflection[1] = normalize(reflect(-lightDir[1], vertNormal));
	
	spec[1] = max(dot(eyeDir, reflection[1]), 0.0);	
	spec[1] = pow(spec[1], 32.0);	

	vec3 result = (ambient + (diff[1] * lightColour[1]) + spec[1]);


	outColour += vec4(result, 1.0) * texture(textureSampler, texCoords);
}
//#######################################################################################

*/













//Normal Texture Output
//#######################################################################################


	//Push Constant Output
	//outColour = vec4(pushColour.colour, 1.0) * texture(textureSampler, texCoords);
	
	//UBO Colour Output
	//outColour = vec4(fragCol, 1.0) * texture(textureSampler, texCoords);
	
	//Normal Output
	//outColour = texture(textureSampler, texCoords);
}
