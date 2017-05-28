// simpleTexture.frag
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core
#pragma include "lighting.inc"

in vec2 fragmentUV;
in vec4 fragmentColour;
in vec3 fragmentNormal;
in vec3 fragmentPosition;

// Ouput data
out vec4 colour;

struct Material
{
	vec4 ambientColour;

	bool isTextured;

	vec4 diffuseColour;
	vec4 specularColour;

	sampler2D diffuseTexture;
	sampler2D specularTexture;

	float shininess;
};

uniform Material material;
uniform vec3 cameraPosition;
uniform sampler2D textureSampler;

// it's here so we know how to call it.
vec4 applyPointLights(vec3 normal, vec3 fragPosition, vec3 viewDirection);

void main()
{
	vec3 viewDirection = normalize(cameraPosition - fragmentPosition);
	vec4 base = ambientLightColour * ambientLightIntensity;

	base += applyPointLights(normalize(fragmentNormal), fragmentPosition, viewDirection);

	colour = base * fragmentColour * texture(textureSampler, fragmentUV).rgba;
}








