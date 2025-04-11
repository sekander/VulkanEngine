#version 450

layout(triangles) in;
//layout(line_strip, max_vertices = 12) out; // Output lines for wireframe grid

layout(line_strip, max_vertices = 90) out;
layout(location = 0) in vec3 fragPosition[]; // Input vertex positions from vertex shader

void main() {
    for(int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
}
