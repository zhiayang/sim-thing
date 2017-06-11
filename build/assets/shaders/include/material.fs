// material.fs
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// version 330 core

struct Material
{
	vec4 ambientColour;

	vec4 diffuseColour;
	vec4 specularColour;

	sampler2D diffuseTexture;
	sampler2D specularTexture;

	float shine;
};

uniform Material material;
