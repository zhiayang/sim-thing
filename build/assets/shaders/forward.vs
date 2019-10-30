// simpleTexture.vert
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColour;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexUV;

// Output data; will be interpolated for each fragment.
out vec2 fragmentUV;
out vec3 fragmentNormal;
out vec4 fragmentColour;
out vec3 fragmentPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 transposedInverseModelMatrix;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1);

	fragmentUV = vertexUV;
	fragmentNormal = transposedInverseModelMatrix * vertexNormal;
	fragmentColour = vertexColour;
	fragmentPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
}

