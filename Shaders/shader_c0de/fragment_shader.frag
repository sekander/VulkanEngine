#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in mat3 TBN;
layout(location = 5) in vec3 fragPosWorld;
layout(location = 6) in vec3 normalWS;

// layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D textureSampler;
layout(set = 1, binding = 1) uniform sampler2D normalMap;


void main() {

    //Testing colour 
    // outColor = vec4(fragColor, 1.0);

    // outColor = vec4(color, 1.0);

    //Testing texture
    //Testing out normal map
    // outColor = vec4(fragColor, 1.0) * texture(textureSampler, texCoords);
    // outColor = vec4(fragColor, 1.0) * texture(normalMap, texCoords);
    
    // outColor = vec4(normalize(normalWS) * 0.5 + 0.5, 1.0);
    // outColor = vec4(1.0, 0.0, 0.0, 1.0); // Red normals
    //Basic Phong lighting
    


//     // --- Tangent space normal map ---
    // TBN[0] = tangent
    // TBN[1] = bitangent
    // TBN[2] = normal

    // vec3 T = normalize(TBN[0]);
    // vec3 B = normalize(TBN[1]);
    // vec3 N = normalize(TBN[2]);

    // Choose what to visualize
    // outColor = vec4(T * 0.5 + 0.5, 1.0);  // Tangent in RGB
    // outColor = vec4(B * 0.5 + 0.5, 1.0);  // Bitangent in RGB
    // outColor = vec4(N * 0.5 + 0.5, 1.0);    // Normal in RGB


        // --- Hardcoded light and camera data ---
    vec3 lightPos = vec3(10.0, 10.0, 10.0);
    vec3 viewPos = vec3(0.0, 0.0, 5.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // --- Sample normal from normal map and convert to [-1, 1] ---
    vec3 normalTS = texture(normalMap, texCoords).rgb;
    normalTS = normalize(normalTS * 2.0 - 1.0);

    // --- Convert tangent-space normal to world space using TBN matrix ---
    vec3 N = normalize(TBN * normalTS);

    // --- Compute vectors for lighting ---
    vec3 L = normalize(lightPos - fragPosWorld); // Light direction
    vec3 V = normalize(viewPos - fragPosWorld);  // View direction
    vec3 R = reflect(-L, N);                     // Reflected light

    // --- Phong lighting components ---
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor;

    float spec = pow(max(dot(R, V), 0.0), 32.0);
    vec3 specular = spec * vec3(1.0);

    // --- Texture color (albedo) ---
    vec3 albedo = texture(textureSampler, texCoords).rgb;

    // --- Combine lighting with texture and vertex colour ---
    vec3 finalColor = (ambient + diffuse + specular) * albedo * fragColor;

    outColor = vec4(finalColor, 1.0);

}