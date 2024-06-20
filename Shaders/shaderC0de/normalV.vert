#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;  // Output color to geometry shader
layout(location = 1) out vec3 fragNormal; // Output normal to geometry shader

layout(set = 0, binding = 0) uniform UboViewProjection {
    mat4 projection;
    mat4 view;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
    mat4 model;
} uboModel;

void main() {
    gl_Position = uboViewProjection.projection * uboViewProjection.view * uboModel.model * vec4(inPosition, 1.0);
    fragColor = vec3(1.0);  // White color for simplicity
    fragNormal = mat3(uboModel.model) * inNormal; // Transform normal to world space and pass to geometry shader
}
