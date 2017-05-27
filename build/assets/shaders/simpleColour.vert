#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColour;

// Output data; will be interpolated for each fragment.
out vec4 fragmentColour;

// Values that stay constant for the whole mesh.
uniform mat4 modelViewProjectionMatrix;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition_modelspace, 1);

	// The color of each vertex will be interpolated
	// to produce the color of each fragment
	fragmentColour = vertexColour;
}

