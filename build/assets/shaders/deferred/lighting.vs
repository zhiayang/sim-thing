// simpleTexture.vert
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 3) in vec2 vertexUV;

// Output data; will be interpolated for each fragment.
out vec2 fragmentUV;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = vec4(vertexPosition, 1);
	fragmentUV = vertexUV;
}

