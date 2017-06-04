// simpleTexture.frag
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core
#pragma include "lighting.fs"

in vec2 fragmentUV;
in vec4 fragmentColour;
in vec3 fragmentNormal;
in vec3 fragmentPosition;

// Ouput data
out vec4 colour;

uniform vec3 cameraPosition;

// it's here so we know how to call it.
vec4 applyPointLights(vec3 normal, vec3 fragPosition, vec3 viewDirection, vec4 diffuseSample, vec4 specularSample);
vec4 applySpotLights(vec3 normal, vec3 fragPosition, vec3 viewDirection, vec4 diffuseSample, vec4 specularSample);

void main()
{
	vec3 viewDirection = normalize(cameraPosition - fragmentPosition);
	vec4 base = ambientLightColour * ambientLightIntensity * material.ambientColour;

	vec4 diffSample = texture(material.diffuseTexture, fragmentUV) * material.diffuseColour;
	vec4 specSample = texture(material.specularTexture, fragmentUV) * material.specularColour;

	base += applyPointLights(normalize(fragmentNormal), fragmentPosition, viewDirection, diffSample, specSample);
	base += applySpotLights(normalize(fragmentNormal), fragmentPosition, viewDirection, diffSample, specSample);

	colour = base * fragmentColour * texture(material.diffuseTexture, fragmentUV).rgba;
}








