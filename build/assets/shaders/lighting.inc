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

// lighting nonsense
uniform vec4 ambientLightColour;
uniform float ambientLightIntensity;

#define MAX_POINT_LIGHTS 16
uniform int pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];



// point lighting
vec4 applyPointLights(vec3 normal, vec3 fragPosition, vec3 viewDirection)
{
	for(int i = 0; i < pointLightCount; i++)
	{
		vec3 lightDir = normalize(pointLights[i].position - fragPosition);

		// Diffuse lighting
		float diff = max(dot(normal, lightDir), 0.0);

		// Specular lighting
		vec3 reflectDir = reflect(-1 * lightDir, normal);
		float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32/* material.shininess */);

		// Attenuation
		float dist = length(pointLights[i].position - fragPosition);

		float _att = 1.0 / (pointLights[i].constantFactor + pointLights[i].linearFactor * dist + pointLights[i].quadFactor * (dist * dist));
		vec4 atten = vec4(_att, _att, _att, 1.0);

		// Combine results
		// vec3 ambient = pointLights[i].ambient;// * vec3(texture(material.diffuse, TexCoords));
		// vec3 diffuse = pointLights[i].diffuse * diff;// * vec3(texture(material.diffuse, TexCoords));
		// vec3 specular = pointLights[i].specular * spec * vec3(texture(material.specular, TexCoords));

		vec4 diffuse = pointLights[i].diffuseColour * diff;
		vec4 specular = pointLights[i].specularColour * 0.5 * spec;

		return (diffuse + specular) * pointLights[i].intensity * atten;
	}
}




