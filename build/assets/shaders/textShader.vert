#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * vec4(vertexPosition, 0, 1);

	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}
