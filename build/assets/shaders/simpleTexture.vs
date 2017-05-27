// simpleTexture.vert
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec4 vertexColour;

// Output data; will be interpolated for each fragment.
out vec2 fragmentUV;
out vec4 fragmentColour;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vertexPosition_modelspace, 1);

	fragmentUV = vertexUV;
	fragmentColour = vertexColour;
}

