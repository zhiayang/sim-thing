#version 140 //core

// Input vertex data, different for all executions of this shader.
/* layout(location = 0) */ in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 combinedMatrix;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = combinedMatrix * vec4(vertexPosition_modelspace, 1);
}
