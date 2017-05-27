#version 330 core

// Interpolated values from the vertex shaders
in vec2 fragmentUV;
in vec4 fragmentColour;

// Ouput data
out vec4 colour;

uniform sampler2D textureSampler;

void main()
{
	colour = texture(textureSampler, fragmentUV).rgba * fragmentColour;
}
