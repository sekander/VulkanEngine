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
} uboModel;

layout(set = 0, binding = 2) uniform UboLightData {
    vec4 colour[3];
    vec3 position[3];
} uboLightData;

layout(push_constant) uniform PushColour {
    vec4 colour;
    mat3 normMat;
} pushColour;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec3 lightPos[3];

    // Initialize light positions
    lightPos[0] = uboLightData.position[0];
    lightPos[1] = vec3(0.0, 0.0, 0.0);
    lightPos[2] = vec3(0.0, 0.0, 0.0);

    // Transform the normal to the correct orientation
    mat3 normalMatrix = pushColour.normMat;
    vs_out.normal = normalize(normalMatrix * normVert);

    // Calculate vertex position
    vec3 vertPos = vec3(uboViewProjection.view * uboModel.model * vec4(pos, 1.0));
    gl_Position = uboViewProjection.projection * vec4(vertPos, 1.0);
}

