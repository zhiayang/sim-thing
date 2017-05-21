#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColour;

// Ouput data
out vec3 colour;

void main()
{
	colour = fragmentColour;
}
