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


struct Material
{
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;

	float shininess;
};

uniform Material material;

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

	// Diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular lighting
	vec3 reflectDir = reflect(-1 * lightDir, normal);
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 32/* material.shininess */);

	// Attenuation
	float dist = length(light.position - fragPosition);

	float _att = 1.0 / (light.constantFactor + light.linearFactor * dist + light.quadFactor * (dist * dist));
	vec4 atten = vec4(_att, _att, _att, 1.0);

	// Combine results
	// vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, TexCoords));
	// vec3 diffuse = light.diffuse * diff;// * vec3(texture(material.diffuse, TexCoords));
	// vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	vec4 diffuse = light.diffuseColour * diff;
	vec4 specular = light.specularColour * 0.5 * spec;

	return (diffuse + specular) * light.intensity * atten;
}




















