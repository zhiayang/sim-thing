// simpleColour.frag
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

in vec4 fragmentColour;
in vec3 fragmentNormal;
in vec3 fragmentPosition;


// Ouput data
out vec4 colour;


// lighting nonsense
uniform vec4 ambientLightColour;
uniform float ambientLightIntensity;

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

uniform vec3 cameraPosition;

vec4 applyPointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection);

void main()
{
	vec3 viewDirection = normalize(cameraPosition - fragmentPosition);
	vec4 result = ambientLightColour * ambientLightIntensity;

	for(int i = 0; i < pointLightCount; i++)
	{
		result += applyPointLight(pointLights[i], normalize(fragmentNormal), fragmentPosition, viewDirection);
	}

	colour = result * fragmentColour;
}






// point lighting
vec4 applyPointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDirection)
{
	vec3 lightDir = normalize(light.position - fragPosition);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// // Specular shading
	// vec3 reflectDir = reflect(-1 * lightDir, normal);
	// float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.shininess);

	// Attenuation
	float dist = length(light.position - fragPosition);

	float attenuation = 1.0 / (light.constantFactor + light.linearFactor * dist + light.quadFactor * (dist * dist));

	// Combine results
	// vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, TexCoords));
	// vec3 diffuse = light.diffuse * diff;// * vec3(texture(material.diffuse, TexCoords));
	// vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	vec4 diffuse = light.diffuseColour * diff;

	diffuse *= vec4(vec3(attenuation), 1.0);
	// specular *= attenuation;

	return diffuse * light.intensity;
}



















