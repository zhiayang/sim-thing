#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 colour;

uniform sampler2D textureSampler;

void main()
{
	// colour = fragmentColour;
	colour = texture(textureSampler, UV).rgba;
}
