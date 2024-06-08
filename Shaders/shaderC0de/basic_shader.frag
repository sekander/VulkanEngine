#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    // Screen coordinates (normalized)
    vec2 uv = gl_FragCoord.xy / vec2(800, 600);

    // Line parameters
    vec2 startPoint = vec2(0.2, 0.2);
    vec2 endPoint = vec2(0.8, 0.8);
    float lineWidth = 0.02;

    // Calculate distance from current fragment to the line
    float distance = abs(dot(normalize(endPoint - startPoint), uv - startPoint));

    // Draw line if distance within line width
    float alpha = smoothstep(lineWidth - 0.005, lineWidth + 0.005, distance);

    // Set color
    //fragColor = vec4(1.0, 0.0, 0.0, alpha); // Red line



    //outColor = vec4(fragColor, 1.0);
    outColor = vec4(fragColor, alpha);
}
