// simpleTexture.frag
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core
#pragma include "include/material.fs"

in vec2 fragmentUV;
in vec4 fragmentColour;
in vec3 fragmentNormal;
in vec3 fragmentPosition;

// Ouput data
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gSpecular;

uniform vec3 cameraPosition;

void main()
{
	gPosition	= vec4(fragmentPosition, 1);
	gNormal		= vec4(normalize(fragmentNormal), 1);
	gDiffuse	= texture(material.diffuseTexture, fragmentUV) * fragmentColour * material.diffuseColour;
	gSpecular	= texture(material.specularTexture, fragmentUV) * material.specularColour;
}








