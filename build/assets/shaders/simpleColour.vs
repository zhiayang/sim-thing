// simpleColour.vert
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColour;
layout(location = 2) in vec3 vertexNormal;

// Output data; will be interpolated for each fragment.
out vec4 fragmentColour;
out vec3 fragmentNormal;
out vec3 fragmentPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1);

	fragmentColour = vertexColour;
	fragmentNormal = vertexNormal;
	fragmentPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
}

