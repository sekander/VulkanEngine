#version 450

layout(triangles) in;
layout(line_strip, max_vertices = 2) out;
//layout(line_strip, max_vertices = 6) out;

layout(location = 0) in vec3 fragNormal[];   // Input normal from vertex shader

void main() {
    // Loop through each triangle
    //for (int i = 0; i < gl_in.length(); ++i) {
        // Emit vertices to draw normals as lines
        for (int j = 0; j < 3; ++j) {
            //gl_Position = gl_in[i].gl_Position;
            gl_Position = gl_in[j].gl_Position;
            EmitVertex();
            
            // Draw line representing normal
            //gl_Position = gl_in[i].gl_Position + vec4(fragNormal[j], 0.0) * 0.1; // Scale normal length by 0.1 for visualization
            gl_Position = gl_in[j].gl_Position + vec4(fragNormal[j], 0.0) * 0.1; // Scale normal length by 0.1 for visualization
            
            // Calculate end of line (vertex position extended by normal)
            //vec3 normalEnd = gl_in[j].gl_Position.xyz + fragNormal[j]; // End position is current vertex position + normal direction
            //gl_Position = vec4(normalEnd, 1.0);
            
            EmitVertex();
        
            EndPrimitive();
        }
        //EndPrimitive();
    //}
}

