#version 450

layout(triangles) in;
layout(line_strip, max_vertices = 6) out; // Draw lines, so max_vertices = 6 (two vertices per line)

layout(location = 0) in vec3 fragColor[];   // Input color from vertex shader
layout(location = 1) in vec3 fragNormal[];  // Input normal from vertex shader

void main() {
    // Loop through each triangle (input primitive)
    for (int i = 0; i < gl_in.length(); ++i) {
        // Emit lines for each vertex normal
        for (int j = 0; j < 3; ++j) {
            // Start point of the line (vertex position)
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();

            // End point of the line (vertex position + normal direction)
            gl_Position = gl_in[i].gl_Position + vec4(fragNormal[i * 3 + j], 0.0) * 0.1; // Scale by 0.1 for visualization
            EmitVertex();

            // End the line
            EndPrimitive();
        }
    }
}
