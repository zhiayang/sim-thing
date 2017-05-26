#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 colour;

uniform sampler2D textureSampler;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureSampler, UV).r);

    colour = vec4(vec3(1.0, 1.0, 1.0), 1.0) * sampled;
}
