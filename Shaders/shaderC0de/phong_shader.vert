#version 450 // Use GLSL 4.5
#extension GL_ARB_separate_shader_objects : enable

// Vertex
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
} uboModel;

layout(set = 0, binding = 2) uniform UboLightData {
    vec4 colour[3];
    vec3 position[3];
} uboLightData;

// Push constant 
layout(push_constant) uniform PushColour {
    vec4 colour;
    mat3 normMat;
} pushColour;

// Light Attributes
layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 lightDir[3];
layout(location = 4) out vec3 eyeDir;
layout(location = 5) out vec2 texCoords;
layout(location = 6) out vec4 lightColour[3];

void main() {
    // Copy light colors from uniform buffer
    lightColour[0] = uboLightData.colour[0];
    lightColour[1] = uboLightData.colour[1];
    lightColour[2] = uboLightData.colour[2];

    // Copy light positions from uniform buffer
    vec3 lightPos[3];
    lightPos[0] = uboLightData.position[0];
    lightPos[1] = uboLightData.position[1];
    lightPos[2] = uboLightData.position[2];

    // Pass texture coordinates
    texCoords = tex;

    // Calculate normal matrix from push constant
    mat3 normalMatrix = pushColour.normMat;

    // Transform normal to the correct orientation
    vertNormal = normalize(normalMatrix * normVert);

    // Transform vertex position to camera space
    vec3 vertPos = vec3(uboViewProjection.view * uboModel.model * vec4(pos, 1.0));

    // Calculate eye direction
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;

    // Calculate light directions
    for (int i = 0; i < 3; ++i) {
        lightDir[i] = normalize(lightPos[i] - vertPos);
    }

    // Calculate vertex position in clip space
    gl_Position = uboViewProjection.projection * vec4(vertPos, 1.0);

    // Pass normal to the next stage
    vs_out.normal = vertNormal;
}

