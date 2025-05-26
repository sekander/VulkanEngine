#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragPosition; // Pass vertex position to geometry shader

layout(set = 0, binding = 0) uniform UboViewProjection {
    mat4 projection;
    mat4 view;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
    mat4 model;
} uboModel;

void main() {
    mat4 flipY = mat4(1.0);
    flipY[1][1] = -1.0;
    // Transform vertex position to clip space
    vec4 pos = uboViewProjection.projection * uboViewProjection.view * uboModel.model * flipY * vec4(inPosition, 1.0);
    gl_Position = pos;

    // Pass vertex position to geometry shader
    fragPosition = pos.xyz;
}