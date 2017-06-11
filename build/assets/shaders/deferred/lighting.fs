// simpleTexture.frag
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core
#pragma include "include/lighting.fs"

in vec2 fragmentUV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

// Ouput data
out vec4 colour;

uniform vec3 cameraPosition;

// it's here so we know how to call it.
vec4 applyPointLights(vec3 normal, vec3 fragPosition, vec3 viewDirection, vec4 diffuseSample, vec4 specularSample, float shine);
vec4 applySpotLights(vec3 normal, vec3 fragPosition, vec3 viewDirection, vec4 diffuseSample, vec4 specularSample, float shine);

void main()
{
	vec3 position = texture(gPosition, fragmentUV).rgb;
	vec3 normal = texture(gNormal, fragmentUV).rgb;
	vec4 diffuse = texture(gDiffuse, fragmentUV);
	vec4 specular = texture(gSpecular, fragmentUV);

	vec3 viewDirection = normalize(cameraPosition - position);
	vec4 base = ambientLightColour * ambientLightIntensity * diffuse;

	base += applyPointLights(normalize(normal), position, viewDirection, diffuse, specular, 16);
	base += applySpotLights(normalize(normal), position, viewDirection, diffuse, specular, 16);

	colour = vec4(0, 0.5, 1, 1);
}








