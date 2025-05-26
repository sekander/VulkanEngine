#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 inTangent;

layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
    vec4 cameraPos;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
} uboModel;

layout(push_constant) uniform PushColour {
	vec4 colour;
	mat3 normMat;
    vec3 tangent;
    
    // float parallaxHeightScale; // For parallax effect in FS
    vec3 rimColor;           // Color for rim lighting in FS
    float rimPower;          // Power for rim lighting in FS
} pushColour;

layout(location = 0) out vec2 texCoords;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out mat3 TBN;
layout(location = 8) out vec3 viewPos;

void main() {
    texCoords = tex;

    // World-space vertex position
    fragPosWorld = vec3(uboModel.model * vec4(inPosition, 1.0));
    viewPos = uboViewProjection.cameraPos.xyz;

    // Compute normal, tangent, bitangent in world space
    vec3 N = normalize(pushColour.normMat * inNormal);
    vec3 T = normalize(pushColour.normMat * inTangent);
    // Re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));

    TBN = mat3(T, B, N);

    gl_Position = uboViewProjection.projection * uboViewProjection.view * vec4(fragPosWorld, 1.0);
}
