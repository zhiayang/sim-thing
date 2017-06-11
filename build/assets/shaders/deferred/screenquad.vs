// screenquad.vs
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColour;
layout(location = 3) in vec2 vertexUV;

// Output data; will be interpolated for each fragment.
out vec2 fragmentUV;
out vec4 fragmentColour;


void main()
{
	gl_Position = vec4(vertexPosition.xy, 0, 1);
	fragmentUV = vertexUV;
	fragmentColour = vertexColour;
}
