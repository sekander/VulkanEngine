// #version 450

// layout(location = 1) in vec2 texCoords;
// layout(location = 5) in vec3 fragPosWorld;
// layout(location = 6) in vec3 normalWS;

// layout(location = 0) out vec4 outColor;

// layout(set = 1, binding = 0) uniform sampler2D texSampler;

// layout(push_constant) uniform PushConstants {
// 	vec4 colour;
// 	mat3 normMat;
//     vec3 sdfCenter;   // Light center
//     float time;
//     // float sdfRadius;  // Sphere radius
//     // vec3 lightColor;  // Light color (e.g., white, yellow)
//     // float intensity;  // Light strength
// } push;
// // Hardcoded camera position
// const vec3 CAMERA_POS = vec3(0.0, 0.0, 5.0);

// // Hardcoded directional light
// const vec3 LIGHT_DIR = normalize(vec3(-0.5, -1.0, -0.3));
// const vec3 LIGHT_COLOR = vec3(1.0, 1.0, 1.0); // white light

// // Fog parameters
// const float FOG_DENSITY = 0.04;
// const float FOG_HEIGHT_FALLOFF = 0.08;
// const float FOG_START_Y = 0.0;
// const vec3 FOG_COLOR = vec3(0.6, 0.7, 0.8); // soft blue fog

// // Time for animation (simulate uniform push.time)
// const float TIME = 10.0; // example time in seconds

// // Simple noise function for fog animation
// float noise(vec2 p) {
//     return fract(sin(dot(p ,vec2(12.9898,78.233))) * 43758.5453);
// }

// void main() {
//     // Sample texture color (albedo)
//     vec3 albedo = texture(texSampler, texCoords).rgb;

//     // Lambertian diffuse lighting
//     float diff = max(dot(normalize(normalWS), -LIGHT_DIR), 0.0);
//     vec3 litColor = albedo * diff * LIGHT_COLOR;

//     // Compute distance from camera for fog
//     float dist = length(fragPosWorld - CAMERA_POS);

//     // Animated noise for swirling fog
//     float fogNoise = noise(fragPosWorld.xz * 0.1 + push.time * 0.1);

//     // Fog factor
//     float fogFactor = exp(-FOG_DENSITY * dist * (1.0 + 0.5 * fogNoise));
//     fogFactor *= exp(-FOG_HEIGHT_FALLOFF * max(fragPosWorld.y - FOG_START_Y, 0.0));
//     fogFactor = clamp(fogFactor, 0.0, 1.0);

//     // Mix fog color with lit surface color
//     vec3 finalColor = mix(FOG_COLOR, litColor, fogFactor);

//     outColor = vec4(finalColor, 1.0);
// }


// #version 450

// layout(location = 1) in vec2 texCoords;
// layout(location = 5) in vec3 fragPosWorld;
// layout(location = 6) in vec3 normalWS;

// layout(location = 0) out vec4 outColor;

// layout(set = 1, binding = 0) uniform sampler2D texSampler;

// layout(push_constant) uniform PushConstants {
// 	vec4 colour;
// 	mat3 normMat;
//     vec3 sdfCenter;   
//     float time;
// } push;

// // Hardcoded camera position
// const vec3 CAMERA_POS = vec3(0.0, 0.0, 5.0);

// // Hardcoded directional light
// const vec3 LIGHT_DIR = normalize(vec3(-0.5, -1.0, -0.3));
// const vec3 LIGHT_COLOR = vec3(1.0, 1.0, 1.0); // white light

// // Fog parameters
// const float FOG_DENSITY = 0.05;
// const float FOG_HEIGHT_FALLOFF = 0.07;
// const float FOG_START_Y = 0.0;
// const vec3 FOG_COLOR = vec3(0.6, 0.7, 0.8); // soft blue fog

// // Smooth noise function using sin and cos to simulate smooth swirling fog
// float smoothNoise(vec2 p) {
//     return 0.5 + 0.5 * sin(p.x * 10.0 + cos(p.y * 10.0));
// }

// void main() {
//     // Sample texture color (albedo)
//     vec3 albedo = texture(texSampler, texCoords).rgb;

//     // Lambertian diffuse lighting
//     float diff = max(dot(normalize(normalWS), -LIGHT_DIR), 0.0);
//     vec3 litColor = albedo * diff * LIGHT_COLOR;

//     // Distance from camera for fog intensity
//     float dist = length(fragPosWorld - CAMERA_POS);

//     // Animate fog swirling offsets differently for X and Z
//     vec2 fogCoords = fragPosWorld.xz * 0.15; 

//     // Offset coords by time to animate fog movement
//     fogCoords.x += push.time * 0.3;
//     fogCoords.y += push.time * 0.5;

//     // Vertical oscillation to simulate fog "breathing"
//     float heightOsc = 0.5 + 0.5 * sin(push.time * 1.5 + fragPosWorld.y * 5.0);

//     // Calculate smooth noise for fog density modulation
//     float fogNoise = smoothNoise(fogCoords) * heightOsc;

//     // Fog factor with noise affecting density and height falloff
//     float fogFactor = exp(-FOG_DENSITY * dist * (1.0 + 0.7 * fogNoise));
//     fogFactor *= exp(-FOG_HEIGHT_FALLOFF * max(fragPosWorld.y - FOG_START_Y, 0.0));
//     fogFactor = clamp(fogFactor, 0.0, 1.0);

//     // Mix fog color with lit surface color based on fog factor
//     vec3 finalColor = mix(FOG_COLOR, litColor, fogFactor);

//     outColor = vec4(finalColor, 1.0);
// }


// #version 450

// layout(location = 1) in vec2 texCoords;
// layout(location = 5) in vec3 fragPosWorld;
// layout(location = 6) in vec3 normalWS;

// layout(location = 0) out vec4 outColor;

// layout(set = 1, binding = 0) uniform sampler2D texSampler;

// layout(push_constant) uniform PushConstants {
//     vec4 colour;
//     mat3 normMat;
//     vec3 sdfCenter;
//     float time;
// } push;

// const vec3 CAMERA_POS = vec3(0.0, 0.0, 5.0);
// const vec3 LIGHT_DIR = normalize(vec3(-0.5, -1.0, -0.3));
// const vec3 LIGHT_COLOR = vec3(1.0, 1.0, 1.0);

// const float FOG_DENSITY = 0.04;
// const float FOG_HEIGHT_FALLOFF = 0.08;
// const float FOG_START_Y = 0.0;
// const vec3 FOG_COLOR = vec3(0.6, 0.7, 0.8);

// // Simple noise function
// float noise(vec2 p) {
//     return fract(sin(dot(p ,vec2(12.9898,78.233))) * 43758.5453);
// }

// // Bilinear interpolation smooth noise
// float smoothNoise(vec2 uv) {
//     vec2 i = floor(uv);
//     vec2 f = fract(uv);

//     float a = noise(i);
//     float b = noise(i + vec2(1.0, 0.0));
//     float c = noise(i + vec2(0.0, 1.0));
//     float d = noise(i + vec2(1.0, 1.0));

//     vec2 u = f * f * (3.0 - 2.0 * f);

//     return mix(a, b, u.x) +
//            (c - a)* u.y * (1.0 - u.x) +
//            (d - b) * u.x * u.y;
// }

// void main() {
//     vec3 albedo = texture(texSampler, texCoords).rgb;

//     float diff = max(dot(normalize(normalWS), -LIGHT_DIR), 0.0);
//     vec3 litColor = albedo * diff * LIGHT_COLOR;

//     float dist = length(fragPosWorld - CAMERA_POS);

//     // Animate fogCoords to move the fog over time visibly
//     vec2 fogCoords = fragPosWorld.xz * 0.15;

//     // Stronger movement offsets:
//     float moveSpeedX = 0.5;
//     float moveSpeedY = 0.7;
//     vec2 moveOffset = vec2(push.time * moveSpeedX, push.time * moveSpeedY);

//     // Rotate fogCoords for swirl effect
//     float angle = push.time * 0.3; // faster rotation for better visibility
//     float cosA = cos(angle);
//     float sinA = sin(angle);
//     mat2 rotation = mat2(cosA, -sinA, sinA, cosA);

//     fogCoords = rotation * (fogCoords + moveOffset);

//     float heightOsc = 0.5 + 0.5 * sin(push.time * 2.0 + fragPosWorld.y * 5.0);

//     float fogNoise = smoothNoise(fogCoords) * heightOsc;

//     float fogFactor = exp(-FOG_DENSITY * dist * (1.0 + 0.7 * fogNoise));
//     fogFactor *= exp(-FOG_HEIGHT_FALLOFF * max(fragPosWorld.y - FOG_START_Y, 0.0));
//     fogFactor = clamp(fogFactor, 0.0, 1.0);

//     vec3 finalColor = mix(FOG_COLOR, litColor, fogFactor);

//     outColor = vec4(finalColor, 1.0);
// }


#version 450

layout(location = 1) in vec2 texCoords;
layout(location = 5) in vec3 fragPosWorld;
layout(location = 6) in vec3 normalWS;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(push_constant) uniform PushConstants {
    vec4 colour;
    mat3 normMat;
    vec3 sdfCenter;
    float time;
} push;

const vec3 CAMERA_POS = vec3(0.0, 0.0, 5.0);
const vec3 LIGHT_DIR = normalize(vec3(-0.5, -1.0, -0.3));
const vec3 LIGHT_COLOR = vec3(1.0, 1.0, 1.0);

const vec3 FOG_COLOR = vec3(0.6, 0.7, 0.8);

// Base fog parameters (will be modulated by time)
const float BASE_FOG_DENSITY = 0.04;
const float BASE_FOG_HEIGHT_FALLOFF = 0.08;
const float BASE_FOG_START_Y = 0.0;

// Simple noise function for a little detail (optional)
float noise(vec2 p) {
    return fract(sin(dot(p ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec3 albedo = texture(texSampler, texCoords).rgb;

    float diff = max(dot(normalize(normalWS), -LIGHT_DIR), 0.0);
    vec3 litColor = albedo * diff * LIGHT_COLOR;

    float dist = length(fragPosWorld - CAMERA_POS);

    // Animate fog density and height start with sine waves
    float fogDensity = BASE_FOG_DENSITY * (0.75 + 0.5 * sin(push.time * 1.5));
    float fogHeightFalloff = BASE_FOG_HEIGHT_FALLOFF * (0.5 + 0.7 * cos(push.time * 1.0));
    float fogStartY = BASE_FOG_START_Y + 0.5 * sin(push.time * 0.7);

    // Use noise modulated fog factor for some texture
    float fogNoise = noise(fragPosWorld.xz * 0.1 + push.time * 0.5);

    // Calculate fog factor (exponential falloff with distance and height)
    float fogFactor = exp(-fogDensity * dist * (1.0 + 0.3 * fogNoise));
    fogFactor *= exp(-fogHeightFalloff * max(fragPosWorld.y - fogStartY, 0.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(FOG_COLOR, litColor, fogFactor);

    outColor = vec4(finalColor, 1.0);
}
