// textShader.frag
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 colour;

uniform sampler2D textureSampler;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureSampler, UV).r);

    colour = vec4(vec3(1.0, 1.0, 1.0), 1.0) * sampled;
}
