// screenquad.fs
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

in vec2 fragmentUV;
in vec4 fragmentColour;

// Ouput data
out vec4 colour;

uniform sampler2D textureSampler;

void main()
{
	colour = vec4(texture(textureSampler, fragmentUV).rgb, 1.0) * fragmentColour;
}
