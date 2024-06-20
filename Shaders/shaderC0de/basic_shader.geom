#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 fragColor[];   // Input color from vertex shader
layout(location = 0) out vec3 geomColor;    // Output color to fragment shader

void main() {
    // Loop through each vertex of the input triangle
    for (int i = 0; i < gl_in.length(); ++i) {
        // Output the vertex position unchanged
        gl_Position = gl_in[i].gl_Position;

        // Pass through the color to the fragment shader
        geomColor = fragColor[i];

        // Emit this vertex to the next stage (fragment shader)
        EmitVertex();
    }

    // End the triangle strip
    EndPrimitive();
}
