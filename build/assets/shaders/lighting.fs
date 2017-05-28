// lighting.inc
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

// version 330 core

struct PointLight
{
	vec3 position;
	float intensity;

	vec4 diffuseColour;
	vec4 specularColour;

	float constantFactor;
	float linearFactor;
	float quadFactor;
};

#define MAX_POINT_LIGHTS 16
uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];


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


// lighting nonsense
uniform vec4 ambientLightColour;
uniform float ambientLightIntensity;



// point lighting
vec4 applyPointLights(vec3 normal, vec3 fragPosition, vec3 viewDirection, vec2 fragUV)
{
	vec4 result = vec4(0, 0, 0, 1);
	for(int i = 0; i < pointLightCount; i++)
	{
		vec3 lightDir = normalize(pointLights[i].position - fragPosition);

		// Diffuse lighting
		float diff = max(dot(normal, lightDir), 0.0);

		// Specular lighting
		vec3 reflectDir = reflect(-1 * lightDir, normal);
		float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shine);

		// Attenuation
		float dist = length(pointLights[i].position - fragPosition);

		float _att = 1.0 / (pointLights[i].constantFactor + pointLights[i].linearFactor * dist + pointLights[i].quadFactor * (dist * dist));
		vec4 atten = vec4(_att, _att, _att, 1.0);

		// Combine results
		vec4 diffuse = pointLights[i].diffuseColour * diff * texture(material.diffuseTexture, fragUV);
		vec4 specular = pointLights[i].specularColour * 0.5 * spec * texture(material.specularTexture, fragUV);

		result += (diffuse + specular) * pointLights[i].intensity * atten;
	}

	return result;
}








