#version 450

layout(triangles)   in; // Input primitive type
layout(line_strip, max_vertices = 6) out; // Output primitive type


layout(set = 0, binding = 0) uniform UboViewProjection {
    mat4 projection;
    mat4 view;
    vec4 cameraPos;
} uboViewProjection;

layout(location = 5) in vec3 fragPosWorld[];
layout(location = 6) in vec3 normalWS[];

void main() {
    for (int i = 0; i < 3; ++i) {
        vec3 p = fragPosWorld[i];
        vec3 n = normalWS[i];

        vec4 start = uboViewProjection.projection * uboViewProjection.view * vec4(p, 1.0);
        vec4 end = uboViewProjection.projection * uboViewProjection.view * vec4(p + n * 0.1, 1.0); // adjust length

        gl_Position = start;
        EmitVertex();

        gl_Position = end;
        EmitVertex();

        EndPrimitive();
    }
}