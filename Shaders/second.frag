#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(input_attachment_index = 0, binding = 0) uniform subpassInput inputColour;   //Colour output
layout(input_attachment_index = 1, binding = 1) uniform subpassInput inputDepth;    //Depth output

layout(location = 0) out vec4 colour;

void main()
{
    colour = subpassLoad(inputColour).rgba;
}