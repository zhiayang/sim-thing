// textShader.vert
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 fragmentColour;

uniform mat4 projectionMatrix;

uniform vec2 offsetPosition;
uniform vec4 fontColour;
uniform float fontScale;

void main()
{
	vec2 vert = offsetPosition + (fontScale * vertexPosition);
	gl_Position = projectionMatrix * vec4(vert, 0, 1);

	// UV of the vertex. No special space for this one.
	UV = vertexUV;
	fragmentColour = fontColour;
}
