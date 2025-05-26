#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 inTangent;

// layout(location = 2) out vec3 inTangent;      // Tangent vector
// layout(location = 2) out vec3 fragPos;

layout(set = 0, binding = 0) uniform UboViewProjection {
	mat4 projection;
	mat4 view;
    vec4 cameraPos;
} uboViewProjection;

layout(set = 0, binding = 1) uniform UboModel {
	mat4 model;
	//mat3 normMat;
} uboModel;

//Push constant 
layout (push_constant) uniform PushColour {
	vec4 colour;
	mat3 normMat;
    // vec3 tangent;
} pushColour;



layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoords;

layout(location = 2) out mat3 TBN;      // Tangent vector
layout(location = 5) out vec3 fragPosWorld;
layout(location = 6) out vec3 normalWS; // Use free location (e.g. 3)


void main() {
    // mat4 flipY = mat4(1.0);
    // flipY[1][1] = -1.0;
    // Pass texture coordinates and colour to fragment shader
    texCoords = tex;
    fragColor = pushColour.colour.rgb;



    
    // Compute world-space position of the vertex
    fragPosWorld = vec3(uboModel.model * vec4(inPosition, 1.0));
    
    //Transform normal into world space using the push constant model matrix
    normalWS = normalize(pushColour.normMat * inNormal); // ✅ transform normal
    



    // Compute TBN matrix
    // Transform vectors into world space
    vec3 T = normalize(pushColour.normMat * inTangent); // Tangent from push constant
    vec3 N = normalize(pushColour.normMat * inNormal);
    vec3 B = normalize(cross(N, T)); // Derive bitangent

    TBN = mat3(T, B, N); // Pass TBN matrix to fragment shader



    // Standard MVP Transform calculation
    gl_Position = uboViewProjection.projection * uboViewProjection.view * uboModel.model * vec4(inPosition, 1.0);
}